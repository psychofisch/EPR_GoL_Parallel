char getNeighbour(int x, int y, int size_x, int size_y, int number, __global char* elements)
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
		x += size_x;
	else if (x >= size_x)
		x = 0;

	if (y < 0)
		y += size_y;
	else if (y >= size_y)
		y = 0;

	return elements[(y*size_x) + x];
}

__kernel void cell(
	__global char *elements,
	__global char *tmp,
	const int size_x,
	const int size_y)
{
	int tidX = get_global_id(0);
	int tidY = get_global_id(1);

	if (tidX < size_x && tidY < size_y)
	{
		int pos = (tidY * size_x) + tidX;
		int alive = 0;
		char neighbour;

		for (int i = 0; i <= 8; ++i)
		{
			if (i == 4)	//you are not your own neighbour
				continue;

			neighbour = getNeighbour(tidX, tidY, size_x, size_y, i, elements);
			if (neighbour == 'x')
				alive++;

			//optimization
			//if (i == 6 && alive == 0 && elements[pos] == '.')
			//	break;

			/*if (alive >= 4)
				break;*/
			//***
		}

		if (elements[pos] == '.' && alive == 3)
			tmp[pos] = 'x';
		else if (elements[pos] == 'x' && (alive >= 4 || alive <= 1))
			tmp[pos] = '.';
		else
			tmp[pos] = elements[pos];
	}
}
