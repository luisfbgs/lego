#include <map>
#include <string>
#include "tables.hpp"

namespace Tables{

	const std::map<std::string, Directive> directives {
		{ "if",      Directive("if",      1, 1, 0) },
		{ "equ",     Directive("equ",     2, 1, 0) },
		{ "section", Directive("section", 3, 1, 0) },
		{ "space",   Directive("space",   4, 1, 1) },
		{ "const",   Directive("const",   5, 1, 1) },
		{ "public",  Directive("public",  6, 0, 0) },
		{ "extern",  Directive("extern",  7, 0, 0) },
		{ "begin",   Directive("begin",   8, 0, 0) },
		{ "end",     Directive("end",     9, 0, 0) },
	};

	const std::map<std::string, Instruction> instructions {
		{ "add",    Instruction("add",    1, 1,  2) },
		{ "sub",    Instruction("sub",    1, 2,  2) },
		{ "mult",   Instruction("mult",   1, 3,  2) },
		{ "div",    Instruction("div",    1, 4,  2) },
		{ "jmp",    Instruction("jmp",    1, 5,  2) },
		{ "jmpn",   Instruction("jmpn",   1, 6,  2) },
		{ "jmpp",   Instruction("jmpp",   1, 7,  2) },
		{ "jmpz",   Instruction("jmpz",   1, 8,  2) },
		{ "copy",   Instruction("copy",   2, 9,  3) },
		{ "load",   Instruction("load",   1, 10, 2) },
		{ "store",  Instruction("store",  1, 11, 2) },
		{ "input",  Instruction("input",  1, 12, 2) },
		{ "output", Instruction("output", 1, 13, 2) },
		{ "stop",   Instruction("stop",   0, 14, 1) },
	};

	std::map<std::string, uint16_t> symbols;
}
