#include "twopass.hpp"

int main() {
	TwoPass algorithm("teste");
	algorithm.second_pass(algorithm.first_pass());
}
