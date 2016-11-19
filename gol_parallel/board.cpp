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
	for (int x = 0; x < m_elements[0].size(); ++x)
	{
		for (int y = 0; y < m_elements.size(); ++y)
		{
			uint alive = 0;
			char neighbour;
			char cache[4] = {'?', '?', '?', '?'};
			int c = 0;

			for (int i = 0; i <= 8; ++i)
			{
				if (i == 4)	//you are not your own neighbour
					continue;

				neighbour = getNeighbour(x, y, i);
				if (neighbour == 'x')
					alive++;

				//optimization
				if (i == 6 && alive == 0 && m_elements[y][x] == '.')
					break;

				if (alive >= 4)
					break;
				//***
			}

			//std::cout << alive;

			if (m_elements[y][x] == '.' && alive == 3)
				tmp_map[y][x] = 'x';
			else if(m_elements[y][x] == 'x' && (alive >= 4 || alive <= 1 ))
				tmp_map[y][x] = '.';
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
	fh.SaveBoard(path, m_elements);
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
		x +=  m_elements[0].size();
	else if (x >= m_elements[0].size())
		x = 0;

	if (y < 0)
		y += m_elements.size();
	else if (y >= m_elements.size())
		y = 0;

	return m_elements[y][x];
}
