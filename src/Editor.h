#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Drawing.h"
#include "Util.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <windows.h>
#include <iostream>


enum {
	width,
	height
};

class Editor {
public:
	Editor();
	void run();
private:
	void processEvents();
	void update(float dt);
	void render();
	void loadImage(const char* fileName);
	
	//State variables
	bool isCtrlZPressed = false;
	bool isCtrlSZPressed = false;
	bool isCtrlOPressed = false;
	bool isFPressed = false;
	bool spriteOn = true;
	char* hideOrshow = "Hide Image";
	int windowSize[2] = { 600,600 }; // 600,600 by default
	int projectSize[2] = { 0,0 };
	char fileName[256] = "";
	bool showWin = false;
	float zoomSpeed = 0.1f;
	float moveSpeed = 400.f;
	bool countHideOrShow = false;
	bool hasImage = false;
	float zoomFactor = 0;
	bool showControls = false;
	char* hideOrshowCtrls = "Show Controls";
	bool countCtrl = false;
	bool inFocus = true;
	bool hasClicked = true;
	bool myToolActive = true;
	bool clear = false;

	//Window
	sf::RenderWindow window;
	sf::Clock deltaClock;

	//Files
	OPENFILENAME ofn;
	char openFile[MAX_PATH] = "";

	//Drawing
	sf::Sprite sprite;
	sf::Vector2f firstPos;
	sf::Texture texture;
	sf::Vector2f pixSize = { 10,10 };
	float singlePixSize = 20;
	float myColor[3] = { 0.f,0.f,0.f };
	std::vector<std::vector<sf::RectangleShape>> undoVec;
	std::vector<sf::RectangleShape> temp;
	std::vector<std::vector<sf::RectangleShape>> redoVec;
	std::vector<sf::RectangleShape> strokes;
	std::vector<std::vector<sf::RectangleShape>> clearVec;
	sf::RectangleShape bg;

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	sf::Vector2f canvasMovement = { 640,360 };
};
