#include <iostream>
#include <SFML/Graphics.hpp>

void draw(const sf::Vector2f& startPoint, const sf::Vector2f& endPoint, std::vector<sf::RectangleShape>& pixels,
	const sf::Vector2f& pix_size, const sf::Color& color, std::vector<sf::RectangleShape>& temp);

void undo(std::vector<sf::RectangleShape>& temp, std::vector<std::vector<sf::RectangleShape>>& undoVec
	, std::vector<sf::RectangleShape>& strokes, std::vector<std::vector<sf::RectangleShape>>& redoVec, std::vector<std::vector<sf::RectangleShape>>& clearVec);

void redo(std::vector<std::vector<sf::RectangleShape>>& redoVec, std::vector<sf::RectangleShape>& strokes, 
	std::vector<std::vector<sf::RectangleShape>>& undoVec);

void fill(sf::RectangleShape& bg, sf::Color color);

//void erase(sf::RenderWindow window, sf::Vector2f pixSize, std::vector<sf::RectangleShape> strokes);