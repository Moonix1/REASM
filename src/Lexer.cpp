#include "Lexer.h"
#include "Log/Log.h"

#include <fstream>
#include <algorithm>

namespace REASM {

static std::string trim(std::string &str) {
	str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    }));

    str.erase(std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
        return std::isspace(c);
    }).base(), str.end());

    return str;
}

static Token TokenizeWord(int &pos, std::string line, std::vector<Token> tokens) {
	int start = pos;
	while (pos < line.size() && std::isalnum(line[pos])
			|| line[pos] == '_' || line[pos] == '.') { pos++; }
	std::string value = line.substr(start, pos - start);
	
	if (value == "MOV"
		|| value == "ADD"
		|| value == "SUB"
		|| value == "MUL"
		|| value == "DIV"
		|| value == "ADC"
		|| value == "SBC"
		|| value == "CMP"
		|| value == "JZ"
		|| value == "HLT")
		return Token { .type = OPCODE, .value = value };

	if (value == "R0"
		|| value == "R1"
		|| value == "R2"
		|| value == "R3"
		|| value == "R4"
		|| value == "R5"
		|| value == "R6"
		|| value == "R7"
		|| value == "R8"
		|| value == "R9"
		|| value == "R10"
		|| value == "R11")
		return Token { .type = REG, .value = value };

	if (value == "ORG")
		return Token { .type = ORG, .value = value };

	for (Token tok : tokens) {
		if (tok.type == LABEL && tok.value == value) {
			return { .type = LABEL, .value = value };
		}
	}

	return { .type = UNKNOWN, .value = value };
}

static Token TokenizeImmediate(int &pos, std::string line) {
	pos++;
	int start = pos;
	while (pos < line.size() && std::isdigit(line[pos])) { pos++; }
	std::string value = line.substr(start, pos - start);

	return { .type = IMMEDIATE, .value = value };
}

static Token TokenizeHex(int &pos, std::string line) {
	int start = pos;
	while (pos < line.size() && std::isalnum(line[pos])) { pos++; }
	std::string value = line.substr(start, pos - start);

	return { .type = HEX, .value = value };
}

static void TokenizeLine(std::string line, std::vector<Token> &tokens) {
    line = trim(line);

    if (!line.empty()) {
        if (line.find(':') == line.size() - 1) {
            line.pop_back();
            tokens.push_back(Token { .type = LABEL, .value = line });
        } else {
            int pos = 0;
            while (pos < line.size()) {
                char current = line[pos];

                if (std::isspace(current)) {
                    pos++;
                    continue;
                }

                if (current == '#') {
                    tokens.push_back(TokenizeImmediate(pos, line));
                    continue;
                }

                if (current == '0' && pos + 1 < line.size() && line[pos + 1] == 'x') {
                    tokens.push_back(TokenizeHex(pos, line));
                    continue;
                }

                if (std::isalpha(current) || current == '_' || current == '.') {
                    tokens.push_back(TokenizeWord(pos, line, tokens));
                    continue;
				}

                switch (current) {
                case ',': tokens.push_back(Token { .type = COMMA, .value = std::string(1, current) }); break;
                case '(': tokens.push_back(Token { .type = LPAREN, .value = std::string(1, current) }); break;
                case ')': tokens.push_back(Token { .type = RPAREN, .value = std::string(1, current) }); break;
                case '[': tokens.push_back(Token { .type = LBRACE, .value = std::string(1, current) }); break;
                case ']': tokens.push_back(Token { .type = RBRACE, .value = std::string(1, current) }); break;
                case '{': tokens.push_back(Token { .type = LCURLYBRACE, .value = std::string(1, current) }); break;
                case '}': tokens.push_back(Token { .type = RCURLYBRACE, .value = std::string(1, current) }); break;
                default: break;
                }

                pos++;
            }
        }
    }
}

std::vector<Token> Lexer::LexFile(std::string path)
{
	std::ifstream file(path);
	std::string line;

	std::vector<Token> tokens;
	while (std::getline(file, line)) {
		TokenizeLine(line, tokens);
	}

	for (Token tok : tokens) {
		INFO("{0} -> {1}", TokenTypeToString(tok.type), tok.value);
	}

	return tokens;
}

}