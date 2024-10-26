#include <iostream>
#include <string>

#include "Log/Log.h"
#include "Assemble.h"

char *Shift(int &argc, char ***argv) {
	char *result = **argv;
	*argv += 1;
	argc -= 1;
	return result;
}

int main(int argc, char **argv) {
	REASM::Log::Init();
	char *program = Shift(argc, &argv);
	if (argc < 1) {
		ERROR("Invalid Arguments!");
		exit(1);
	}

	char *subcommand = Shift(argc, &argv);
	if (std::string(subcommand) == "build") {
		INFO("{0}", argc);
		if (argc < 1) {
			ERROR("Invalid Subargs!");
			exit(1);
		}
		char *input = Shift(argc, &argv);
		REASM::Assemble(std::string(input));
	} else {
		ERROR("Invalid subcommand: {0}", subcommand);
		exit(1);
	}
}