#include <iostream>
#include "twopass.hpp"

int main(int argc, const char *argv[]) {
	if(argc != 2) {
		std::cout << "Deve ser fornecido o nome do arquivo a ser montado" << std::endl;
		return 1;
	}
	TwoPass algorithm(argv[1]);
	algorithm.second_pass(algorithm.first_pass());
}
