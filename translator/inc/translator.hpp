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
        std::string obj_file;

        std::ifstream source;

    public:

        Translator(std::string source_file);

        std::vector<Line> pre_process();
};

#endif
