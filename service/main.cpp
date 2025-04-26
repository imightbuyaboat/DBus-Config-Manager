#include <sdbus-c++/sdbus-c++.h>
#include <cstdlib>
#include <map>
#include <string>
#include "service.h"

int main(int argc, char* argv[]) {
    std::string ConfigFolderPath =
        std::getenv("HOME") + std::string("/com.system.configurationManager/");

    if (argc == 3) {
        if (std::string(argv[1]) == "-d") {
            ConfigFolderPath = argv[2];
        } else {
            std::cerr << "Invalid flag: " << argv[1] << std::endl;
            exit(1);
        }
    }

    // подключаемся к сессионной шине DBus и задаем имя сервиса
    auto connection = sdbus::createSessionBusConnection();

    const char* serviceName = "com.system.configurationManager";
    connection->requestName(sdbus::ServiceName(serviceName));

    // инициализируем объекты приложений из конфигурационныз файлов
    std::map<std::string, std::unique_ptr<ApplicationConfigObject>> appObjects;
    initObjects(appObjects, *connection, ConfigFolderPath);

    connection->enterEventLoop();
}