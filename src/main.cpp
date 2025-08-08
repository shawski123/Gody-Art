#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Gody Art");
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
    sf::RectangleShape bg({ 1280,720 });
    bg.setOrigin(bg.getLocalBounds().width / 2, bg.getLocalBounds().height / 2);
    bg.setFillColor(sf::Color::White);
    bg.setPosition({ 640, 360 });

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

        ImGui::Begin("Pixel Editor", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Project"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {  }
                if (ImGui::MenuItem("Close", "Ctrl+W")) { ImGui::SFML::Shutdown(); return 0; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::ColorEdit4("Color", my_color);
        if (ImGui::Button("Clear")) {
            clear = true;
        }

        ImGui::InputFloat("", &single_pix_size);

        if (ImGui::Button("Set thickness")) {
            pix_size = { single_pix_size, single_pix_size };
        }

        if (ImGui::Button("Pencil")) {
            pix_size.x = 10;
            pix_size.y = 5;
        }

        ImGui::End();
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
            pixel.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
            strokes.push_back(pixel);
            temp.push_back(pixel);
        }
        else if (!temp.empty()) {
            undo.push_back(temp);
            temp.clear();
        }

        if (clear == true) {
            strokes.clear();
            undo.clear();
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !isctrlwpressed) {
            ImGui::SFML::Shutdown(); return 0;
        }

        window.clear();
        window.draw(bg);
        ImGui::SFML::Render(window);
        for (auto& pixel : strokes) {
            window.draw(pixel);
        }
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

