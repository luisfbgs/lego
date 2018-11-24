#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "translator.hpp"
#include "line.hpp"

Translator::Translator (std::string source_file) {
    input_file = source_file + ".asm";

    source.open(input_file);
}

void convert_add (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("add edx, ["+ operand + "]");
    ia32.push_back("mov [ACC], eax");
}

void convert_sub (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("sub edx, ["+ operand + "]");
    ia32.push_back("mov [ACC], eax");
}

void convert_mult (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("mov edx, ["+ operand + "]");
    ia32.push_back("imul edx");
    ia32.push_back("mov [ACC], eax");
}

void convert_div (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("cdq");
    ia32.push_back("mov ecx, [" + operand + "]");
    ia32.push_back("idiv ecx");
    ia32.push_back("mov [ACC], eax");
}

void convert_jmp (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("jmp ["+ operand + "]");
}

void convert_jmpn (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("comp eax, 0");
    ia32.push_back("jl " + operand);
}

void convert_jmpp (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("comp eax, 0");
    ia32.push_back("jg " + operand);
}

void convert_jmpz (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("comp eax, 0");
    ia32.push_back("je " + operand);
}

void convert_copy (std::string operand1, std::string operand2, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [" + operand1 + "]");
    ia32.push_back("mov [" + operand2 + "], eax");
}

void convert_load (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [" + operand + "]");
    ia32.push_back("mov [ACC], eax");
}

void convert_store (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, [ACC]");
    ia32.push_back("mov [" + operand + "], eax");
}

void convert_section (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("section ." + operand);
}

void convert_space (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("resd " + operand);
}

void convert_const (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("dd " + operand);
}

void convert_stop (std::vector<std::string> &ia32) {
    ia32.push_back("mov eax, 1");
    ia32.push_back("mov ebx, 0");
    ia32.push_back("int 80h");
}

std::vector<std::string> Translator::izi_to_ia32 (Line izi_line) {
    std::vector<std::string> ia32;

    std::string operation = izi_line.operation;
    std::vector<std::string> operands = izi_line.operands;
    if (operation == "add") {
        convert_add(operands[0], ia32);
    }
    else if (operation == "sub") {
        convert_sub(operands[0], ia32);
    }
    else if (operation == "mult") {
        convert_mult(operands[0], ia32);
    }
    else if (operation == "div") {
        convert_div(operands[0], ia32);
    }
    else if (operation == "jmp") {
        convert_jmp(operands[0], ia32);
    }
    else if (operation == "jmpn") {
        convert_jmpn(operands[0], ia32);
    }
    else if (operation == "jmpp") {
        convert_jmpp(operands[0], ia32);
    }
    else if (operation == "jmpz") {
        convert_jmpz(operands[0], ia32);
    }
    else if (operation == "copy") {
        convert_copy(operands[0], operands[1], ia32);
    }
    else if (operation == "load") {
        convert_load(operands[0], ia32);
    }
    else if (operation == "store") {
        convert_store(operands[0], ia32);
    }
    else if (operation == "section") {
        convert_section(operands[0], ia32);
    }
    else if (operation == "space") {
        if (operands.empty()) {
            convert_space("1", ia32);
        }
        else {
            convert_space(operands[0], ia32);
        }
    }
    else if (operation == "const") {
        convert_const(operands[0], ia32);
    }
    else if (operation == "stop") {
        convert_stop(ia32);
    }

    if (!izi_line.label.empty()) {
        if (ia32.empty()) {
            ia32.push_back(izi_line.label + ": ");
        }
        else {
            ia32[0] = izi_line.label + ": " + ia32[0];
        }
    }

    return ia32;
}

std::vector<Line> Translator::pre_process () {
    std::vector<Line> code;

    std::string raw_line;
    while (getline(source, raw_line)) {
        Line line(raw_line);
        code.push_back(line);
    }

    return code;
}

std::vector<std::string> Translator::translate (std::vector<Line> code) {
    std::vector<std::string> ia32_code;

    for (Line izi_line : code) {

        std::vector<std::string> ia32 = izi_to_ia32(izi_line);
        for (std::string ia32_line : ia32) {
            ia32_code.push_back(ia32_line);
        }
    }

    return ia32_code;
}