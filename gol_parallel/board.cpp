#include "board.h"

board::board(const char* path)
	:m_debug(false)
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
	tmp_map.resize(m_elements.size());
	for (int i = 0; i < cycles; ++i)
	{
		//tmp_map = char_vector(m_elements);

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
	tmp_map.resize(m_elements.size());
	for (int i = 0; i < cycles; ++i)
	{
		//tmp_map = char_vector(m_elements);

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

void board::cycle_ocl(int cycles, OCLMODE mode, int platformId, int deviceId)
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
	cl::Context context;
	
	if (mode == OCL_ALL)
		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);
	else if(mode == OCL_CPU)
		context = cl::Context(CL_DEVICE_TYPE_CPU, properties);
	else if (mode == OCL_GPU)
		context = cl::Context(CL_DEVICE_TYPE_GPU, properties);

	devices = context.getInfo<CL_CONTEXT_DEVICES>();

	char deviceName[255];
	err = devices[deviceId].getInfo(CL_DEVICE_NAME, &deviceName);
	handle_clerror(err);
	if(m_debug)
		std::cout << deviceName << std::endl;

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

	//try {
	err = program.build(devices);
	if (err != ERROR_SUCCESS)
	{
		std::string s;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &s);
		std::cout << s << std::endl;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_OPTIONS, &s);
		std::cout << s << std::endl;
	}
	handle_clerror(err);
	//}
	//catch (cl::Error err) {
	//	// error handling
	//	// if the kernel has failed to compile, print the error log
	//	std::string s;
	//	program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &s);
	//	std::cout << s << std::endl;
	//	program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_OPTIONS, &s);
	//	std::cout << s << std::endl;

	//	std::cerr
	//		<< "ERROR: "
	//		<< err.what()
	//		<< "("
	//		<< err.err()
	//		<< ")"
	//		<< std::endl;
	//}
	//create kernels
	cl::Kernel kernel(program, "cell", &err);
	handle_clerror(err);

	cl::CommandQueue queue(context, devices[deviceId], 0, &err);
	handle_clerror(err);
	// create input and output data
	
	tmp_map.resize(m_elements.size());
	// buffers
	cl::Buffer elements = cl::Buffer(context, CL_MEM_READ_WRITE, m_elements.size() * sizeof(char));
	cl::Buffer tmp = cl::Buffer(context, CL_MEM_READ_WRITE, tmp_map.size() * sizeof(char));

	// fill buffers
	queue.enqueueWriteBuffer(
		elements, // which buffer to write to
		CL_TRUE, // block until command is complete
		0, // offset
		m_elements.size() * sizeof(char), // size of write 
		&m_elements[0]); // pointer to input

	kernel.setArg(0, elements);
	kernel.setArg(1, tmp);
	kernel.setArg(2, m_size.x);
	kernel.setArg(3, m_size.y);

	// launch add kernel
	// Run the kernel on specific ND range
	size globalSize;
	for (int i = 1; i <= 20; ++i)
	{
		if (pow(2, i) > m_size.x)
		{
			globalSize.x = pow(2, i);
			break;
		}
	}

	for (int i = 1; i <= 20; ++i)
	{
		if (pow(2, i) > m_size.y)
		{
			globalSize.y = pow(2, i);
			break;
		}
	}

	if(m_debug)
		std::cout << "threads|real size -> " << globalSize.x << ":" << m_size.x << "|" << globalSize.y << ":" << m_size.y << std::endl;

	cl::NDRange global(globalSize.x, globalSize.y);
	cl::NDRange local(16, 16); //make sure local range is divisible by global range
	cl::NDRange offset(0, 0);

	for (int i = 0; i < cycles; ++i)
	{
		//std::cout << "call 'cell' kernel; cycle " << i << std::endl;
		queue.enqueueNDRangeKernel(kernel, offset, global, local);

		queue.enqueueCopyBuffer(tmp, elements, 0, 0, m_elements.size() * sizeof(char), 0, 0);
	}

	// read back result
	queue.enqueueReadBuffer(elements, CL_TRUE, 0, m_elements.size() * sizeof(char), &m_elements[0]);
}

int board::getSizeX()
{
	return m_size.x;
}

int board::getSizeY()
{
	return m_size.y;
}

void board::SaveBoard(const char * path)
{
	FileHandler fh;
	fh.SaveBoard(path, m_elements, m_size);
}

void board::setDebug(bool d)
{
	m_debug = d;
}

