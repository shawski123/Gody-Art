#include "Drawing.h"

void draw(const sf::Vector2i& startPoint, const sf::Vector2i& endPoint, sf::Image& canvasImage,
	const sf::Color& color) {

	int x0 = startPoint.x;
	int x1 = endPoint.x;
	int y0 = startPoint.y;
	int y1 = endPoint.y;

	int dx = std::abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -std::abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	while (true) {
		//Plot
		if (x0 >= 0 && x0 < canvasImage.getSize().x &&
			y0 >= 0 && y0 < canvasImage.getSize().y) {
			canvasImage.setPixel(x0, y0, color);
		}

		int e2 = 2 * err; 
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

void saveState(std::vector<sf::Image>& undoVec, sf::Image& canvasImage) {
	undoVec.push_back(canvasImage);
}

void undo(std::vector<sf::Image>& undoVec , std::vector<sf::Image>& redoVec, sf::Image& canvasImage, sf::Texture& canvasTexture) {
	if (!undoVec.empty()) {
		redoVec.push_back(canvasImage);

		canvasImage = undoVec.back();
		canvasTexture.update(canvasImage);
		undoVec.pop_back();
	}
}

void redo(std::vector<sf::Image>& undoVec, std::vector<sf::Image>& redoVec, sf::Image& canvasImage, sf::Texture& canvasTexture) {
	if (!redoVec.empty()) {
		undoVec.push_back(canvasImage);

		canvasImage = redoVec.back();
		canvasTexture.update(canvasImage);
		redoVec.pop_back();
	}
}

