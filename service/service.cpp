#include "service.h"

void ApplicationConfigObject::ChangeConfiguration(const std::string& key,
                                                  const sdbus::Variant& value) {
    // проверяем существует ли в словаре ключ key
    auto it = dict.find(key);
    if (it == dict.end()) {
        throw sdbus::Error(sdbus::Error::Name("com.system.configurationManager.Error"),
                           "Incorrect key: " + key);
    }

    // проверяем совпадают ли типы старого и нового значений параметра key
    const sdbus::Variant& oldValue = it->second;
    std::string oldType = oldValue.peekValueType();
    std::string newType = value.peekValueType();

    if (oldType != newType) {
        throw sdbus::Error(sdbus::Error::Name("com.system.configurationManager.Error"),
                           "Incorrect value for the key: " + key);
    }

    dict[key] = value;

    // сохраняем измененную конфигурацию в файл
    try {
        SaveConfiguration();
    } catch (const std::exception& e) {
        throw sdbus::Error(sdbus::Error::Name("com.system.configurationManager.Error"),
                           "Failed to save configuration: " + std::string(e.what()));
    }

    // создаем и отправляем сигнал об изменении настроек
    const char* interfaceName = "com.system.configurationManager.Application.Configuration";
    const char* signalName = "configurationChanged";
    auto signal =
        object->createSignal(sdbus::InterfaceName(interfaceName), sdbus::SignalName(signalName));
    signal << dict;
    object->emitSignal(signal);
}

std::map<std::string, sdbus::Variant> ApplicationConfigObject::GetConfiguration() const {
    return dict;
}

void ApplicationConfigObject::SaveConfiguration() {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

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
            std::cerr << "Failed to extract value for key " << key << ": " << e.what() << std::endl;
        }
    }

    file << root;
    if (!file) {
        throw std::runtime_error("Failed to write configuration in file: " + path);
    }
}

void ApplicationConfigObject::ReadConfiguration(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    Json::Value root;
    file >> root;

    // парсим json и сохраняем настройки в словарь
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& val = root[key];

        std::cout << "\t" << key << ": " << val << std::endl;

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
    const char* interfaceName = "com.system.configurationManager.Application.Configuration";

    // регистрируем метод ChangeConfiguration
    const char* methodName = "ChangeConfiguration";
    object
        ->addVTable(sdbus::registerMethod(sdbus::MethodName(methodName))
                        .implementedAs([this](const std::string& key, const sdbus::Variant& value) {
                            return this->ChangeConfiguration(key, value);
                        }))
        .forInterface(sdbus::InterfaceName(interfaceName));

    // регистрируем метод GetConfiguration
    methodName = "GetConfiguration";
    object
        ->addVTable(sdbus::registerMethod(sdbus::MethodName(methodName)).implementedAs([this]() {
            return this->GetConfiguration();
        }))
        .forInterface(sdbus::InterfaceName(interfaceName));

    // регистрируем сигнал configurationChanged
    const char* signalName = "configurationChanged";
    object->addVTable(sdbus::registerSignal(sdbus::SignalName(signalName)))
        .forInterface(sdbus::InterfaceName(interfaceName));

    // читаем файл конфигурации приложения
    ReadConfiguration(filePath);
}

void initObjects(std::map<std::string, std::unique_ptr<ApplicationConfigObject>>& appObjects,
                 sdbus::IConnection& conn, const std::string& folderPath) {
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            // если текущий файл не является папкой и имеет расширение .json
            if (fs::is_regular_file(entry.status()) && entry.path().extension() == ".json") {
                std::string applicationName = entry.path().stem().string();

                std::cout << "file: " << entry.path() << std::endl;
                std::cout << "application name: " << applicationName << std::endl;

                try {
                    // создаем объект DBus для текущего приложения
                    std::string objectPath =
                        "/com/system/configurationManager/Application/" + applicationName;
                    auto object = sdbus::createObject(conn, sdbus::ObjectPath(objectPath));

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
        exit(1);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}