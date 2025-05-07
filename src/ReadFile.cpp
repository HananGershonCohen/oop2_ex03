#include "ReadFile.h"

ReadFile::ReadFile(std::string& path) : m_path(path) , m_file(path)
{
	if (!m_file) // if file not open 
	{
		throw std::runtime_error("Failed to open file: " + path);
	}
}


bool ReadFile::getline(std::string& outLine)
{
	return static_cast<bool>(std::getline(m_file, outLine));
}
