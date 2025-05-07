#pragma once

#include <string>
#include <fstream>

class ReadFile
{
public:
	ReadFile(std::string& path);
	bool getline(std::string& outLine);

private:
	std::ifstream m_file;
	std::string m_path;
	

};