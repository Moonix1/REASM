#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Log/Log.h"

namespace REASM {

enum TokenType {
	OPCODE,

	ORG,

	IMMEDIATE,
	HEX,
	REG,

	LABEL,

	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	LCURLYBRACE,
	RCURLYBRACE,
	COLON,
	SEMICOLON,
	COMMA,

	UNKNOWN,
};

struct Token {
	TokenType type;
	std::string value;
};

inline std::string TokenTypeToString(TokenType type) {
	std::unordered_map<TokenType, std::string> tokenTypeToString = {
		{ TokenType::OPCODE, "opcode" },
		{ TokenType::ORG, "org" },
		{ TokenType::IMMEDIATE, "immediate" },
		{ TokenType::HEX, "hex" },
		{ TokenType::REG, "reg" },
		{ TokenType::LABEL, "label" },
		{ TokenType::LPAREN, "lparen" },
		{ TokenType::RPAREN, "rparen" },
		{ TokenType::LBRACE, "lbrace" },
		{ TokenType::RBRACE, "rbrace" },
		{ TokenType::LCURLYBRACE, "lcurlybrace" },
		{ TokenType::RCURLYBRACE, "rcurlybrace" },
		{ TokenType::COLON, "colon" },
		{ TokenType::SEMICOLON, "semicolon" },
		{ TokenType::COMMA, "comma" },
		{ TokenType::UNKNOWN, "unknown" },
	};

	return tokenTypeToString[type];
}

static Token FetchToken(std::vector<Token> &tokens, int &pos) {
	Token result = tokens[pos];
	pos++;
	return result;
}

static Token ExpectToken(std::vector<Token> &tokens, TokenType expect, int &pos) {
	Token token = FetchToken(tokens, pos);
	if (token.type == expect) { ERROR("Unexpected token, expected: {0} got: {1}",
		TokenTypeToString(expect), TokenTypeToString(token.type)); exit(1); }

	return token;
}

}