#pragma once

#include <fstream>
#include <sstream>
#include <string>

namespace REASM {

std::string GetFileContents(std::string path) {
	std::ifstream file(path);
	std::stringstream buffer;

	buffer << file.rdbuf();
	return buffer.str();
}

}