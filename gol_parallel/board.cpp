#include "board.h"

board::board(const char* path)
{
	FileHandler fh;
	fh.LoadBoardFromFile(path, m_elements, m_size);
}


board::~board()
{
}

const char_vector * board::getBoard()
{
	return &m_elements;
}

void board::cycle_seq(int cycles)
{
	for (int i = 0; i < cycles; ++i)
	{
		tmp_map = char_vector(m_elements);

		for (int y = 0; y < m_size.y; ++y)
		{
			int row = y * m_size.x;
			for (int x = 0; x < m_size.x; ++x)
			{
				i_calc(x, y);
			}
		}

		m_elements = tmp_map;
	}
}

void board::cycle_omp(int cycles, int t)
{
	for (int i = 0; i < cycles; ++i)
	{
		tmp_map = char_vector(m_elements);

		omp_set_dynamic(0);
#pragma omp parallel for num_threads(t)
		for (int y = 0; y < m_size.y; ++y)
		{
			int row = y * m_size.x;
			for (int x = 0; x < m_size.x; ++x)
			{
				i_calc(x, y);
			}
		}

		m_elements = tmp_map;
	}
}

size_t board::getSizeX()
{
	return m_size.x;
}

size_t board::getSizeY()
{
	return m_size.y;
}

void board::SaveBoard(const char * path)
{
	FileHandler fh;
	fh.SaveBoard(path, m_elements, m_size);
}

void board::i_calc(int x, int y)
{
	int pos = y * m_size.x + x;
	int alive = 0;
	char neighbour;

	for (int i = 0; i <= 8; ++i)
	{
		if (i == 4)	//you are not your own neighbour
			continue;

		neighbour = getNeighbour(x, y, i);
		if (neighbour == 'x')
			alive++;

		//optimization
		if (i == 6 && alive == 0 && m_elements[pos] == '.')
			break;

		if (alive >= 4)
			break;
		//***
	}

	if (m_elements[pos] == '.' && alive == 3)
		tmp_map[pos] = 'x';
	else if (m_elements[pos] == 'x' && (alive >= 4 || alive <= 1))
		tmp_map[pos] = '.';
}

char board::getNeighbour(int x, int y, int number)
{
	switch (number)
	{
	case 0: x -= 1;
			y -= 1;
		break;
	case 1: x -= 1;
		break;
	case 2: x -= 1;
			y += 1;
		break;
	case 3: y -= 1;
		break;
	case 5: y += 1;
		break;
	case 6: x += 1;
			y -= 1;
		break;
	case 7: x += 1;
		break;
	case 8: x += 1;
			y += 1;
		break;
	}

	if (x < 0)
		x +=  m_size.x;
	else if (x >= m_size.x)
		x = 0;

	if (y < 0)
		y += m_size.y;
	else if (y >= m_size.y)
		y = 0;

	return m_elements[(y*m_size.x) + x];
}
