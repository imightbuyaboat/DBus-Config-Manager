#include <sdbus-c++/sdbus-c++.h>

#include <filesystem>
#include <iostream>
#include <list>
#include <string>

#include "applicationConfigObject.h"

namespace fs = std::filesystem;

class Service {
   private:
    std::string configFolderPath; // путь к папке конфигураций
    std::unique_ptr<sdbus::IConnection> connection; // соединение с сессионной шиной DBus
    std::list<std::unique_ptr<ApplicationConfigObject>>
        appObjects; // список объектов приложений DBus

    sdbus::ServiceName serviceName{"com.system.configurationManager"};

    /**
     * @brief Инициализирует и регистрирует приложения в сервисе DBus.
     */
    void initObjects();

   public:
    /**
     * @brief Создает объект Service.
     * @param folderPath Путь к папке конфигураций.
     */
    Service(const std::string& folderPath);
};