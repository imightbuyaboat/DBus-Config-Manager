#include <json/json.h>
#include <sdbus-c++/sdbus-c++.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

namespace fs = std::filesystem;

// объект приложения
class ApplicationConfigObject {
   private:
    std::unique_ptr<sdbus::IObject> object;     // объект DBus
    std::map<std::string, sdbus::Variant> dict; // словарь
    std::string path; // путь к файлу конфигураций приложения

    // изменяем настройки конфигурации приложения
    void ChangeConfiguration(const std::string& key, const sdbus::Variant& value);

    // возвращаем настройки приложения
    std::map<std::string, sdbus::Variant> GetConfiguration() const;

    // сохранение измененных настроек в файл конфигурации json
    void SaveConfiguration();

    // чтение настроек из файла конфигурации json
    void ReadConfiguration(const std::string& filePath);

   public:
    ApplicationConfigObject(std::unique_ptr<sdbus::IObject> obj, const std::string& filePath);
    ~ApplicationConfigObject(){};
};

// инициализация и регистрация в DBus приложений
void initObjects(std::map<std::string, std::unique_ptr<ApplicationConfigObject>>& appObjects,
                 sdbus::IConnection& conn, const std::string& folderPath);