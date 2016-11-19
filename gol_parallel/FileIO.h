#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <string.h>

#include "board.h"
#include "size.hpp"

typedef unsigned int uint;

class FileHandler
{
public:
	FileHandler();
	~FileHandler();

	void LoadBoardFromFile(const char* path, char_vector& map, size& size);
	void SaveBoard(const char* path, char_vector& map);
	void SaveText(const char*);
	std::string LoadText(const char*);
};

