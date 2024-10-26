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

void PushNumberU16(std::vector<uint8_t> &binSource, Token number) {
	if (number.type == IMMEDIATE) {
		char *end;
		uint16_t value = (uint16_t)std::strtol(number.value.c_str(), &end, 10);
		binSource.push_back(value & 0xFF);
		binSource.push_back((value >> 8) & 0xFF);
	} else if (number.type == HEX) {
		uint16_t value = static_cast<uint16_t>(std::stoul(number.value, nullptr, 16));
		binSource.push_back(value & 0xFF);
		binSource.push_back((value >> 8) & 0xFF);
	} else {
		ERROR("Invalid VALUE for translation!");
		exit(1);
	}
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
					case IMMEDIATE:
					case HEX: {
						program.push_back(MOV_IM);
						program.push_back(StringToRegister(dest.value));
						PushNumberU16(program, value);
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(ADD_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADD_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(ADD_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid ADD destination!");
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(SUB_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SUB_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SUB_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(MUL_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(MUL_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(MUL_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid MUL destination!");
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(DIV_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(DIV_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(DIV_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid DIV destination!");
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(ADC_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(ADC_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(ADC_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid ADC destination!");
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
						case IMMEDIATE:
						case HEX: {
							program.push_back(SBC_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SBC_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SBC_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SBC destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "INC") {
				Token reg = NextToken(pos, tokens);

				program.push_back(INC_R);
				program.push_back(StringToRegister(reg.value));
			} else if (tok.value == "DEC") {
				Token reg = NextToken(pos, tokens);

				program.push_back(DEC_R);
				program.push_back(StringToRegister(reg.value));
			} else if (tok.value == "AND") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(AND_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(AND_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(AND_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(AND_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid AND destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "OR") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(OR_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(OR_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(OR_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(OR_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid OR destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "XOR") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(XOR_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(XOR_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(XOR_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(XOR_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid XOR destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "NOT") {
				Token dest = NextToken(pos, tokens);
				Token reg = NextTokenC(pos, tokens);

				program.push_back(NOT_R);
				program.push_back(StringToRegister(dest.value));
				program.push_back(StringToRegister(reg.value));
			} else if (tok.value == "SHL") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SHL_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SHL_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SHL_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SHL_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SHL destination!");
					exit(1);
				} break;
				}
			} else if (tok.value == "SHR") {
				Token dest = NextToken(pos, tokens);
				switch (dest.type) {
				case REG: {
					Token value1 = NextTokenC(pos, tokens);
					switch (value1.type) {
					case REG: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SHR_R);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SHR_RI);
							program.push_back(StringToRegister(dest.value));
							program.push_back(StringToRegister(value1.value));
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					case IMMEDIATE:
					case HEX: {
						Token value2 = NextTokenC(pos, tokens);
						switch (value2.type) {
						case REG: {
							program.push_back(SHR_IR);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							program.push_back(StringToRegister(value2.value));
						} break;
						case IMMEDIATE:
						case HEX: {
							program.push_back(SHR_I);
							program.push_back(StringToRegister(dest.value));
							PushNumberU16(program, value1);
							PushNumberU16(program, value2);
						} break;
						}
					} break;
					}
				} break;
				default: {
					ERROR("invalid SHR destination!");
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
					case IMMEDIATE:
					case HEX: {
						program.push_back(CMP_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				case IMMEDIATE:
				case HEX: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(CMP_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				}
			} else if (tok.value == "IGT") {
				Token value1 = NextToken(pos, tokens);
				switch (value1.type) {
				case REG: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(IGT_R);
						program.push_back(StringToRegister(value1.value));
						program.push_back(StringToRegister(value2.value));
					} break;
					case IMMEDIATE:
					case HEX: {
						program.push_back(IGT_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				case IMMEDIATE:
				case HEX: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(IGT_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				}
			} else if (tok.value == "ILT") {
				Token value1 = NextToken(pos, tokens);
				switch (value1.type) {
				case REG: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(ILT_R);
						program.push_back(StringToRegister(value1.value));
						program.push_back(StringToRegister(value2.value));
					} break;
					case IMMEDIATE:
					case HEX: {
						program.push_back(ILT_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				case IMMEDIATE:
				case HEX: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(ILT_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				}
			} else if (tok.value == "IGE") {
				Token value1 = NextToken(pos, tokens);
				switch (value1.type) {
				case REG: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(IGE_R);
						program.push_back(StringToRegister(value1.value));
						program.push_back(StringToRegister(value2.value));
					} break;
					case IMMEDIATE:
					case HEX: {
						program.push_back(IGE_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				case IMMEDIATE:
				case HEX: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(IGE_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				}
			} else if (tok.value == "ILE") {
				Token value1 = NextToken(pos, tokens);
				switch (value1.type) {
				case REG: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(ILE_R);
						program.push_back(StringToRegister(value1.value));
						program.push_back(StringToRegister(value2.value));
					} break;
					case IMMEDIATE:
					case HEX: {
						program.push_back(ILE_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				case IMMEDIATE:
				case HEX: {
					Token value2 = NextTokenC(pos, tokens);
					switch (value2.type) {
					case REG: {
						program.push_back(ILT_RI);
						program.push_back(StringToRegister(value1.value));
						PushNumberU16(program, value2);
					} break;
					}
				} break;
				}
			} else if (tok.value == "JMP") {
				GotoLabel(JMP, tokens, program, pos);
			} else if (tok.value == "JNZ") {
				GotoLabel(JNZ, tokens, program, pos);
			} else if (tok.value == "JZ") {
				GotoLabel(JZ, tokens, program, pos);
			} else if (tok.value == "JNC") {
				GotoLabel(JNC, tokens, program, pos);
			} else if (tok.value == "JC") {
				GotoLabel(JZ, tokens, program, pos);
			} else if (tok.value == "JNS") {
				GotoLabel(JNS, tokens, program, pos);
			} else if (tok.value == "JS") {
				GotoLabel(JS, tokens, program, pos);
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

void Assemble::GotoLabel(uint8_t opcode, std::vector<Token> tokens, std::vector<uint8_t>& program, int& pos) {
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
	program.push_back(opcode);
	PushNumberU16(program, Token { .type = IMMEDIATE, .value = std::to_string(addr) });
}

}