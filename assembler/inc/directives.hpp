#ifndef DIRECTIVES_H
#define DIRECTIVES_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include "line.hpp"

namespace Directives {

		bool resolve_if(Line line);

		void resolve_equ(Line line);
		
		uint16_t resolve_space(Line line);
		
		void resolve_const(Line line);

		void resolve_section(Line line);
		
		void resolve_extern(Line line);

		void resolve_public(Line line);

		// First element of the pair = size of directive
		// Second element of the pair = true if the next line must be ignored
		std::pair<uint16_t, bool> resolve(Line line);

} // end namespace Directives

#endif
