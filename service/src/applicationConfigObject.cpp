#include "applicationConfigObject.h"

void ApplicationConfigObject::ChangeConfiguration(sdbus::MethodCall call) {
    std::string key;
    sdbus::Variant value;
    call >> key >> value;

    std::thread([this, key = std::move(key), value = std::move(value), call = std::move(call)]() {
        sdbus::Variant oldValue;

        // создаем сигнал
        auto signal = object->createSignal(interfaceName, signalName);

        {
            std::lock_guard<std::mutex> lock(mu);

            // проверяем существует ли в словаре ключ key
            auto it = dict.find(key);
            if (it == dict.end()) {
                auto reply = call.createErrorReply(
                    {sdbus::Error::Name("com.system.configurationManager.Error"),
                     "Incorrect key: " + key});
                reply.send();
                return;
            }

            // проверяем совпадают ли типы старого и нового значений параметра key
            oldValue = it->second;
            std::string oldType = oldValue.peekValueType();
            std::string newType = value.peekValueType();

            if (oldType != newType) {
                auto reply = call.createErrorReply(
                    {sdbus::Error::Name("com.system.configurationManager.Error"),
                     "Incorrect value for the key: " + key});
                reply.send();
                return;
            }

            dict[key] = value;

            // сохраняем измененную конфигурацию в файл
            try {
                SaveConfiguration();
            } catch (const std::exception& e) {
                // возвращаем старое значение параметра в словарь
                dict[key] = oldValue;

                auto reply = call.createErrorReply(
                    {sdbus::Error::Name("com.system.configurationManager.Error"),
                     "Failed to save configuration: " + std::string(e.what())});
                reply.send();
                return;
            }

            signal << dict;
        }

        auto reply = call.createReply();
        reply.send();

        object->emitSignal(signal);
    }).detach();
}

void ApplicationConfigObject::GetConfiguration(sdbus::MethodCall call) {
    std::thread([this, call = std::move(call)]() {
        auto reply = call.createReply();
        {
            std::lock_guard<std::mutex> lock(mu);

            reply << dict;
        }
        reply.send();
    }).detach();
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
    std::lock_guard<std::mutex> lock(mu);

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

    // регистрируем методы и сигнал
    object
        ->addVTable(sdbus::MethodVTableItem{changeMethodName,
                                            sdbus::Signature{"sv"},
                                            {},
                                            sdbus::Signature{""},
                                            {},
                                            [this](sdbus::MethodCall call) {
                                                this->ChangeConfiguration(std::move(call));
                                            },
                                            {}},
                    sdbus::MethodVTableItem{
                        getMethodName,
                        sdbus::Signature{""},
                        {},
                        sdbus::Signature{"a{sv}"},
                        {},
                        [this](sdbus::MethodCall call) { this->GetConfiguration(std::move(call)); },
                        {}},
                    sdbus::SignalVTableItem{signalName, sdbus::Signature{"a{sv}"}, {}, {}})
        .forInterface(interfaceName);

    // читаем файл конфигурации приложения
    ReadConfiguration();
}