#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <imgui-SFML.h>
#include <imgui.h>

#include <windows.h>
#include <commdlg.h>

#ifdef _MSC_VER
#ifndef _DEBUG  
#pragma comment(linker, "/SUBSYSTEM:windows")
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#endif
#endif

enum {
	width,
	height
};

void SeparatorText(const char* label);
void saveFile(int projectSize[], std::vector<sf::RectangleShape>& strokes, char fileName[]);
void draw(const sf::Vector2f& startPoint, const sf::Vector2f& endPoint, std::vector<sf::RectangleShape>& pixels,
	const sf::Vector2f& pix_size, const sf::Color& color, std::vector<sf::RectangleShape>& temp);

int main() {
	bool isctrlopressed = true;
	bool spriteOn = true;
	char* hideOrshow = "Hide Image";
	int windowSize[2] = { 600,600 }; // 600,600 by default
	int projectSize[2] = { 0,0 };
	char fileName[256] = "";
	bool showWin = false;
	float zoomSpeed = 0.1f;
	float moveSpeed = 400.f;
	bool countHideOrShow = false;
	bool hasImage = false;
	float zoomFactor = 0;
	bool showControls = false;
	char* hideOrshowCtrls = "Show Controls";
	bool countCtrl = false;
	bool inFocus = true;

	bool hasClicked = true;
	sf::Vector2f firstPos;

	sf::Vector2f canvasMovement = { 640,360 };

	OPENFILENAME ofn;
	char openFile[MAX_PATH] = "";

	//Open file
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // No specific owner window
	ofn.lpstrFilter = "PNG Images\0*.png\0JPEG Images\0*.jpg";
	ofn.lpstrFile = openFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	sf::Texture texture;

	sf::RenderWindow window(sf::VideoMode(windowSize[width], windowSize[height]), "Gody Art");
	window.setFramerateLimit(144);
	ImGui::SFML::Init(window);
	bool my_tool_active = true;
	float my_color[3] = { 0.f,0.f,0.f };
	bool clear = false;
	sf::Vector2f pix_size = { 10,10 };
	float single_pix_size = 20;
	std::vector<std::vector<sf::RectangleShape>> undo;
	std::vector<sf::RectangleShape> temp;
	std::vector<sf::RectangleShape> strokes;

	std::vector<sf::Vector2f> posHistory(2);

	undo.reserve(10000);
	strokes.reserve(10000);
	sf::RectangleShape bg(sf::Vector2f(0, 0));
	bg.setFillColor(sf::Color::White);

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	bool isctrlzpressed = false;

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			//Zoom in and out
			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					float delta = event.mouseWheelScroll.delta;
					zoomFactor = 1.f + delta * zoomSpeed;

					if (zoomFactor > 0) {
						sf::View view = window.getView();
						view.setCenter(canvasMovement);
						view.zoom(1.f / zoomFactor);
						window.setView(view);
					}
				}
			}

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::GainedFocus) {
				inFocus = true;
			}
			else if (event.type == sf::Event::LostFocus) {
				inFocus = false;
			}
		}

		//Delta Time
		sf::Time dt = deltaClock.restart();
		float deltaTime = dt.asSeconds();

		ImGui::SFML::Update(window, dt);

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		if (!showWin) {
			ImGui::Begin("Project Configuration");
			SeparatorText("Project Size");
			ImGui::InputInt2("", projectSize);
			SeparatorText("Size Presets (Recommended)");
			if (ImGui::Button("64x64")) {
				projectSize[width] = 64;
				projectSize[height] = 64;
			}
			if (ImGui::Button("128x128")) {
				projectSize[width] = 128;
				projectSize[height] = 128;
			}
			if (ImGui::Button("256x256")) {
				projectSize[width] = 256;
				projectSize[height] = 256;
			}
			SeparatorText("Project Name");
			if (ImGui::InputText("", fileName, 256)) {
			}
			if (ImGui::Button("Create")) {
				//Set window size to 720p and center it
				windowSize[width] = 1280;
				windowSize[height] = 720;

				window.setSize(sf::Vector2u(windowSize[width], windowSize[height]));

				sf::View view = window.getView();
				view.setSize(static_cast<float>(windowSize[width]), static_cast<float>(windowSize[height]));
				view.setCenter(windowSize[width] / 2.f, windowSize[height] / 2.f);
				window.setView(view);
				bg.setSize(sf::Vector2f(windowSize[width], windowSize[height]));

				window.setPosition(sf::Vector2i(
					(desktop.width - window.getSize().x) / 2,
					(desktop.height - window.getSize().y) / 2
				));
				//Set project size
				bg.setSize(sf::Vector2f(projectSize[width], projectSize[height]));
				bg.setOrigin(bg.getLocalBounds().width / 2, bg.getLocalBounds().height / 2);
				bg.setPosition({ 640, 360 });
				showWin = true;
			}
			ImGui::End();
		}

		ImGui::SetNextWindowSize(
			ImVec2(320, 170), ImGuiCond_Always
		);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		if (showWin) {
			ImGui::Begin("Pixel Editor", &my_tool_active, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Project Settings"))
				{
					if (ImGui::MenuItem("Open", "Ctrl+O")) {
						spriteOn = true;
						if (GetOpenFileName(&ofn)) {
							std::cout << "Selected file: " << openFile << std::endl;
							hasImage = true;
							texture.loadFromFile(openFile);
						}
						else {
							std::cout << "No file selected or dialog canceled." << std::endl;
						}
					}
					if (ImGui::MenuItem("Save", "Ctrl+S")) {
						saveFile(projectSize, strokes, fileName);
					}
					if (ImGui::MenuItem("Close", "Ctrl+W")) {
						if (MessageBoxA(NULL, "Are you sure?", "Exit", MB_YESNO | MB_ICONEXCLAMATION) == 6) {
							ImGui::SFML::Shutdown(); return 0;
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::ColorEdit3("Color", my_color);
			if (ImGui::Button("Clear")) {
				clear = true;
			}

			if (hasImage) {
				if (ImGui::Button(hideOrshow)) {
					if (countHideOrShow == false) {
						hideOrshow = "Show Image";
						countHideOrShow = true;
						spriteOn = false;
					}
					else {
						hideOrshow = "Hide Image";
						countHideOrShow = false;
						spriteOn = true;
					}
				}
			}

			if (ImGui::InputFloat("px", &single_pix_size)) {
				pix_size = { single_pix_size, single_pix_size };
			}

			if (ImGui::Button(hideOrshowCtrls)) {
				if (countCtrl == false) {
					showControls = true;
					hideOrshowCtrls = "Hide Controls";
					countCtrl = true;
				}
				else {
					showControls = false;
					hideOrshowCtrls = "Show Controls";
					countCtrl = false;
				}
			}

			ImGui::End();
		}

		if (showControls) {
			ImGui::SetNextWindowSize(ImVec2(550, 170), ImGuiCond_Appearing);
			ImGui::SetNextWindowPos(ImVec2(320, 0), ImGuiCond_Appearing);
			ImGui::Begin("Controls");
			ImGui::Text("LControl + O = Open");
			ImGui::Text("LControl + S = Save");
			ImGui::Text("LControl + W = Quit");
			ImGui::Text("WASD = Move in canvas");
			ImGui::Text("LShift = Precision Mode (Slows movement speed in canvas)");

			ImGui::End();
		}

		sf::Color color(
			static_cast<sf::Uint8>(my_color[0] * 255),
			static_cast<sf::Uint8>(my_color[1] * 255),
			static_cast<sf::Uint8>(my_color[2] * 255)
		);

		//Opened image
		sf::Sprite sprite(texture);
		sf::Vector2f spriteSize = { sprite.getLocalBounds().width, sprite.getLocalBounds().height };
		sprite.setOrigin({ spriteSize.x / 2, spriteSize.y / 2 });
		sprite.setPosition(640, 360);

		float scaleFactor = 1.0f;
		if (spriteSize.x > projectSize[width] || spriteSize.y > projectSize[height]) {
			float scaleX = projectSize[width] / spriteSize.x;
			float scaleY = projectSize[height] / spriteSize.y;
			scaleFactor = std::min(scaleX, scaleY);
		}

		sprite.scale(scaleFactor, scaleFactor);

		//Draw
		if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Left) && inFocus) {
			sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

			// Snap to grid in world coords
			mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pix_size.x) * pix_size.x + pix_size.x / 2.f;
			mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pix_size.y) * pix_size.y + pix_size.y / 2.f;

			if (hasClicked) {
				firstPos = mouseWorldPos;
				hasClicked = false;
			}
			else {
				if (bg.getGlobalBounds().contains(mouseWorldPos)) {
					draw(firstPos, mouseWorldPos, strokes, pix_size, color, temp);
				}

				firstPos = mouseWorldPos;
			}
		}
		else if (!temp.empty()) {
			undo.push_back(temp);
			temp.clear();
		}
		else {
			hasClicked = true;
		}

		//Erase
		/*if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

			// Snap to grid in world coords
			mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pix_size.x) * pix_size.x + pix_size.x / 2.f;
			mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pix_size.y) * pix_size.y + pix_size.y / 2.f;

			for (auto& pixel : strokes) {
				if (pixel.getPosition() == mouseWorldPos) {
					pixel.setFillColor(sf::Color::Transparent);
				}
			}
		}*/

		//Clears screen
		if (clear == true) {
			strokes.clear();
			undo.clear();
			hasImage = false;
			spriteOn = false;
			hideOrshow = "Hide image";
			countHideOrShow = false;
			clear = false;
		}

		//Undo CTRL+Z
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) && !isctrlzpressed) {
			if (!temp.empty()) {
				undo.push_back(temp);
				temp.clear();
			}

			if (!undo.empty()) {
				std::vector<sf::RectangleShape>& lastStroke = undo.back();
				if (strokes.size() >= lastStroke.size()) {
					strokes.resize(strokes.size() - lastStroke.size());
					undo.pop_back();
				}
			}
			isctrlzpressed = true;
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
			isctrlzpressed = false;
		}
		//Close CTRL+W
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && showWin) {
			if (MessageBoxA(NULL, "Are you sure?", "Exit", MB_YESNO | MB_ICONEXCLAMATION) == 6) {
				ImGui::SFML::Shutdown(); return 0;
			}
		}

		//Save to file CTRL+S
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && showWin) {
			saveFile(projectSize, strokes, fileName);
		}
		//Open file CTRL+O
		if (!isctrlopressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O) && showWin) {
			spriteOn = true;
			if (GetOpenFileName(&ofn)) {
				std::cout << "Selected file: " << openFile << std::endl;
				hasImage = true;
				texture.loadFromFile(openFile);
			}
			else {
				std::cout << "No file selected or dialog canceled." << std::endl;
			}
			isctrlopressed = true;
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) {
			isctrlopressed = false;
		}

		//Move around in the canvas

		sf::View view = window.getView();

		if (inFocus) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
				moveSpeed = 100.f;
			}
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
				moveSpeed = 400.f;
			}

			sf::View view = window.getView();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
				canvasMovement.y += -moveSpeed * deltaTime;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
				canvasMovement.y += moveSpeed * deltaTime;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				canvasMovement.x += -moveSpeed * deltaTime;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				canvasMovement.x += moveSpeed * deltaTime;
			}
		}

		view.setCenter(canvasMovement);
		window.setView(view);

		window.clear();
		window.draw(bg);
		if (spriteOn) {
			window.draw(sprite);
		}
		for (auto& pixel : strokes) {
			if (pixel.getFillColor() != sf::Color::Transparent) {
				window.draw(pixel);
			}
		}
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}

