#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include "instructions.hpp"
#include "directives.hpp"
#include "split.hpp"

int resolve_directive(std::string s){
	return 1;
}

std::map<std::string, int> symbol_table;

bool first_pass(string source_file){
    int position_count = 0;
    int line_count = 0;
    std::ifstream source;
    source.open(source_file);
    std::string line;
    while(getline(source, line)){
        std::vector<std::string> aux_elements = Helper::split(line, ' ');
        std::vector<std::string> elements;
		for(auto it : aux_elements){
			if(it[0] == ' ' || it.empty()) continue;
			if(it[0] == ';') break;
			elements.push_back(it);
		}
		if(elements.empty()) continue;
		
		std::string rot, operation;
		
		int pos = 0;

		if(elements[pos].back() == ':'){
			rot = elements[pos];
			if(symbol_table.count(rot)){
				cout << "ERRO rotulo redefinido na linha " << line_count << endl;
				return false;
			}
			else{
				symbol_table[rot] = position_count;
			}
			pos++;
		}

		operation = elements[pos];
		if(Tables::instruction_table.count(operation)){
			position_count += Tables::instruction_table[operation].size;
		}
		else if(Tables::directive_table.count(operation)){
			position_count += resolve_directive(operation);
		}
		else{
			cout << "ERRO operacao nao identificada na linha " << line_count << endl;
			return false;
		}
		line_count++;
    }
	return true;
}

int main(int argc, char *argv []){
	first_pass(argv[0]);
}

