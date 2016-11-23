#pragma once
struct size
{
	int x, y;

	size()
		:size(-1, -1)
	{
	}

	size(int ix, int iy)
	{
		x = ix;
		y = iy;
	}
};