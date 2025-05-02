#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include "application.h"

const std::string getFilePath(int argc, char* argv[]) {
    std::string filePath =
        std::getenv("HOME") +
        std::string("/com.system.configurationManager/confManagerApplication1.json");

    if(argc != 1 && argc != 3) {
        throw std::runtime_error("Incorrect number of arguments: " + std::to_string(argc));
    }

    if (argc == 3) {
        if (std::string(argv[1]) == "-c") {
            filePath = argv[2];
        } else {
            throw std::runtime_error("Invalid flag: " + std::string(argv[1]));
        }
    }

    return filePath;
}

int main(int argc, char* argv[]) {
    try {
        auto ConfigFilePath = getFilePath(argc, argv);

        // подключаемся к сессионной шине DBus
        auto connection = sdbus::createSessionBusConnection();

        Application app(ConfigFilePath, *connection);

        connection->enterEventLoop();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}