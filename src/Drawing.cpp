#include "Drawing.h"

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

void undo(std::vector<sf::RectangleShape>& temp, std::vector<std::vector<sf::RectangleShape>>& undoVec
	, std::vector<sf::RectangleShape>& strokes, std::vector<std::vector<sf::RectangleShape>>& redoVec, std::vector<std::vector<sf::RectangleShape>>& clearVec) {
	if (!temp.empty()) {
		undoVec.push_back(temp);
		temp.clear();

		redoVec.clear();
	}

	if (!undoVec.empty()) {
		const std::vector<sf::RectangleShape>& lastStroke = undoVec.back();
		if (strokes.size() >= lastStroke.size()) {
			strokes.resize(strokes.size() - lastStroke.size());
			redoVec.push_back(lastStroke);
			undoVec.pop_back();
		}
	}

	/*if (!clearVec.empty()) {
		strokes = clearVec.back();
		undoVec.push_back(strokes);
		clearVec.pop_back();
	}*/
}

void redo(std::vector<std::vector<sf::RectangleShape>>& redoVec, std::vector<sf::RectangleShape>& strokes,
	std::vector<std::vector<sf::RectangleShape>>& undoVec) {
	if (!redoVec.empty()) {
		const std::vector<sf::RectangleShape>& lastStroke = redoVec.back();
		strokes.insert(strokes.end(), lastStroke.begin(), lastStroke.end());
		undoVec.push_back(lastStroke);
		redoVec.pop_back();
	}
}

