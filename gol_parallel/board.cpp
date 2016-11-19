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

void board::cycle()
{
	tmp_map = char_vector(m_elements);

	/*
	this for-loop constellation is faster than for(y){for(x)}; i'm not sure why...
	*/
	for (int y = 0; y < m_size.y; ++y)
	{
		int row = y * m_size.x;
		for (int x = 0; x < m_size.x; ++x)
		{
			uint alive = 0;
			char neighbour;
			int c = 0;

			for (int i = 0; i <= 8; ++i)
			{
				if (i == 4)	//you are not your own neighbour
					continue;

				neighbour = getNeighbour(x, y, i);
				if (neighbour == 'x')
					alive++;

				//optimization
				if (i == 6 && alive == 0 && m_elements[row+x] == '.')
					break;

				if (alive >= 4)
					break;
				//***
			}

			//std::cout << alive;

			if (m_elements[row + x] == '.' && alive == 3)
				tmp_map[row + x] = 'x';
			else if(m_elements[row + x] == 'x' && (alive >= 4 || alive <= 1 ))
				tmp_map[row + x] = '.';
		}
		//std::cout << std::endl;
	}

	m_elements = tmp_map;
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
