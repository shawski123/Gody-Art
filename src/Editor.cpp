#include "Editor.h"

Editor::Editor()
	: window(sf::VideoMode(windowSize[width], windowSize[height]), "Gody Art") {
	window.setFramerateLimit(144);
	ImGui::SFML::Init(window);
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // No specific owner window
	ofn.lpstrFilter = "All Images\0*.png;*.jpg;*.jpeg\0";
	ofn.lpstrFile = openFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	eraser.setSize({ 3,2 });
	eraser.setOutlineColor(sf::Color::Black);
	eraser.setFillColor(sf::Color::Transparent);
	eraser.setOutlineThickness(0.5f);
	eraser.setOrigin(eraser.getLocalBounds().width / 2, eraser.getLocalBounds().height / 2);
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
		SeparatorText("Project Size (Custom)");
		ImGui::InputInt2("", projectSize);
		SeparatorText("Size Presets (Low Res)");
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
		SeparatorText("Size Presets (Higher Res)");
		if (ImGui::Button("1280x720")) {
			projectSize[width] = 1280;
			projectSize[height] = 720;
		}
		if (ImGui::Button("1536x864")) {
			projectSize[width] = 1536;
			projectSize[height] = 864;
		}
		if (ImGui::Button("1920x1080")) {
			projectSize[width] = 1920;
			projectSize[height] = 1080;
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

			window.setPosition(sf::Vector2i(
				(desktop.width - window.getSize().x) / 2,
				(desktop.height - window.getSize().y) / 2
			));
			//Set project size
			canvasImage.create(projectSize[width], projectSize[height], sf::Color::White);
			canvasTexture.loadFromImage(canvasImage);
			canvasSprite.setTexture(canvasTexture);
			canvasSprite.setOrigin(canvasSprite.getLocalBounds().width / 2, canvasSprite.getLocalBounds().height / 2);
			canvasSprite.setPosition(canvasMovement);
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

			ImGui::SliderFloat("Transparency", &imageTransparency, 0, 255);

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
		ImGui::Text("LControl + Z = Undo");
		ImGui::Text("LControl + Shift + Z = Redo");
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
	if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && inFocus && !isFPressed && showWin) {
		sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
		sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

		sf::Vector2f localPos = mouseWorldPos - canvasSprite.getPosition();
		localPos += { canvasSprite.getLocalBounds().width / 2.f,
			canvasSprite.getLocalBounds().height / 2.f };

		sf::Vector2i imagePos(localPos.x, localPos.y);

		if (hasClicked) {
			saveState(undoVec, canvasImage);
			firstPos = imagePos;
			hasClicked = false;
		}
		else {
			if (canvasSprite.getGlobalBounds().contains(mouseWorldPos)) {
				draw(firstPos, imagePos, canvasImage, color);
				canvasTexture.update(canvasImage);
			}

			firstPos = imagePos;
		}
	}
	else {
		hasClicked = true;
	}

	//Erase
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && showWin) {
		if (saveErase) {
			saveState(undoVec, canvasImage);
			saveErase = false;
		}
		isEraserOn = true;
		erase(canvasImage, canvasTexture, canvasSprite, window, eraser);
	}
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		isEraserOn = false;
		saveErase = true;
	}

	//Fill
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F) && showWin) {
		isFPressed = true;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isFPressed && 
		canvasSprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {

		isFPressed = false;
	}

	//Clears screen
	if (clear == true) {
		clearScreen();
	}

	//Undo CTRL+Z
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) 
		&& !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !isCtrlZPressed) {
		undo(undoVec, redoVec, canvasImage, canvasTexture);
		isCtrlZPressed = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		isCtrlZPressed = false;
	}

	//Redo CTRL+Shift+Z
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) && !isCtrlSZPressed) {
		redo(undoVec, redoVec, canvasImage, canvasTexture);
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
		saveFile(canvasImage, fileName);
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
	window.draw(canvasSprite);
	if (spriteOn) {
		sf::Color color = imageSprite.getColor();
		color.a = imageTransparency;
		imageSprite.setColor(color);
		window.draw(imageSprite);
	}
	if (isEraserOn) {
		window.draw(eraser);
	}
	ImGui::SFML::Render(window);
	window.display();
}

void Editor::loadImage(const char* filename) {
	if (imageTexture.loadFromFile(filename)) {
		std::cout << "Loaded from " << filename << std::endl;
		imageSprite.setTexture(imageTexture);
		sf::Vector2f spriteSize(imageTexture.getSize().x, imageTexture.getSize().y);
		imageSprite.setPosition(canvasMovement);

		float scaleFactor = 1.0f;
		if (spriteSize.x > projectSize[width] || spriteSize.y > projectSize[height]) {
			float scaleX = projectSize[width] / spriteSize.x;
			float scaleY = projectSize[height] / spriteSize.y;
			scaleFactor = std::min(scaleX, scaleY);
		}

		imageSprite.setScale(scaleFactor, scaleFactor);
		hasImage = true;
		spriteOn = true;
	}
}

void Editor::clearScreen() {
	std::cout << "Sprite Scale X: " << imageSprite.getScale().x << " Y: " << imageSprite.getScale().y << std::endl;
	hasImage = false;
	spriteOn = false;
	hideOrshow = "Hide image";
	countHideOrShow = false;
	clear = false;
	saveState(undoVec, canvasImage);
	canvasImage.create(projectSize[width], projectSize[height], sf::Color::White);
	canvasTexture.update(canvasImage);
}