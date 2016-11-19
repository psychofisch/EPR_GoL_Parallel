#pragma once

#include <omp.h>
#include <vector>

typedef std::vector<char> char_vector;
typedef unsigned int uint;

#include "FileIO.h"
#include "size.hpp"

class board
{
public:
	board(const char* path);
	~board();

	const char_vector* getBoard();
	void cycle_seq(int cycles);
	void cycle_omp(int cyccles, int t);
	size_t getSizeX();
	size_t getSizeY();
	char getNeighbour(int x, int y, int number);
	void SaveBoard(const char* path);
private:
	size m_size;
	char_vector m_elements;
	char_vector tmp_map;

	void i_calc(int x, int y);
};

