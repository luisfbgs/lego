#ifndef DIRECTIVES_H
#define DIRECTIVES_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <map>

struct Directive {
    Directive(std::string mnemonic, uint8_t operands, uint8_t size) :
        mnemonic(mnemonic), operands(operands), size(size) {}

    std::string mnemonic;

    uint8_t operands;

    uint8_t size;
};

std::map<std::string, Directive> directive_table {
    { "if",      Directive("if",      1, 0) },
    { "equ",     Directive("equ",     1, 0) },
    { "section", Directive("section", 1, 0) },
    { "space",   Directive("space",   1, 1) },
    { "const",   Directive("const",   1, 1) },
    { "public",  Directive("public",  0, 0) },
    { "extern",  Directive("extern",  0, 0) },
    { "begin",   Directive("begin",   0, 0) },
    { "end",     Directive("end",     0, 0) },
};

#endif