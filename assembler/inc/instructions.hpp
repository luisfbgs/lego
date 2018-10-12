#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <map>

struct Instruction {
    Instruction(std::string mnemonic, uint8_t operands, uint8_t op_code, uint8_t size) :
        mnemonic(mnemonic), operands(operands), op_code(op_code), size(size) {}

    std::string mnemonic;

    uint8_t operands;

    uint8_t op_code;

    uint8_t size;
};

std::map<std::string, Instruction> instruction_table {
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

#endif