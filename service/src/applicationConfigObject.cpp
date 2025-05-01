#include "applicationConfigObject.h"

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
        // возвращаем старое значение параметра в словарь
        dict[key] = oldValue;

        throw sdbus::Error(sdbus::Error::Name("com.system.configurationManager.Error"),
                           "Failed to save configuration: " + std::string(e.what()));
    }

    // создаем и отправляем сигнал об изменении настроек
    auto signal = object->createSignal(interfaceName, signalName);
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
        if (value.containsValueOfType<std::string>()) {
            root[key] = value.get<std::string>();
        } else if (value.containsValueOfType<uint32_t>()) {
            root[key] = value.get<uint32_t>();
        } else if (value.containsValueOfType<int32_t>()) {
            root[key] = value.get<int32_t>();
        } else if (value.containsValueOfType<bool>()) {
            root[key] = value.get<bool>();
        }
    }

    file << root;
    if (!file) {
        throw std::runtime_error("Failed to write configuration in file: " + path);
    }
}

void ApplicationConfigObject::ReadConfiguration() {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    Json::Value root;
    file >> root;

    // парсим json и сохраняем настройки в словарь
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& val = root[key];

        if (val.isUInt()) {
            dict[key] = sdbus::Variant(static_cast<uint32_t>(val.asUInt()));
        } else if (val.isInt()) {
            dict[key] = sdbus::Variant(static_cast<int32_t>(val.asInt()));
        } else if (val.isString()) {
            dict[key] = sdbus::Variant(val.asString());
        } else if (val.isBool()) {
            dict[key] = sdbus::Variant(val.asBool());
        } else {
            throw std::runtime_error("Unknown type: " + key + " in file: " + path);
        }
    }
}

ApplicationConfigObject::ApplicationConfigObject(sdbus::IConnection& connection,
                                                 const std::string& filePath,
                                                 sdbus::ObjectPath objectPath)
    : path(filePath) {
    object = sdbus::createObject(connection, objectPath);

    // регистрируем метод ChangeConfiguration
    object
        ->addVTable(sdbus::registerMethod(changeMethodName)
                        .implementedAs([this](const std::string& key, const sdbus::Variant& value) {
                            return this->ChangeConfiguration(key, value);
                        }))
        .forInterface(interfaceName);

    // регистрируем метод GetConfiguration
    object
        ->addVTable(sdbus::registerMethod(getMethodName).implementedAs([this]() {
            return this->GetConfiguration();
        }))
        .forInterface(interfaceName);

    // регистрируем сигнал configurationChanged
    object->addVTable(sdbus::registerSignal(signalName)).forInterface(interfaceName);

    // читаем файл конфигурации приложения
    ReadConfiguration();
}