#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <string>

std::string ReadFile(std::string filePath)
{
	std::ifstream fileIn;
	fileIn.open(filePath, std::ios_base::in);
	std::string ret = "";
	std::string buffer;
	while (std::getline(fileIn, buffer)) { ret += (buffer + "\n"); }
	fileIn.close();
	return ret;
}
