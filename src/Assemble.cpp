#include "Assemble.h"

#include <fstream>

#include "Defines/Opcodes.h"
#include "Defines/Registers.h"

namespace REASM {

uint16_t StringToU16(std::string number) {
	char *end;
	uint16_t value = (uint16_t)std::stoul(number, nullptr, 16);

	return value;
}
	
uint8_t StringToU8(std::string number) {
	char *end;
	uint8_t value = (uint8_t)std::strtol(number.c_str(), &end, 10);

	return value;
}

uint8_t StringToRegister(std::string reg) {
	if (reg == "R0") return R0;
	if (reg == "R1") return R1;
	if (reg == "R2") return R2;
	if (reg == "R3") return R3;
	if (reg == "R4") return R4;
	if (reg == "R5") return R5;
	if (reg == "R6") return R6;
	if (reg == "R7") return R7;
	if (reg == "R8") return R8;
	if (reg == "R9") return R9;
	if (reg == "R10") return R10;
	if (reg == "R11") return R11;

	ERROR("Invalid Register: {0}", reg);
	exit(1);
}

void PushU16(std::vector<uint8_t> &binSource, uint16_t number) {
	binSource.push_back((number >> 8) & 0xFF);
	binSource.push_back(number & 0xFF);
}

void PushNumberU16(std::vector<uint8_t> &binSource, std::string number) {
	char *end;
	uint16_t value = (uint16_t)std::strtol(number.c_str(), &end, 10);
	binSource.push_back(value & 0xFF);
	binSource.push_back((value >> 8) & 0xFF);
}
	
void PushNumberU8(std::vector<uint8_t> &binSource, std::string number) {
	char *end;
	uint8_t value = (uint8_t)std::strtol(number.c_str(), &end, 10);
	binSource.push_back(value);
}

static Token NextToken(int &iter, std::vector<Token> &tokens) {
	if (iter + 1 >= tokens.size()) {
		ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
		exit(1);
	} else iter++;
	return tokens[iter];
}

static Token ExpectNextToken(int &iter, std::vector<Token> &tokens, TokenType expect) {
	if (iter + 1 >= tokens.size()) {
		ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
		exit(1);
	} else iter++;

	if (tokens[iter].type != expect) {
		ERROR("Expected: {0}, got: {1}", TokenTypeToString(expect), TokenTypeToString(tokens[iter].type));
		exit(1);
	}
	return tokens[iter];
}

static Token NextTokenC(int &iter, std::vector<Token> &tokens) {
	ExpectNextToken(iter, tokens, COMMA);
	if (iter + 1 >= tokens.size()) {
		ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
		exit(1);
	} else iter++;
	return tokens[iter];
}

static void ExpectNextTokenV(int &iter, std::vector<Token> &tokens, TokenType expect) {
	if (iter + 1 >= tokens.size()) {
		ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
		exit(1);
	}

	if (tokens[iter + 1].type == expect) {
		return;
	} else {
		ERROR("Expected: {0}, got: {1}", TokenTypeToString(expect), TokenTypeToString(tokens[iter].type));
		exit(1);
	}
}

static bool ExpectNextTokenB(int &iter, std::vector<Token> &tokens, TokenType expect) {
	if (iter + 1 >= tokens.size()) {
		ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
		exit(1);
	}

	if (tokens[iter + 1].type == expect) {
		return true;
	}

	return false;
}

static std::vector<Token> GetOperands(std::vector<Token>& tokens, int& pos) {
	std::vector<Token> operands;
	
	pos += 1;

	bool end = false;
	while (!end) {
		if (pos >= tokens.size()) {
			end = true;
			break;
		}

		operands.push_back(tokens[pos]);

		if (pos + 1 < tokens.size()) {
			if (ExpectNextTokenB(pos, tokens, COMMA)) {
				pos += 1;
			} else {
				end = true;
			}
		} else {
			end = true;
		}

		pos++;
	}

	return operands;
}

Assemble::Assemble(std::string filePath) {
	std::vector<Token> tokens = Lexer::LexFile(filePath);
	
	std::vector<uint8_t> program;

	int pos = 0;
	while (pos < tokens.size()) {
		Token tok = tokens[pos];
		switch (tok.type) {
		case ORG: {
			m_OrgAddr = StringToU16(ExpectNextToken(pos, tokens, HEX).value);
		} break;
		case LABEL: {
			m_Labels.push_back(Label { .name = tok.value, .addr = static_cast<uint16_t>(m_OrgAddr + program.size()) });
		} break;
		case OPCODE: {
			if (tok.value == "MOV") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value = NextTokenC(pos, tokens);
					switch (value.type) {
					case IMMEDIATE: {
						program.push_back(MOV_IM);
						program.push_back(StringToRegister(dest.value));
						PushNumberU16(program, value.value);
					} break;
					case REG: {
						program.push_back(MOV_R);
						program.push_back(StringToRegister(dest.value));
						program.push_back(StringToRegister(value.value));
					} break;
					}
				} break;
				default: {
					ERROR("invalid MOV destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "ADD") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADD_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(ADD_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADD_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(ADD_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "SUB") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SUB_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(SUB_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SUB_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(SUB_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "MUL") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(MUL_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(MUL_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(MUL_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(MUL_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "DIV") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(DIV_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(DIV_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(DIV_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(DIV_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "ADC") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADC_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(ADC_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADC_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(ADC_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "SBC") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SBC_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(SBC_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					case IMMEDIATE: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SBC_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE: {
							program.push_back(SBC_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1.value);
							PushNumberU16(program, value2.value);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "CMP") {
				Token value1 = NextToken(pos, tokens);
				switch (value1.type) {
				case REG: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(CMP_R);
						program.push_back(StringToRegister(value1.value));
						program.push_back(StringToRegister(value2.value));
					} break;
					case IMMEDIATE: {
						program.push_back(CMP_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2.value);
					} break;
					}
				} break;
				case IMMEDIATE: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(CMP_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2.value);
					} break;
					}
				} break;
				default: {
					ERROR("invalid SUB destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "JZ") {
				uint16_t addr;
				for (Label label : m_Labels) {
					if (label.name == ExpectNextToken(pos, tokens, LABEL).value) {
						addr = label.addr;
						goto no_error;
					}
				}

				ERROR("Invalid LABEL!");
				exit(1);
			no_error:
				program.push_back(JZ);
				PushNumberU16(program, std::to_string(addr));
			} else if (tok.value == "HLT") {
				program.push_back(HLT);
			};
		} break;
		}

		pos++;
	}

	// Generate File
	std::ofstream file("output.bin", std::ios::binary);
	if (!file) {
		ERROR("Unabled to open file for write!");
		exit(1);
	}

	file.write(reinterpret_cast<const char*>(program.data()), program.size());

	file.close();
}

}