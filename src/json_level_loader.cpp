#include "json_level_loader.hpp"
#include <iostream>
#include <fstream>


namespace Level_Loader {

json load_level(std::string path){
    std::ifstream i(path);
    json output;
    i >> output;
    // if you want to debug you could get a string like this:
    // std::string s = output.dump(); 
    return output;
}

}