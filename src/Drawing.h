#include <iostream>
#include <SFML/Graphics.hpp>

void draw(const sf::Vector2i& startPoint, const sf::Vector2i& endPoint, sf::Image& canvasImage, const sf::Color& color);
void saveState(std::vector<sf::Image>& undoVec, sf::Image& canvasImage);
void undo(std::vector<sf::Image>& undoVec, std::vector<sf::Image>& redoVec, sf::Image& canvasImage, sf::Texture& canvasTexture);
void redo(std::vector<sf::Image>& undoVec, std::vector<sf::Image>& redoVec, sf::Image& canvasImage, sf::Texture& canvasTexture);

//void fill();
