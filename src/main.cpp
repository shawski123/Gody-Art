#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <imgui-SFML.h>
#include <imgui.h>

#include <windows.h>
#include <commdlg.h>

#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows")
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#endif


enum {
	width,
	height
};

void SeparatorText(const char* label);
void saveFile(int projectSize[], std::vector<sf::RectangleShape> strokes, char fileName[]);

int main() {
	bool isctrlopressed = true;
	bool spriteOn = true;
	int windowSize[2] = { 600,600 }; // 600,600 by default
	int projectSize[2] = { 0,0 };
	char fileName[256] = "";
	bool showWin = true;
	float zoomSpeed = 0.1f;

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
	window.setFramerateLimit(1000);
	ImGui::SFML::Init(window);
	bool my_tool_active = true;
	float my_color[4] = { .5f,.5f,.5f,.5f };
	bool clear = false;
	sf::Vector2f pix_size = { 10,10 };
	float single_pix_size = 20;
	std::vector<std::vector<sf::RectangleShape>> undo;
	std::vector<sf::RectangleShape> temp;
	std::vector<sf::RectangleShape> strokes;
	sf::RectangleShape bg(sf::Vector2f(0, 0));
	bg.setFillColor(sf::Color::White);

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	bool isctrlzpressed = false;

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					float delta = event.mouseWheelScroll.delta;
					float zoomFactor = 1.f + delta * zoomSpeed;

					if (zoomFactor > 0) {
						sf::View view = window.getView();
						view.setCenter(640, 360);
						view.zoom(1.f / zoomFactor);
						window.setView(view);
					}
				}
			}

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		if (showWin) {
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
				showWin = false;
			}
			ImGui::End();
		}

		ImGui::SetNextWindowSize(
			ImVec2(float(windowSize[width] / 4), float(windowSize[height])/5), ImGuiCond_Always
		);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		if (!showWin) {
			ImGui::Begin("Pixel Editor", &my_tool_active, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Project Settings"))
				{
					if (ImGui::MenuItem("Open..", "Ctrl+O")) {
						spriteOn = true;
						if (GetOpenFileName(&ofn)) {
							std::cout << "Selected file: " << openFile << std::endl;
						}
						else {
							std::cout << "No file selected or dialog canceled." << std::endl;
						}

						texture.loadFromFile(openFile);
					}
					if (ImGui::MenuItem("Save", "Ctrl+S")) {
						saveFile(projectSize, strokes, fileName);
					}
					if (ImGui::MenuItem("Close", "Ctrl+W")) { ImGui::SFML::Shutdown(); return 0; }
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::ColorEdit4("Color", my_color);
			if (ImGui::Button("Clear")) {
				clear = true;
			}

			if (ImGui::Button("Delete Image")) {
				spriteOn = false;
			}

			if (ImGui::InputFloat("px", &single_pix_size)) {
				pix_size = { single_pix_size, single_pix_size };
			}

			ImGui::End();
		}

		sf::Color color(
			static_cast<sf::Uint8>(my_color[0] * 255),
			static_cast<sf::Uint8>(my_color[1] * 255),
			static_cast<sf::Uint8>(my_color[2] * 255),
			static_cast<sf::Uint8>(my_color[3] * 255)
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
		if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

			// Snap to grid in world coords
			mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pix_size.x) * pix_size.x + pix_size.x / 2.f;
			mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pix_size.y) * pix_size.y + pix_size.y / 2.f;
			if (bg.getGlobalBounds().contains(mouseWorldPos)) {
				sf::RectangleShape pixel(pix_size);
				pixel.setOrigin({ pixel.getLocalBounds().width / 2, pixel.getLocalBounds().height / 2 });
				pixel.setFillColor(color);
				pixel.setPosition(mouseWorldPos);
				strokes.push_back(pixel);
				temp.push_back(pixel);
			}
		}
		else if (!temp.empty()) {
			undo.push_back(temp);
			temp.clear();
		}

		//Erase
		if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
			sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

			// Snap to grid in world coords
			mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pix_size.x) * pix_size.x + pix_size.x / 2.f;
			mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pix_size.y) * pix_size.y + pix_size.y / 2.f;

			for (auto& pixel : strokes) {
				if (pixel.getPosition() == mouseWorldPos) {
					pixel.setFillColor(sf::Color::White);
				}
			}
		}

		//Clears screen
		if (clear == true) {
			strokes.clear();
			undo.clear();
			spriteOn = false;
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			ImGui::SFML::Shutdown(); return 0;
		}
		//Save to file CTRL+S
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			saveFile(projectSize, strokes, fileName);
		}
		//Open file CTRL+O
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) {
			spriteOn = true;
			if (GetOpenFileName(&ofn)) {
				std::cout << "Selected file: " << openFile << std::endl;
			}
			else {
				std::cout << "No file selected or dialog canceled." << std::endl;
			}

			texture.loadFromFile(openFile);
			isctrlopressed = true;
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) {
			isctrlopressed = false;
		}

		window.clear();
		window.draw(bg);
		if (spriteOn) {
			window.draw(sprite);
		}
		ImGui::SFML::Render(window);
		for (auto& pixel : strokes) {
			window.draw(pixel);
		}
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

void saveFile(int projectSize[], std::vector<sf::RectangleShape> strokes, char fileName[]) {
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
	image.saveToFile(path);
}