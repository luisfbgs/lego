#include <iostream>
#include "translator.hpp"
#include "line.hpp"

int main(int argc, const char *argv[]) {
	if(argc != 2) {
		std::cout << "Deve ser fornecido o nome do arquivo a ser traduzido" << std::endl;
		return 1;
	}

    Translator algo(argv[1]);

    std::vector<Line> v = algo.pre_process();
    for (auto i : v){
        std::cout << i.label << " " << i.operation;
        for (auto operand : i.operands) {
            std::cout << " " << operand;
        }
        std::cout << std::endl;
    }
}
