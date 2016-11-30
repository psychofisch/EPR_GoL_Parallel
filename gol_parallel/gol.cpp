// gol.cpp : Defines the entry point for the console application.
//

#define DEBUG(x) std::cout << #x << ": " << x << std::endl;

#include <limits>

#include "board.h"
#include "stopwatch.h"

enum Mode { MODE_SEQ, MODE_OMP, MODE_OCL };

int main(int argc, char* argv[])
{
	stopwatch cl;

	std::string path_in = "", path_out = "";
	int cycles = 1,
		threads = 4,
		platformId = -1,
		deviceId = -1;
	bool measure = false;
	bool debug = false;
	bool fulltime = false;
	Mode mode = MODE_SEQ;
	board::OCLMODE ocl_mode = board::OCL_ALL;
	
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--load") == 0)
		{
			path_in = argv[++i];
			if(debug)
				DEBUG(path_in);
		}
		else if (strcmp(argv[i], "--generations") == 0)
		{
			cycles = atoi(argv[++i]);
			if (debug)
				DEBUG(cycles);
		}
		else if (strcmp(argv[i], "--save") == 0)
		{
			path_out = argv[++i];
			if (debug)
				DEBUG(path_out);
		}
		else if (strcmp(argv[i], "--measure") == 0)
		{
			measure = true;
			if (debug)
				std::cout << "measuring " << ((measure) ? "ENABLED" : "DISABLED") << std::endl;
		}
		else if (strcmp(argv[i], "--fulltime") == 0)
		{
			fulltime = true;
			if (debug)
				std::cout << "measuring " << ((fulltime) ? "ENABLED" : "DISABLED") << std::endl;
		}
		else if(strcmp(argv[i], "--mode") == 0)
		{
			++i;
			if (strcmp(argv[i], "omp") == 0)
				mode = MODE_OMP;
			else if (strcmp(argv[i], "ocl") == 0)
				mode = MODE_OCL;
		}
		else if (strcmp(argv[i], "--threads") == 0)
		{
			threads = atoi(argv[++i]);
			if (debug)
				DEBUG(threads);
		}
		else if (strcmp(argv[i], "--device") == 0)
		{
			++i;
			if (strcmp(argv[i], "cpu") == 0)
				ocl_mode = board::OCL_CPU;
			else if (strcmp(argv[i], "gpu") == 0)
				ocl_mode = board::OCL_GPU;
		}
		else if (strcmp(argv[i], "--platformId") == 0)
		{
			platformId = atoi(argv[++i]);
			if (debug)
				DEBUG(platformId);
		}
		else if (strcmp(argv[i], "--deviceId") == 0)
		{
			deviceId = atoi(argv[++i]);
			if (debug)
				DEBUG(deviceId);
		}
		else if (strcmp(argv[i], "--debug") == 0)
		{
			debug = true;
			std::cout << "DEBUG MODE ACTIVATED" << std::endl;
		}
	}

	if (path_in == "" || path_out == "")
	{
		std::cout << "no " << ((path_in == "") ? "in" : "out") << "put file specified -> exiting...\n";
		exit(1);
	}

	if (debug)
	{
		if (mode == MODE_SEQ)
			std::cout << "SEQUENTIAL";
		else if (mode == MODE_OMP)
			std::cout << "OpenMP";
		else if (mode == MODE_OCL)
			std::cout << "OpenCL";
		std::cout << " mode selected" << std::endl;
	}

	//LOADING
	int stamps = 0;
	cl.start();

	board* main_board = new board(path_in.c_str());
	if (debug)
		main_board->setDebug(debug);

	stamps = cl.stop();

	if (debug)
		std::cout << "time 2 load: " << cl.getDuration(stamps) << std::endl;

	//CYCLING
	cl.start();

	if (mode == MODE_SEQ)
		main_board->cycle_seq(cycles);
	else if (mode == MODE_OMP)
		main_board->cycle_omp(cycles, threads);
	else if (mode == MODE_OCL && ocl_mode == board::OCL_ALL)
		main_board->cycle_ocl(cycles, ocl_mode, platformId, deviceId);
	else if (mode == MODE_OCL && ocl_mode != board::OCL_ALL)
		main_board->cycle_ocl(cycles, ocl_mode, 0, 0);

	stamps = cl.stop();

	if (debug)
		std::cout << "time 2 compute: " << cl.getDuration(stamps) << std::endl;

	//SAVING
	cl.start();
	main_board->SaveBoard(path_out.c_str());
	delete main_board;
	
	stamps = cl.stop();

	if (debug)
	{
		std::cout << "time 2 save and cleanup: " << cl.getDuration(stamps) << std::endl;

		std::cout << "-> " << cl.getFullDurationString() << std::endl;

		std::cout << "finished...";
	}
	else
	{
		std::cout.precision(3);
		std::cout.fill('0');
		if(fulltime)
			std::cout << std::fixed << cl.getFullDuration() << ";\n";
		else
			std::cout << cl.getDurationString(0) << "; " << cl.getDurationString(1) << "; " << cl.getDurationString(2) << ";\n";
	}
	if(debug)
		std::cin.ignore();
	return 0;
}