void board::i_calc(int x, int y)
{
	int pos = y * m_size.x + x;
	int alive = 0;
	char neighbour;

	for (int i = 0; i <= 8; ++i)
	{
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
	else
		tmp_map[pos] = m_elements[pos];
}

char board::getNeighbour(int x, int y, int& number)
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
	case 4: return '.';
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

// list of error codes from "CL/cl.h"
std::string board::cl_errorstring(cl_int err) {
	switch (err) {
	case CL_SUCCESS:                          return std::string("Success");
	case CL_DEVICE_NOT_FOUND:                 return std::string("Device not found");
	case CL_DEVICE_NOT_AVAILABLE:             return std::string("Device not available");
	case CL_COMPILER_NOT_AVAILABLE:           return std::string("Compiler not available");
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return std::string("Memory object allocation failure");
	case CL_OUT_OF_RESOURCES:                 return std::string("Out of resources");
	case CL_OUT_OF_HOST_MEMORY:               return std::string("Out of host memory");
	case CL_PROFILING_INFO_NOT_AVAILABLE:     return std::string("Profiling information not available");
	case CL_MEM_COPY_OVERLAP:                 return std::string("Memory copy overlap");
	case CL_IMAGE_FORMAT_MISMATCH:            return std::string("Image format mismatch");
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return std::string("Image format not supported");
	case CL_BUILD_PROGRAM_FAILURE:            return std::string("Program build failure");
	case CL_MAP_FAILURE:                      return std::string("Map failure");
		// case CL_MISALIGNED_SUB_BUFFER_OFFSET:     return std::string("Misaligned sub buffer offset");
		// case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return std::string("Exec status error for events in wait list");
	case CL_INVALID_VALUE:                    return std::string("Invalid value");
	case CL_INVALID_DEVICE_TYPE:              return std::string("Invalid device type");
	case CL_INVALID_PLATFORM:                 return std::string("Invalid platform");
	case CL_INVALID_DEVICE:                   return std::string("Invalid device");
	case CL_INVALID_CONTEXT:                  return std::string("Invalid context");
	case CL_INVALID_QUEUE_PROPERTIES:         return std::string("Invalid queue properties");
	case CL_INVALID_COMMAND_QUEUE:            return std::string("Invalid command queue");
	case CL_INVALID_HOST_PTR:                 return std::string("Invalid host pointer");
	case CL_INVALID_MEM_OBJECT:               return std::string("Invalid memory object");
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return std::string("Invalid image format descriptor");
	case CL_INVALID_IMAGE_SIZE:               return std::string("Invalid image size");
	case CL_INVALID_SAMPLER:                  return std::string("Invalid sampler");
	case CL_INVALID_BINARY:                   return std::string("Invalid binary");
	case CL_INVALID_BUILD_OPTIONS:            return std::string("Invalid build options");
	case CL_INVALID_PROGRAM:                  return std::string("Invalid program");
	case CL_INVALID_PROGRAM_EXECUTABLE:       return std::string("Invalid program executable");
	case CL_INVALID_KERNEL_NAME:              return std::string("Invalid kernel name");
	case CL_INVALID_KERNEL_DEFINITION:        return std::string("Invalid kernel definition");
	case CL_INVALID_KERNEL:                   return std::string("Invalid kernel");
	case CL_INVALID_ARG_INDEX:                return std::string("Invalid argument index");
	case CL_INVALID_ARG_VALUE:                return std::string("Invalid argument value");
	case CL_INVALID_ARG_SIZE:                 return std::string("Invalid argument size");
	case CL_INVALID_KERNEL_ARGS:              return std::string("Invalid kernel arguments");
	case CL_INVALID_WORK_DIMENSION:           return std::string("Invalid work dimension");
	case CL_INVALID_WORK_GROUP_SIZE:          return std::string("Invalid work group size");
	case CL_INVALID_WORK_ITEM_SIZE:           return std::string("Invalid work item size");
	case CL_INVALID_GLOBAL_OFFSET:            return std::string("Invalid global offset");
	case CL_INVALID_EVENT_WAIT_LIST:          return std::string("Invalid event wait list");
	case CL_INVALID_EVENT:                    return std::string("Invalid event");
	case CL_INVALID_OPERATION:                return std::string("Invalid operation");
	case CL_INVALID_GL_OBJECT:                return std::string("Invalid OpenGL object");
	case CL_INVALID_BUFFER_SIZE:              return std::string("Invalid buffer size");
	case CL_INVALID_MIP_LEVEL:                return std::string("Invalid mip-map level");
	case CL_INVALID_GLOBAL_WORK_SIZE:         return std::string("Invalid gloal work size");
		// case CL_INVALID_PROPERTY:                 return std::string("Invalid property");
	default:                                  return std::string("Unknown error code");
	}
}

void board::handle_clerror(cl_int err) {
	if (err != CL_SUCCESS) {
		std::cerr << "OpenCL Error: " << cl_errorstring(err) << std::string(".") << std::endl;
		std::cin.ignore();
		exit(EXIT_FAILURE);
	}
}
