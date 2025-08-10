#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <imgui-SFML.h>
#include <imgui.h>

enum {
    width,
    height
};

int main() {
    int windowSize[2] = { 600,600};
    char fileName [256] = "drawing";
    bool showWin = true;

    sf::RenderWindow window(sf::VideoMode(windowSize[width], windowSize[height]), "Gody Art");
    window.setFramerateLimit(144);
    ImGui::SFML::Init(window);
    bool my_tool_active = true;
    float my_color[4] = { .5f,.5f,.5f,.5f };
    bool clear = false;
    sf::Vector2f pix_size = { 10,10 };
    float single_pix_size = 5;
    std::vector<std::vector<sf::RectangleShape>> undo;
    std::vector<sf::RectangleShape> temp;
    std::vector<sf::RectangleShape> strokes;
    sf::RectangleShape bg(sf::Vector2f(windowSize[width], windowSize[height]));
    bg.setOrigin(bg.getLocalBounds().width / 2, bg.getLocalBounds().height / 2);
    bg.setFillColor(sf::Color::White);
    bg.setPosition(windowSize[width] / 2, windowSize[height] / 2);

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    std::vector<sf::RectangleShape> eraseVec;

    bool isctrlzpressed = false;
    bool isctrlwpressed = false;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SetNextWindowSize(ImVec2(600,600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0,0));
        if (showWin) {
			ImGui::Begin("Project Settings");
			ImGui::InputInt2("Window Size", windowSize);
			if (ImGui::InputText("Save Location", fileName, 256)) {

			}
			if (ImGui::Button("Save and Close")) {
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
                showWin = false;
			}
            ImGui::End();
        }

        ImGui::SetNextWindowSize(
            ImVec2(float(windowSize[width] / 4), float(windowSize[height])), ImGuiCond_Always
        );
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (!showWin) {
			ImGui::Begin("Pixel Editor", &my_tool_active, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Project"))
				{
					if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
					if (ImGui::MenuItem("Save", "Ctrl+S")) {
						sf::RenderTexture renderTexture;
						renderTexture.create(windowSize[width], windowSize[height]);
						renderTexture.clear(sf::Color::White);

						for (auto& pixel : strokes) {
							renderTexture.draw(pixel);
						}
						renderTexture.display();

						sf::Image image = renderTexture.getTexture().copyToImage();
						char fullFileName[260];
						strcpy(fullFileName, fileName);
						strcat(fullFileName, ".png");
						image.saveToFile(fullFileName);
						ImGui::SFML::Shutdown(); return 0;
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

			if (ImGui::InputFloat("Thickness", &single_pix_size)) {
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

        if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::RectangleShape pixel(pix_size);
            pixel.setOrigin({ pixel.getLocalBounds().width / 2, pixel.getLocalBounds().height / 2 });
            pixel.setFillColor(color);
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // Snap to grid
			mousePos.x = ((mousePos.x + (int)pix_size.x / 2) / (int)pix_size.x) * (int)pix_size.x;
			mousePos.y = ((mousePos.y + (int)pix_size.y / 2) / (int)pix_size.y) * (int)pix_size.y;

            pixel.setPosition(sf::Vector2f(mousePos));
            strokes.push_back(pixel);
            temp.push_back(pixel);
        }
        else if (!temp.empty()) {
            undo.push_back(temp);
            temp.clear();
        }

        //Erase
		if (!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::RectangleShape erase(pix_size);
			erase.setFillColor(sf::Color::White);
            erase.setOrigin({ erase.getLocalBounds().width / 2, erase.getLocalBounds().height / 2 });

			sf::Vector2i mousePos = sf::Mouse::getPosition(window);

			// Snap to grid
			mousePos.x = ((mousePos.x + (int)pix_size.x / 2) / (int)pix_size.x) * (int)pix_size.x;
			mousePos.y = ((mousePos.y + (int)pix_size.y / 2) / (int)pix_size.y) * (int)pix_size.y;


            erase.setPosition(sf::Vector2f(mousePos));
            eraseVec.push_back(erase);
		}

        if (clear == true) {
            strokes.clear();
            undo.clear();
            eraseVec.clear();
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
            sf::RenderTexture renderTexture;
            renderTexture.create(windowSize[width], windowSize[height]);
            renderTexture.clear(sf::Color::White);
            
            for (auto& pixel : strokes) {
                renderTexture.draw(pixel);
            }
            renderTexture.display();

            sf::Image image = renderTexture.getTexture().copyToImage();
			char fullFileName[260];
			strcpy(fullFileName, fileName);
			strcat(fullFileName, ".png");
			image.saveToFile(fullFileName);
            ImGui::SFML::Shutdown(); return 0;
        }

        window.clear();
        window.draw(bg);
        ImGui::SFML::Render(window);
        for (auto& pixel : strokes) {
            window.draw(pixel);
        }
        for (auto& pixel : eraseVec) {
            window.draw(pixel);
        }
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

