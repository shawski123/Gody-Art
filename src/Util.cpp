#include "Util.h"

void SeparatorText(const char* label) {
	ImGui::Separator();
	ImGui::Text("%s", label);
	ImGui::Separator();
}

void saveFile(sf::Image& canvasImage, const char* fileName) {
	// Get the folder containing the executable
	char exePath[260];
	GetModuleFileNameA(NULL, exePath, 260);
	std::string exeFolder(exePath);
	exeFolder = exeFolder.substr(0, exeFolder.find_last_of("\\/"));

	// Ensure the "save" folder exists inside the exe folder
	std::string saveFolder = exeFolder + "\\save";
	CreateDirectoryA(saveFolder.c_str(), NULL);

	// Build full path
	std::string fullPath = saveFolder + "\\" + fileName + ".png";

	// Save to file
	if (!canvasImage.saveToFile(fullPath)) {
		MessageBoxA(NULL, "Failed to save the image!", "Error", MB_ICONERROR);
	}
	else {
		std::string msg = "Saved file to " + fullPath;
		MessageBoxA(NULL, msg.c_str(), "Success", MB_OK | MB_ICONASTERISK);
	}
}