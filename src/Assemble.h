#pragma once

#include <string>

#include "Lexer.h"

namespace REASM {

class Assemble {
public:
	Assemble(std::string FilePath);
private:
	void GotoLabel(uint8_t opcode, std::vector<Token> tokens, std::vector<uint8_t> &program, int &pos);
	void MOWD(uint8_t opcode, std::vector<Token> tokens, std::vector<uint8_t> &program, int &pos);
private:
	struct Label {
		std::string name;
		uint16_t addr;
	};
private:
	uint16_t m_OrgAddr = 0x0000;

	std::vector<Label> m_Labels;
};

}