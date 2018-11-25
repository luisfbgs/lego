#ifndef TRANSLTOR_H
#define TRANSLTOR_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>
#include <fstream>
#include "line.hpp"

class Translator {

    private:

        std::string input_file;
        std::string ia32_file;

        std::ifstream source;
        std::ofstream out;

        std::vector<std::string> izi_to_ia32(Line line);

    public:

        Translator(std::string source_file);

        std::vector<Line> pre_process();

        void translate(std::vector<Line> code);
};

#endif
