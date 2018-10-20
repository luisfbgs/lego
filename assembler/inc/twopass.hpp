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

		bool resolve_if(Line line);
		void resolve_equ(Line line);
		uint16_t resolve_space(Line line);

		// First element of the pair = size of directive
		// Second element of the pair = true if the next line must be ignored
		std::pair<uint16_t, bool> resolve_directive(Line line);

		void store_label(Line line, int position_count);
		void replace_equ(std::vector<std::string> &operands);

	public:

		TwoPass(std::string source_file);

		std::vector<Line> first_pass();

		void second_pass(std::vector<Line> code);

};

#endif
