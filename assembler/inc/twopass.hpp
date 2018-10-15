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
			DATA,
			TEXT,
			BSS
		};

		Sections section = NONE;

		int resolve_directive(std::string s);

	public:

		TwoPass(std::string source_file);

		std::vector<Line> first_pass();

		void second_pass(std::vector<Line> code);

};

#endif
