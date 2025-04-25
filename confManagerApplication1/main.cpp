#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include "application.h"

int main() {
    auto connection = sdbus::createSessionBusConnection();

    const std::string ConfigFilePath =
        "../com.system.configurationManager/confManagerApplication1.json";
    Application app(ConfigFilePath, *connection);

    connection->enterEventLoop();
}