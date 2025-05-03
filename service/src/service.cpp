#include "service.h"

Service::Service(const std::string& folderPath) : configFolderPath(folderPath) {
    connection = sdbus::createSessionBusConnection();

    connection->requestName(serviceName);

    initObjects();
}

void Service::initObjects() {
    for (const auto& entry : fs::directory_iterator(configFolderPath)) {
        // если текущий файл не является папкой и имеет расширение .json
        if (fs::is_regular_file(entry.status()) && entry.path().extension() == ".json") {
            std::string applicationName = entry.path().stem().string();

            std::string objectPath =
                "/com/system/configurationManager/Application/" + applicationName;

            std::cout << "application: " << objectPath << std::endl;

            // создаем объект DBus для текущего приложения
            auto applicationObject = std::make_unique<ApplicationConfigObject>(
                *connection, entry.path(), sdbus::ObjectPath(objectPath));
            appObjects.push_back(std::move(applicationObject));
        }
    }
}

void Service::StartEventLoop() {
    std::cout << "Starting DBus service" << std::endl;
    connection->enterEventLoop();
}