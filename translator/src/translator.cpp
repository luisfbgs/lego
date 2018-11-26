#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "translator.hpp"
#include "line.hpp"

Translator::Translator (std::string source_file) {
    input_file = source_file + ".asm";
    ia32_file  = source_file + ".s";

    source.open(input_file);
}

void convert_add (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    add eax, ["+ operand + "]");
}

void convert_sub (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    sub eax, ["+ operand + "]");
}

void convert_mult (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    mov edx, ["+ operand + "]");
    ia32.push_back("    imul edx");
}

void convert_div (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    cdq");
    ia32.push_back("    mov ecx, [" + operand + "]");
    ia32.push_back("    idiv ecx");
}

void convert_jmp (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    jmp ["+ operand + "]");
}

void convert_jmpn (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    comp eax, 0");
    ia32.push_back("    jl " + operand);
}

void convert_jmpp (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    comp eax, 0");
    ia32.push_back("    jg " + operand);
}

void convert_jmpz (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    comp eax, 0");
    ia32.push_back("    je " + operand);
}

void convert_copy (std::string operand1, std::string operand2, std::vector<std::string> &ia32) {
    ia32.push_back("    mov ebx, [" + operand1 + "]");
    ia32.push_back("    mov [" + operand2 + "], ebx");
}

void convert_load (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    mov eax, [" + operand + "]");
}

void convert_store (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    mov [" + operand + "], eax");
}

void convert_section (std::string operand, std::vector<std::string> &ia32) {
    std::transform(operand.begin(), operand.end(), operand.begin(), ::tolower);
    ia32.push_back("    section ." + operand);
}

void convert_space (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    resd " + operand);
}

void convert_const (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    dd " + operand);
}

void convert_stop (std::vector<std::string> &ia32) {
    ia32.push_back("    mov eax, 1");
    ia32.push_back("    mov ebx, 0");
    ia32.push_back("    int 80h");
}

void convert_in_c (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    push eax");
    ia32.push_back("    push " + operand);
    ia32.push_back("    call LeerChar");
    ia32.push_back("    pop eax");
}

void convert_out_c (std::string operand, std::vector<std::string> &ia32) {
    ia32.push_back("    push eax");
    ia32.push_back("    push " + operand);
    ia32.push_back("    call EscreverChar");
    ia32.push_back("    pop eax");
}

void convert_in_s (std::string operand1, std::string operand2, std::vector<std::string> &ia32) {
    ia32.push_back("    push eax");
    ia32.push_back("    push " + operand2);
    ia32.push_back("    push " + operand1);
    ia32.push_back("    call LeerString");
    ia32.push_back("    pop eax");
}

void convert_out_s (std::string operand1, std::string operand2, std::vector<std::string> &ia32) {
    ia32.push_back("    push eax");
    ia32.push_back("    push " + operand2);
    ia32.push_back("    push " + operand1);
    ia32.push_back("    call EscreverString");
    ia32.push_back("    pop eax");
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
    else if (operation == "input_c") {
        convert_in_c(operands[0], ia32);
    }
    else if (operation == "output_c") {
        convert_out_c(operands[0], ia32);
    }
    else if (operation == "input_s") {
        convert_in_s(operands[0], operands[1], ia32);
    }
    else if (operation == "output_s") {
        convert_out_s(operands[0], operands[1], ia32);
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

void proc_in_c (std::vector<std::string> &ia32_code) {
    ia32_code.push_back("LeerChar:    mov eax, 3");
    ia32_code.push_back("    mov ebx, 1");
    ia32_code.push_back("    mov ecx, [ESP+4]");
    ia32_code.push_back("    mov edx, 1");
    ia32_code.push_back("    int 80h");
    ia32_code.push_back("    ret 4"); 
}

void proc_out_c (std::vector<std::string> &ia32_code) {
    ia32_code.push_back("EscreverChar:    mov eax, 4");
    ia32_code.push_back("    mov ebx, 1");
    ia32_code.push_back("    mov ecx, [ESP+4]");
    ia32_code.push_back("    mov edx, 1");
    ia32_code.push_back("    int 80h");
    ia32_code.push_back("    ret 4"); 
}

void proc_in_s (std::vector<std::string> &ia32_code) {
    ia32_code.push_back("LeerString:");
    ia32_code.push_back("    mov ecx, [ESP+4]");
    ia32_code.push_back("    mov edx, [ESP+8]");
    ia32_code.push_back("    mov eax, 0");
    ia32_code.push_back("LoopLeerString:");
    ia32_code.push_back("    cmp edx, 0");
    ia32_code.push_back("    jle SaiLeerString");
    ia32_code.push_back("    dec edx");
    ia32_code.push_back("    inc eax");
    ia32_code.push_back("    push eax");
    ia32_code.push_back("    push ecx");
    ia32_code.push_back("    push edx");
    ia32_code.push_back("    push ecx");
    ia32_code.push_back("    call LeerChar");
    ia32_code.push_back("    pop edx");
    ia32_code.push_back("    pop ecx");
    ia32_code.push_back("    pop eax");
    ia32_code.push_back("    cmp byte [ecx], 10");
    ia32_code.push_back("    je SaiLeerString");
    ia32_code.push_back("    inc ecx");
    ia32_code.push_back("    jmp LoopLeerString");
    ia32_code.push_back("SaiLeerString:    ret 8"); 
}

void proc_out_s (std::vector<std::string> &ia32_code) {
    ia32_code.push_back("EscreverString:    mov eax, 4");
    ia32_code.push_back("    mov ebx, 1");
    ia32_code.push_back("    mov ecx, [ESP+4]");
    ia32_code.push_back("    mov edx, [ESP+8]");
    ia32_code.push_back("    int 80h");
    ia32_code.push_back("    ret 8"); 
}

void Translator::translate (std::vector<Line> code) {
    std::vector<std::string> ia32_code;

    bool set_start = true;
    for (Line izi_line : code) {
        if (set_start && izi_line.operation == "section" && izi_line.operands[0] == "TEXT") {
            ia32_code.push_back("global _start");
            ia32_code.push_back("_start:");
            set_start = false;
        }
        std::vector<std::string> ia32 = izi_to_ia32(izi_line);
        for (std::string ia32_line : ia32) {
            ia32_code.push_back(ia32_line);
        }
    }

    ia32_code.push_back("section .text");
    proc_in_c(ia32_code);
    proc_out_c(ia32_code);
    proc_in_s(ia32_code);
    proc_out_s(ia32_code);

    out.open(ia32_file);

    for (std::string ia32_line : ia32_code) {
        out << ia32_line << std::endl;
    }
}