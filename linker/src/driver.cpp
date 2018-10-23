#include <map>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "driver.hpp"
#include "helpers.hpp"

/******** START MODULE ********/

Module::Module(std::string filename) {
    std::ifstream file(filename);
    
    std::string line;
    std::vector<std::string> key_value;
    std::vector<std::string> values;
    while (getline(file, line)) {
        if (line == "TABLE USE") {
            while (getline(file, line) && !line.empty()) {
                key_value = Helpers::split_invisible_semicolon(line);
                use_table[key_value[0]].push_back(stoi(key_value[1]));
            }
        }
        else if (line == "TABLE DEFINITION") {
            while (getline(file, line) && !line.empty())  {
                key_value = Helpers::split_invisible_semicolon(line);
                definition_table[key_value[0]] = stoi(key_value[1]);
            }
        }
        else if (line == "RELATIVE") {
            while (getline(file, line) && !line.empty())  {
                values = Helpers::split_invisible_semicolon(line);
                for (auto s : values) relatives.push_back(stoi(s));
            }
        }
        else if (line == "CODE") {
            while (getline(file, line) && !line.empty())  {
                values = Helpers::split_invisible_semicolon(line);
                for (auto s : values) code.push_back(stoi(s));
            }
        }
    }
}

uint32_t Module::size() {
    return code.size();
}

/******** END MODULE ********/

/******** START DRIVER ********/

Driver::Driver(std::vector<std::string> files) : files(files) {
    out_file = files[0];
    Helpers::replace(out_file, ".obj", ".e");
}

void Driver::run() {
    std::cout << files.size() << " arquivos encontrados..." << std::endl;
    for (auto file : files) {
        std::cout << "Escaneando tabelas do arquivo " << file << std::endl;
        Module module(file);
        modules.push_back(module);
    }

    if (files.size() > 1) {
        std::cout << "Criando tabela global de definições" << std::endl;
        create_gdf();
        std::cout << "Criando arquivo executável" << std::endl;
        create_executable();
    }
    else {
        treat_one_file();
    }
}

void Driver::create_gdf() {
    uint32_t correction_factor = 0;

    for (auto module : modules) {
        for (auto definition : module.definition_table) {
            gdf[definition.first] = definition.second + correction_factor;
        }

        correction_factor += module.size();
    }
}

void Driver::create_executable() {
    std::ofstream output(out_file);
    uint32_t correction_factor = 0;

    for (auto module : modules) {
        std::vector<uint32_t> code = module.code;

        std::set<uint32_t> uses;
        std::set<uint32_t> relatives;
        for (auto use : module.use_table) {
            for (auto i : use.second) {
                code[i] = gdf[use.first];
                uses.insert(i);
            }
        }

        if (!module.relatives.empty()) {
            for (int i : module.relatives) {
                if (uses.count(i) == 0) relatives.insert(i);
            }
        }

        for (int s : relatives) {
            code[s] = code[s] + correction_factor;
        }

        for (int i :  code) {
            output << i << " ";
        }

        correction_factor += module.size();
    }
}

void Driver::treat_one_file() {
    std::ofstream output(out_file);
    Module module = modules[0];

    std::vector<uint32_t> code = module.code;
    for (int i :  code) {
        output << i << " ";
    }
}

/******** END DRIVER ********/