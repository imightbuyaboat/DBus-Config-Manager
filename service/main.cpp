#include <sdbus-c++/sdbus-c++.h>
#include <map>
#include <string>
#include "service.h"

int main() {
    // подключаемся к сессионной шине DBus и задаем имя сервиса
    auto connection = sdbus::createSessionBusConnection();
    connection->requestName("com.system.configurationManager");

    // инициализируем объекты приложений из конфигурационныз файлов
    std::map<std::string, std::unique_ptr<ApplicationConfigObject>> appObjects;
    initObjects(appObjects, *connection);

    connection->enterEventLoop();
}