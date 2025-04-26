#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include "application.h"

int main(int argc, char* argv[]) {
    std::string ConfigFilePath =
        std::getenv("HOME") +
        std::string("/com.system.configurationManager/confManagerApplication1.json");

    if (argc == 3) {
        if (std::string(argv[1]) == "-c") {
            ConfigFilePath = argv[2];
        } else {
            std::cerr << "Invalid flag: " << argv[1] << std::endl;
            exit(1);
        }
    }

    // подключаемся к сессионной шине DBus
    auto connection = sdbus::createSessionBusConnection();

    Application app(ConfigFilePath, *connection);

    connection->enterEventLoop();
}