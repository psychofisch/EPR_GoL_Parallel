#include "FileIO.h"

FileHandler::FileHandler()
{
}

FileHandler::~FileHandler()
{
}

void FileHandler::LoadBoardFromFile(const char* path, char_vector& map, size& size)
{
	std::ifstream file;
	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}

	std::string tmp;
	std::getline(file, tmp);

	int x, y;
	sscanf_s(tmp.c_str(), "%i,%i", &x, &y);

	size.x = x;
	size.y = y;

	map.resize(size.y);

	for (uint i = 0; i < map.size(); ++i)
	{
		map[i].resize(size.x);
	}

	uint county = 0;
	do {
		std::getline(file, tmp);
		
		for (uint x = 0; x < tmp.size(); ++x)
		{
			map[county][x] = tmp[x];
		}
		county++;
	} while (!file.eof());

	file.close();
}

void FileHandler::SaveBoard(const char * path, char_vector & map)
{
	std::ofstream file;
	file.open(path);

	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}

	file << map[0].size() << "," << map.size() << std::endl;

	std::string line = "";
	
	for (uint y = 0; y < map.size(); ++y)
	{
		line = "";
		for (uint x = 0; x < map[y].size(); ++x)
		{
			file << map[y][x];
		}
		file << std::endl;
	}

	file.close();
}

void FileHandler::SaveText(const char * path)
{
	std::ofstream file;
	file.open(path);

	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}

	file << "start\n";
	file << "end\n";

	file.close();
}

std::string FileHandler::LoadText(const char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return "Error";
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	file.close();

	return content;
}
