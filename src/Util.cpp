#include "Util.h"

void SeparatorText(const char* label) {
	ImGui::Separator();
	ImGui::Text("%s", label);
	ImGui::Separator();
}

void saveFile(int projectSize[], std::vector<sf::RectangleShape>& strokes, char fileName[]) {
	CreateDirectoryA("save", NULL); 

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

	if (!image.saveToFile(path)) {
		MessageBoxA(NULL, "Failed to save the image!", "Error", MB_ICONERROR);
	}
	else {
		std::string msg = std::string("Saved file to save/") + fileName + ".png";
		MessageBoxA(NULL, msg.c_str(), "Success", MB_OK | MB_ICONASTERISK);
	}
}