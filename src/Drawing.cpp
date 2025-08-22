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
	std::cout << "Saved\n";
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

void erase(sf::Image& canvasImage, sf::Texture& canvasTexture, const sf::Sprite& canvasSprite, const sf::RenderWindow& window, sf::RectangleShape& eraser) {
	sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
	sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
	
	eraser.setPosition(mouseWorldPos);

	sf::Vector2f localPos = mouseWorldPos - canvasSprite.getPosition();
	localPos += { canvasSprite.getLocalBounds().width / 2.f,
		canvasSprite.getLocalBounds().height / 2.f };

	sf::Vector2i imagePos(localPos.x, localPos.y);
	
	canvasImage.setPixel(imagePos.x, imagePos.y, sf::Color::White);
	canvasImage.setPixel(imagePos.x+1, imagePos.y, sf::Color::White);
	canvasImage.setPixel(imagePos.x-1, imagePos.y, sf::Color::White);
	canvasImage.setPixel(imagePos.x, imagePos.y+1, sf::Color::White);
	canvasImage.setPixel(imagePos.x, imagePos.y-1, sf::Color::White);
	canvasImage.setPixel(imagePos.x+1, imagePos.y+1, sf::Color::White);
	canvasImage.setPixel(imagePos.x-1, imagePos.y-1, sf::Color::White);
	canvasImage.setPixel(imagePos.x+1, imagePos.y-1, sf::Color::White);
	canvasImage.setPixel(imagePos.x-1, imagePos.y+1, sf::Color::White);
	canvasTexture.update(canvasImage);
}

void floodFill(sf::Image& canvasImage, sf::Color newColor, sf::Vector2i mousePos) {
	if (canvasImage.getPixel(mousePos.x, mousePos.y) == newColor) return;

	std::vector<std::pair<int, int>> directions = {
		{1, 0}, {-1, 0}, {0,1}, {0,-1} };
	
	std::queue<std::pair<int, int>> q;
	sf::Color oldColor = canvasImage.getPixel(mousePos.x, mousePos.y);
	q.push({ mousePos.x, mousePos.y });

	canvasImage.setPixel(mousePos.x, mousePos.y, newColor);

	while (!q.empty()) {
		std::pair<int, int> front = q.front();
		int x = front.first, y = front.second;
		q.pop();

		for (const std::pair<int, int>& direction : directions) {
			int nx = x + direction.first;
			int ny = y + direction.second;

			if (nx >= 0 && nx < canvasImage.getSize().x &&
				ny >= 0 && ny < canvasImage.getSize().y &&
				canvasImage.getPixel(nx, ny) == oldColor) {
				
				canvasImage.setPixel(nx, ny, newColor);
				q.push({ nx, ny });
			}
		}
	}
}
