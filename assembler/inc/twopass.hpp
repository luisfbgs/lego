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

		std::ifstream source;
		std::ofstream pre;
		std::ofstream obj;
 		
		void write_directive(Line line);
		
		void store_label(Line line, int position_count);

		void replace_equ(std::vector<std::string> &operands);

	public:

		enum Sections {
			NONE,
			TEXT,
			DATA,
			BSS
		};

		static Sections section;

		TwoPass(std::string source_file);

		std::vector<Line> first_pass();

		void second_pass(std::vector<Line> code);

		static std::vector<std::string> error_list;

};

#endif
