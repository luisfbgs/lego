#ifndef TABLES_H
#define TABLES_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <map>

namespace Tables {

	struct Instruction {
		Instruction() {}

		Instruction(std::string mnemonic, uint8_t operands, uint8_t op_code, uint8_t size) :
			mnemonic(mnemonic), operands(operands), op_code(op_code), size(size) {}

		std::string mnemonic;

		uint8_t operands;

		uint8_t op_code;

		uint8_t size;

	};

	struct Directive {
		Directive() {}

		Directive(std::string mnemonic, uint8_t id, uint8_t operands, uint8_t size) :
			mnemonic(mnemonic), id(id), operands(operands), size(size) {}

		std::string mnemonic;

		uint8_t id;

		uint8_t operands;

		uint8_t size;

	};

	extern const std::map<std::string, Instruction> instructions;

	extern const std::map<std::string, Directive> directives;

	extern std::map<std::string, uint16_t> symbols;

} // end namespace Tables

#endif
