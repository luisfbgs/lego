#ifndef PASSES_H
#define PASSES_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <fstream>
#include "line.hpp"

class TwoPass {

	private:

		std::string input_file;
		std::string pre_file;
		std::string obj_file;

		std::vector<std::string> error_list;
		std::ifstream source;
		std::ofstream pre;
		std::ofstream obj;

		enum Sections {
			NONE,
			TEXT,
			DATA,
			BSS
		};

		Sections section = NONE;

		// First element of the pair = size of directive
		// Second element of the pair = type of directive, 1 for IF, 2 for EQU, 0 for the others
		std::pair<uint16_t, int> resolve_directive(std::string operation, std::vector<std::string> operands);

	public:

		TwoPass(std::string source_file);

		std::vector<Line> first_pass();

		void second_pass(std::vector<Line> code);

};

#endif
