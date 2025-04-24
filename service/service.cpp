#include "service.h"

void ApplicationConfigObject::ChangeConfiguration(const std::string& key,
                                                  const sdbus::Variant& value) {
    dict[key] = value;

    // создаем и отправляем сигнал об изменении настроек
    auto signal = object->createSignal("com.system.configurationManager.Application.Configuration",
                                       "configurationChanged");
    signal << dict;
    object->emitSignal(signal);

    SaveConfiguration();
}

std::map<std::string, sdbus::Variant> ApplicationConfigObject::GetConfiguration() const {
    return dict;
}

void ApplicationConfigObject::SaveConfiguration() {
    std::ofstream file(path);
    if (file.is_open()) {
        Json::Value root;

        // формируем json с настройками конфигураций приложения
        for (const auto& [key, value] : dict) {
            try {
                if (value.containsValueOfType<std::string>()) {
                    root[key] = value.get<std::string>();
                } else if (value.containsValueOfType<uint32_t>()) {
                    root[key] = value.get<uint32_t>();
                } else if (value.containsValueOfType<int32_t>()) {
                    root[key] = value.get<int32_t>();
                } else if (value.containsValueOfType<bool>()) {
                    root[key] = value.get<bool>();
                } else {
                    std::cerr << "Unknown type: " << key << std::endl;
                }
            } catch (const std::bad_cast& e) {
                std::cerr << "Failed to extract value for key " << key << ": " << e.what()
                          << std::endl;
            }
        }
        file << root;
    }
}

void ApplicationConfigObject::ReadConfiguration(const std::string filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    Json::Value root;
    file >> root;

    // парсим json и сохраняем настройки в словарь
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& val = root[key];

        std::cout << key << ": " << val << std::endl;

        if (val.isUInt()) {
            dict[key] = sdbus::Variant(static_cast<uint32_t>(val.asUInt()));
        } else if (val.isInt()) {
            dict[key] = sdbus::Variant(static_cast<int32_t>(val.asInt()));
        } else if (val.isString()) {
            dict[key] = sdbus::Variant(val.asString());
        } else if (val.isBool()) {
            dict[key] = sdbus::Variant(val.asBool());
        } else {
            std::cerr << "Unknown type: " << key << std::endl;
        }
    }
}

ApplicationConfigObject::ApplicationConfigObject(std::unique_ptr<sdbus::IObject> obj,
                                                 const std::string& filePath)
    : object(std::move(obj)), path(filePath) {
    // регистрируем метод ChangeConfiguration
    object->registerMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .implementedAs([this](const std::string& key, const sdbus::Variant& value) {
            return this->ChangeConfiguration(key, value);
        });

    // регистрируем метод GetConfiguration
    object->registerMethod("GetConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .implementedAs([this]() { return this->GetConfiguration(); });

    // регистрируем сигнал configurationChanged
    object->registerSignal("configurationChanged")
        .onInterface("com.system.configurationManager.Application.Configuration");

    object->finishRegistration();

    // читаем файл конфигурации приложения
    ReadConfiguration(filePath);
}

void initObjects(std::map<std::string, std::unique_ptr<ApplicationConfigObject>>& appObjects,
                 sdbus::IConnection& conn) {
    try {
        for (const auto& entry : fs::directory_iterator(ConfigFolderPath)) {
            // если текущий файл не является папкой и имеет расширение .json
            if (fs::is_regular_file(entry.status()) && entry.path().extension() == ".json") {
                std::string applicationName = entry.path().stem().string();

                std::cout << "file: " << entry.path() << std::endl;
                std::cout << "application name: " << applicationName << std::endl;

                try {
                    // создаем объект DBus для текущего приложения
                    auto object = sdbus::createObject(
                        conn, "/com/system/configurationManager/Application/" + applicationName);

                    // создаем объект приложения
                    appObjects[entry.path()] =
                        std::make_unique<ApplicationConfigObject>(std::move(object), entry.path());

                } catch (const std::exception& e) {
                    std::cerr << "Error creating object " << applicationName << ": " << e.what()
                              << std::endl;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}