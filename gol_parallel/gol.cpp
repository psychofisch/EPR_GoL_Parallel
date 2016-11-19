// gol.cpp : Defines the entry point for the console application.
//

#include "board.h"
#include "stopwatch.h"

int main(int argc, char* argv[])
{
	stopwatch cl;

	std::string path_in = "", path_out = "";
	int cycles = 1;
	bool measure = false;
	bool debug = false;
	
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--load") == 0)
		{
			path_in = argv[++i];
			if(debug)
				std::cout << "input file: " << path_in << std::endl;
		}
		else if (strcmp(argv[i], "--generations") == 0)
		{
			cycles = atoi(argv[++i]);
			if (debug)
				std::cout << "generations: " << cycles << std::endl;
		}
		else if (strcmp(argv[i], "--save") == 0)
		{
			path_out = argv[++i];
			if (debug)
				std::cout << "output file: " << path_out << std::endl;
		}
		else if (strcmp(argv[i], "--measure") == 0)
		{
			measure = true;
			if (debug)
				std::cout << "measuring " << ((measure) ? "ENABLED" : "DISABLED") << std::endl;
		}
		else if(strcmp(argv[i], "--debug") == 0)
		{
			debug = true;
			std::cout << "DEBUG MODE ACTIVATED"  << std::endl;
		}
	}

	if (path_in == "" || path_out == "")
	{
		std::cout << "no " << ((path_in == "") ? "in" : "out") << "put file specified -> exiting...\n";
		exit(1);
	}

	//LOADING
	int stamps = 0;
	cl.start();

	board* main_board = new board(path_in.c_str());

	stamps = cl.stop();

	if (debug)
		std::cout << "time 2 load: " << cl.getDuration(stamps) << std::endl;

	//CYCLING
	cl.start();

	for (int i = 0; i < cycles; ++i)
	{
		main_board->cycle();
	}

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
		std::cout << cl.getDurationString(0) << "; " << cl.getDurationString(1) << "; " << cl.getDurationString(2) << ";\n";
	}
	std::cin.ignore();
	return 0;
}

