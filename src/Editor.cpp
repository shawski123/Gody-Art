#include "Editor.h"

Editor::Editor()
	: window(sf::VideoMode(windowSize[width], windowSize[height]), "Gody Art") {
	window.setFramerateLimit(144);
	ImGui::SFML::Init(window);
	undoVec.reserve(10000);
	redoVec.reserve(10000);
	strokes.reserve(10000);
	bg.setFillColor(sf::Color::White);
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // No specific owner window
	ofn.lpstrFilter = "PNG Images\0*.png\0JPEG Images\0*.jpg\0";
	ofn.lpstrFile = openFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void Editor::run() {
	while (window.isOpen()) {
		processEvents();

		sf::Time dt = deltaClock.restart();           
		ImGui::SFML::Update(window, dt);             

		update(dt.asSeconds());
		render();
	}
}

void Editor::processEvents() {
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
}

void Editor::update(float deltaTime) {

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
		ImGui::Begin("Pixel Editor", &myToolActive, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Project Settings"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					if (GetOpenFileName(&ofn)) {
						loadImage(openFile);
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
						ImGui::SFML::Shutdown(); return;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::ColorEdit3("Color", myColor);
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

		if (ImGui::InputFloat("px", &singlePixSize)) {
			pixSize = { singlePixSize, singlePixSize };
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
		static_cast<sf::Uint8>(myColor[0] * 255),
		static_cast<sf::Uint8>(myColor[1] * 255),
		static_cast<sf::Uint8>(myColor[2] * 255)
	);

	//Opened image

	//Draw
	if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Left) && inFocus) {
		sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

		// Snap to grid in world coords
		mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pixSize.x) * pixSize.x + pixSize.x / 2.f;
		mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pixSize.y) * pixSize.y + pixSize.y / 2.f;

		if (hasClicked) {
			firstPos = mouseWorldPos;
			hasClicked = false;
		}
		else {
			if (bg.getGlobalBounds().contains(mouseWorldPos)) {
				draw(firstPos, mouseWorldPos, strokes, pixSize, color, temp);
			}

			firstPos = mouseWorldPos;
		}
	}
	else if (!temp.empty()) {
		undoVec.push_back(temp);
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
		mouseWorldPos.x = static_cast<int>(mouseWorldPos.x / pixSize.x) * pixSize.x + pixSize.x / 2.f;
		mouseWorldPos.y = static_cast<int>(mouseWorldPos.y / pixSize.y) * pixSize.y + pixSize.y / 2.f;

		for (auto& pixel : strokes) {
			if (pixel.getPosition() == mouseWorldPos) {
				pixel.setFillColor(sf::Color::Transparent);
			}
		}
	}*/

	//Clears screen
	if (clear == true) {
		redoVec.clear();
		//clearVec.push_back(strokes);
		undoVec.clear();
		strokes.clear();
		hasImage = false;
		spriteOn = false;
		hideOrshow = "Hide image";
		countHideOrShow = false;
		clear = false;
	}

	//Undo CTRL+Z
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) 
		&& !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !isCtrlZPressed) {
		undo(temp, undoVec, strokes, redoVec, clearVec);
		isCtrlZPressed = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		isCtrlZPressed = false;
	}

	//Redo CTRL+Shift+Z
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) && !isCtrlSZPressed) {
		redo(redoVec, strokes, undoVec);
		isCtrlSZPressed = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
		isCtrlSZPressed = false;
	}

	//Close CTRL+W
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && showWin) {
		if (MessageBoxA(NULL, "Are you sure?", "Exit", MB_YESNO | MB_ICONEXCLAMATION) == 6) {
			ImGui::SFML::Shutdown(); return;
		}
	}

	//Save to file CTRL+S
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && showWin) {
		saveFile(projectSize, strokes, fileName);
	}
	//Open file CTRL+O
	if (!isCtrlOPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O) && showWin) {
		spriteOn = true;
		if (GetOpenFileName(&ofn)) {
			loadImage(openFile);
		}
		else {
			std::cout << "No file selected or dialog canceled." << std::endl;
		}
		isCtrlOPressed = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O)) {
		isCtrlOPressed = false;
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
}

void Editor::render() {
	window.clear();
	window.draw(bg);
	if (spriteOn) {
		window.draw(sprite);
	}
	for (auto& pixel : strokes) window.draw(pixel);
	ImGui::SFML::Render(window);
	window.display();
}

void Editor::loadImage(const char* filename) {
	if (texture.loadFromFile(filename)) {
		std::cout << "Loaded from " << filename << std::endl;
		sprite.setTexture(texture);
		sf::Vector2f spriteSize(texture.getSize().x, texture.getSize().y);
		sprite.setOrigin({ spriteSize.x / 2, spriteSize.y / 2 });
		sprite.setPosition(bg.getPosition());

		float scaleFactor = 1.0f;
		if (spriteSize.x > projectSize[width] || spriteSize.y > projectSize[height]) {
			float scaleX = projectSize[width] / spriteSize.x;
			float scaleY = projectSize[height] / spriteSize.y;
			scaleFactor = std::min(scaleX, scaleY);
		}

		sprite.scale(scaleFactor, scaleFactor);
		hasImage = true;
		spriteOn = true;
	}
}
