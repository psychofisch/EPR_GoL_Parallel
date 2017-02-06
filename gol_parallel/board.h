#pragma once

//#define __CL_ENABLE_EXCEPTIONS
#include <CL\cl.hpp>
#include <omp.h>
#include <vector>
#include <string>

typedef std::vector<char> char_vector;
typedef unsigned int uint;

#include "FileIO.h"
#include "size.hpp"

const int neighbours[] = { 
	-1,-1,
	-1,0,
	-1,1,
	0,-1,
	0,0,
	0,1,
	1,-1,
	1,0,
	1,1
};

class board
{
public:
	enum OCLMODE {OCL_GPU, OCL_CPU, OCL_ALL};

	board(const char* path);
	~board();

	const char_vector* getBoard();
	void cycle_seq(int cycles);
	void cycle_omp(int cyccles, int t);
	void cycle_ocl(int cycles, OCLMODE mode, int platformId, int deviceId);
	int getSizeX();
	int getSizeY();
	//char getNeighbour(int x, int y, int& number);
	void SaveBoard(const char* path);
	void setDebug(bool d);

	std::string cl_errorstring(cl_int err);
	void handle_clerror(cl_int err);
private:
	bool m_debug;
	size m_size;
	char_vector m_elements;
	char_vector tmp_map;

	void i_calc(int x, int y);
};

