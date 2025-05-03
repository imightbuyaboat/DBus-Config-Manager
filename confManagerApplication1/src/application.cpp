#include "application.h"

void Application::Loop() {
    // бесконечный цикл
    while (true) {
        uint32_t timeout;

        // выводим фразу и определяем время сна
        {
            std::lock_guard<std::mutex> lock(configMutex);
            timeout = Timeout;
            std::cout << TimeoutPhrase << std::endl;
        }

        // засыпаем на timeout миллисекунд
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}

void Application::configurationChangedSignalHandler(std::map<std::string, sdbus::Variant>& dict) {
    uint32_t newTimeout = 0;
    std::string newTimeoutPhrase = "";

    // определяем новые значения параметров
    for (const auto& [key, value] : dict) {
        if (key == "Timeout") {
            newTimeout = value.get<uint32_t>();
        } else if (key == "TimeoutPhrase") {
            newTimeoutPhrase = value.get<std::string>();
        }
    }

    // проверяем правильные ли данные переданы в теле сигнала
    if (newTimeout == 0 || newTimeoutPhrase == "") {
        throw std::runtime_error("Invalid configuration in body of signal: " + path);
    }

    std::lock_guard<std::mutex> lock(configMutex);
    this->Timeout = newTimeout;
    this->TimeoutPhrase = newTimeoutPhrase;
}

Application::Application(const std::string& filePath) : path(filePath) {
    connection = sdbus::createSessionBusConnection();

    ReadConfigFromFile();

    // создаем поток с бесконечным циклом
    loopThread = std::thread(&Application::Loop, this);

    // создаем прокси объекта приложения
    proxy = sdbus::createProxy(*connection, serviceName, objectPath);

    // подписываемся на сигнал configurationChanged
    proxy->uponSignal(signalName)
        .onInterface(interfaceName)
        .call([this](std::map<std::string, sdbus::Variant> dict) {
            try {
                this->configurationChangedSignalHandler(dict);
            } catch (const std::exception& e) {
                std::cerr << "Error while handling configurationChanged signal: " << e.what()
                          << std::endl;
                exit(1);
            }
        });
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

    uint32_t newTimeout = 0;
    std::string newTimeoutPhrase = "";

    // парсим json и сохраняем настройки конфигурации
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& val = root[key];

        if (key == "Timeout") {
            newTimeout = static_cast<uint32_t>(val.asUInt());
        } else if (key == "TimeoutPhrase") {
            newTimeoutPhrase = val.asString();
        }
    }

    // проверяем правильные ли данные из файла конфигурации
    if (newTimeout == 0 || newTimeoutPhrase == "") {
        throw std::runtime_error("Invalid configuration in file: " + path);
    }

    std::lock_guard<std::mutex> lock(configMutex);
    this->Timeout = newTimeout;
    this->TimeoutPhrase = newTimeoutPhrase;
}

void Application::StartEventLoop() {
    connection->enterEventLoop();
}