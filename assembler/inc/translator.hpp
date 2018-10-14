#ifndef TRANSLATOR_H
#define TRANSLATOR_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <map>

namespace Translate {

uint8_t get_value(std::string operand, std::map<std::string, uint8_t> symbol_table);

std::vector<uint8_t> to_machine_code(std::string operation, std::vector<std::string> operands, uint8_t type, std::map<std::string, uint8_t> symbol_table);

} // end namespace Translate

#endif
