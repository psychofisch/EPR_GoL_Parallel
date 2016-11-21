char getNeighbour(int x, int y, int number)
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
		x += m_size.x;
	else if (x >= m_size.x)
		x = 0;

	if (y < 0)
		y += m_size.y;
	else if (y >= m_size.y)
		y = 0;

	return m_elements[(y*m_size.x) + x];
}

__kernel void cell(
	__global const char *A,
	__global char *C),
	__global int cycles
{
	int tidX = get_global_id(0);
	int tidY = get_global_id(1);

	

	printf("hi from thread (%d,%d) \n", tidX, tidY);
	C[i] = A[i] + B[i];
}
