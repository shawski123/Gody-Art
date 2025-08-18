#pragma once
#include <imgui.h>
#include <iostream>
#include<windows.h>
#include <SFML/Graphics.hpp>
#include "Editor.h"

void SeparatorText(const char* label);
void saveFile(int projectSize[], std::vector<sf::RectangleShape>& strokes, const char* fileName, const sf::RectangleShape& bg);