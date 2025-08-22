#pragma once
#include <imgui.h>
#include <iostream>
#include<windows.h>
#include <SFML/Graphics.hpp>
#include "Editor.h"

void SeparatorText(const char* label);
void saveFile(sf::Image& canvasImage, const char* fileName);