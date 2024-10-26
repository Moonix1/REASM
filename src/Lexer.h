#pragma once

#include <vector>

#include "Token.h"

namespace REASM {
	
class Lexer {
public:
	static std::vector<Token> LexFile(std::string path);
};

}