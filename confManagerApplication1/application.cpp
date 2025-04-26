#include "application.h"

void Application::Loop() {
    // бесконечный цикл
    while (true) {
        // выводим фразу и определяем время сна
        configMutex.lock();
        uint32_t timeout = Timeout;
        std::cout << TimeoutPhrase << std::endl;
        configMutex.unlock();

        // засыпаем на timeout секунд
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}

Application::Application(const std::string& filePath, sdbus::IConnection& conn) : path(filePath) {
    ReadConfigFromFile();

    // создаем поток с бесконечным циклом
    loopThread = std::thread(&Application::Loop, this);

    const char* serviceName = "com.system.configurationManager";
    const char* objectPath = "/com/system/configurationManager/Application/confManagerApplication1";
    const char* signalName = "configurationChanged";
    const char* interfaceName = "com.system.configurationManager.Application.Configuration";

    // создаем прокси объекта приложения
    proxy =
        sdbus::createProxy(conn, sdbus::ServiceName(serviceName), sdbus::ObjectPath(objectPath));
    proxy->uponSignal(sdbus::SignalName(signalName))
        .onInterface(sdbus::InterfaceName(interfaceName))
        .call([this]() { this->ReadConfigFromFile(); });
}

Application::~Application() {
    if (loopThread.joinable()) {
        loopThread.join();
    }
}

void Application::ReadConfigFromFile() {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    Json::Value root;
    file >> root;

    uint32_t timeout = 0;
    std::string timeoutPhrase = "";

    // парсим json и сохраняем настройки конфигурации
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& val = root[key];

        if (key == "Timeout") {
            timeout = static_cast<uint32_t>(val.asUInt());
        } else if (key == "TimeoutPhrase") {
            timeoutPhrase = val.asString();
        }
    }

    // проверяем правильные ли данные из файла конфигурации
    if (timeout == 0 || timeoutPhrase == "") {
        throw std::runtime_error("Invalid configuration in file: " + path);
    }

    configMutex.lock();
    this->Timeout = timeout;
    this->TimeoutPhrase = timeoutPhrase;
    configMutex.unlock();
}