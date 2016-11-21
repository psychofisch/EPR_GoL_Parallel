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
			for (int x = 0; x < m_size.x; ++x)
			{
				i_calc(x, y);
			}
		}

		m_elements = tmp_map;
	}
}

void board::cycle_ocl(int cycles, int platformId, int deviceId)
{
	const std::string KERNEL_FILE = "cell.cl";
	cl_int err = CL_SUCCESS;
	cl::Program program;
	std::vector<cl::Device> devices;

	platformId = ((platformId < 0) ? 0 : platformId);
	deviceId = ((deviceId < 0) ? 0 : deviceId);

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) {
		std::cout << "No OpenCL platforms available!\n";
		return;
	}

	// create a context and get available devices
	cl::Platform platform = platforms[platformId]; // on a different machine, you may have to select a different platform
	cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0 };
	cl::Context context(CL_DEVICE_TYPE_GPU, properties);

	devices = context.getInfo<CL_CONTEXT_DEVICES>();

	// load and build the kernel
	std::ifstream sourceFile(KERNEL_FILE);
	if (!sourceFile)
	{
		std::cout << "kernel source file " << KERNEL_FILE << " not found!" << std::endl;
		return;
	}
	std::string sourceCode(
		std::istreambuf_iterator<char>(sourceFile),
		(std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	program = cl::Program(context, source);
	program.build(devices);
	//create kernels
	cl::Kernel kernel(program, "cell", &err);

	cl::CommandQueue queue(context, devices[0], 0, &err);
	// create input and output data
	
	std::vector<int> c;
	c.resize(a.size());
	// input buffers
	cl::Buffer bufferA = cl::Buffer(context, CL_MEM_READ_ONLY, a.size() * sizeof(int));
	cl::Buffer bufferB = cl::Buffer(context, CL_MEM_READ_ONLY, b.size() * sizeof(int));
	// output buffers
	cl::Buffer bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY, c.size() * sizeof(int));

	// fill buffers
	queue.enqueueWriteBuffer(
		bufferA, // which buffer to write to
		CL_TRUE, // block until command is complete
		0, // offset
		a.size() * sizeof(int), // size of write 
		&a[0]); // pointer to input
	queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, b.size() * sizeof(int), &b[0]);

	cl::Kernel Kernel(program, "cell", &err);
	Kernel.setArg(0, bufferA);
	Kernel.setArg(1, bufferB);
	Kernel.setArg(2, bufferC);

	// launch add kernel
	// Run the kernel on specific ND range
	cl::NDRange global(a.size());
	cl::NDRange local(1); //make sure local range is divisible by global range
	cl::NDRange offset(0);
	std::cout << "call 'vector_add' kernel" << std::endl;
	queue.enqueueNDRangeKernel(Kernel, offset, global, local);

	// read back result
	queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, c.size() * sizeof(int), &c[0]);

	for (int i : c)
	{
		std::cout << i << std::endl;
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
