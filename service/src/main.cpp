#include <sdbus-c++/sdbus-c++.h>
#include <cstdlib>
#include <map>
#include <string>
#include "service.h"

const std::string getFolderPath(int argc, char* argv[]) {
    std::string folderPath =
        std::getenv("HOME") + std::string("/com.system.configurationManager/");

    if(argc != 1 && argc != 3) {
        throw std::runtime_error("Incorrect number of arguments: " + std::to_string(argc));
    }

    if (argc == 3) {
        if (std::string(argv[1]) == "-d") {
            folderPath = argv[2];
        } else {
            throw std::runtime_error("Invalid flag: " + std::string(argv[1]));
        }
    }

    return folderPath;
}

int main(int argc, char* argv[]) {
    try {
        auto ConfigFolderPath = getFolderPath(argc, argv);

        Service service(ConfigFolderPath);

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}