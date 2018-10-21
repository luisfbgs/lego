#ifndef DRIVER_H
#define DRIVER_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES AND DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <string>
#include <vector>
#include <map>

class  Module {

public:

    Module(std::string filename);

    std::map<std::string, std::vector<uint32_t>> use_table;

    std::map<std::string, uint32_t> definition_table;

    std::vector<uint32_t> relatives;

    std::vector<uint32_t> code;

    uint32_t size();

};

class Driver {
    
public:

    Driver(std::vector<std::string> files);

    void run();


private:

    void create_gdf();

    void create_executable();

    void treat_one_file();

    std::vector<std::string> files;

    std::string out_file;

    std::vector<Module> modules;

    std::map<std::string, uint32_t> gdf;

};

#endif
