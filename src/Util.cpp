#include "Util.h"

void SeparatorText(const char* label) {
	ImGui::Separator();
	ImGui::Text("%s", label);
	ImGui::Separator();
}

void saveFile(int projectSize[], std::vector<sf::RectangleShape>& strokes, const char* fileName) {
	// Get the folder containing the executable
	char exePath[260];
	GetModuleFileNameA(NULL, exePath, 260);
	std::string exeFolder(exePath);
	exeFolder = exeFolder.substr(0, exeFolder.find_last_of("\\/"));

	// Ensure the "save" folder exists inside the exe folder
	std::string saveFolder = exeFolder + "\\save";
	CreateDirectoryA(saveFolder.c_str(), NULL);

	// Setup render texture
	sf::RenderTexture renderTexture;
	renderTexture.create(projectSize[width], projectSize[height]);

	sf::View view(sf::FloatRect(
		640 - projectSize[width] / 2.f,
		360 - projectSize[height] / 2.f,
		static_cast<float>(projectSize[width]),
		static_cast<float>(projectSize[height])
	));
	renderTexture.setView(view);

	renderTexture.clear(sf::Color::White);

	for (auto& pixel : strokes) {
		renderTexture.draw(pixel);
	}

	renderTexture.display();

	sf::Image image = renderTexture.getTexture().copyToImage();

	// Build full path
	std::string fullPath = saveFolder + "\\" + fileName + ".png";

	// Save to file
	if (!image.saveToFile(fullPath)) {
		MessageBoxA(NULL, "Failed to save the image!", "Error", MB_ICONERROR);
	}
	else {
		std::string msg = "Saved file to " + fullPath;
		MessageBoxA(NULL, msg.c_str(), "Success", MB_OK | MB_ICONASTERISK);
	}
}