void SeparatorText(const char* label)
{
	ImGui::Separator();
	ImGui::Text("%s", label);
	ImGui::Separator();
}

void saveFile(int projectSize[], std::vector<sf::RectangleShape>& strokes, char fileName[]) {
	CreateDirectoryA("save", NULL); // Creates if missing, does nothing if exists

	sf::RenderTexture renderTexture;
	renderTexture.create(projectSize[width], projectSize[height]);

	sf::View view(sf::FloatRect(
		640 - projectSize[width] / 2.f,
		360 - projectSize[height] / 2.f,
		projectSize[width],
		projectSize[height]
	));
	renderTexture.setView(view);

	renderTexture.clear(sf::Color::White);

	for (auto& pixel : strokes) {
		renderTexture.draw(pixel);
	}

	renderTexture.display();

	sf::Image image = renderTexture.getTexture().copyToImage();
	char path[260];
	strcpy(path, "save/");
	strcat(path, fileName);
	strcat(path, ".png");

	if (!image.saveToFile(path)) {
		MessageBoxA(NULL, "Failed to save the image!", "Error", MB_ICONERROR);
	}
	else {
		std::string msg = std::string("Saved file to save/") + fileName + ".png";
		MessageBoxA(NULL, msg.c_str(), "Success", MB_OK | MB_ICONASTERISK);
	}
}

//Bresenham's Line Algorithm
void draw(const sf::Vector2f& startPoint, const sf::Vector2f& endPoint, std::vector<sf::RectangleShape>& pixels,
	const sf::Vector2f& pix_size, const sf::Color& color, std::vector<sf::RectangleShape>& temp) {

	float x0 = startPoint.x;
	float x1 = endPoint.x;
	float y0 = startPoint.y;
	float y1 = endPoint.y;

	float dx = std::abs(x1 - x0);
	float sx = x0 < x1 ? pix_size.x : -pix_size.x;
	float dy = -std::abs(y1 - y0);
	float sy = y0 < y1 ? pix_size.y : -pix_size.y;
	float err = dx + dy;

	while (true) {
		//Plot
		sf::RectangleShape pixel(pix_size);
		pixel.setFillColor(color);
		pixel.setOrigin(pixel.getLocalBounds().width / 2, pixel.getLocalBounds().height / 2);
		pixel.setPosition(x0, y0);
		pixels.push_back(pixel);
		temp.push_back(pixel);

		float e2 = 2 * err;
		if (e2 >= dy) {
			if (x0 == x1) break;
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx) {
			if (y0 == y1) break;
			err += dx;
			y0 += sy;
		}
	}
}


