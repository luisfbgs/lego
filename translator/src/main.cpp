#include <iostream>
#include "translator.hpp"
#include "line.hpp"

int main(int argc, const char *argv[]) {
	if(argc != 2) {
		std::cout << "Deve ser fornecido o nome do arquivo a ser traduzido" << std::endl;
		return 1;
	}

    Translator algo(argv[1]);

    std::vector<std::string> ia32_code = algo.translate(algo.pre_process());

    for (auto i : ia32_code){
        std::cout << i << std::endl;
    }
}
