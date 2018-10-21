#include <vector>
#include <string>
#include <iostream>
#include "driver.hpp"

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        std::cout << "Pelo menos um arquivo deve ser indicado, use como:" << std::endl;
        std::cout << "    ./ligador arquivo1 [arquivo2] [arquivo3] [arquivo4]";
        return 1;
    }

    if (argc > 5) {
        std::cout << "No máximo 4 arquivos podem ser indicados, use como:" << std::endl;
        std::cout << "    ./ligador arquivo1 [arquivo2] [arquivo3] [arquivo4]";
        return 1;
    }

    std::vector<std::string> files;
    for (int i = 1; i < argc; i++) {
        std::string filename(argv[i]);
        files.push_back(filename + ".obj");
    }

    std::cout << "Inicializando ligador" << std::endl;
    Driver d(files);

    std::cout << "Iniciando ligação dos arquivos" << std::endl;
    d.run();
};