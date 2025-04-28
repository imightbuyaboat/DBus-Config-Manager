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

    /**
     * @brief Изменяем настройки конфигурации приложения.
     * @param key Имя параметра.
     * @param value Значение параметра.
     */
    void ChangeConfiguration(const std::string& key, const sdbus::Variant& value);

    /**
     * @brief Возвращает полную конфигурацию приложения.
     * @return Полная конфигурация приложения.
     */
    std::map<std::string, sdbus::Variant> GetConfiguration() const;

    /**
     * @brief Сохраняет измененную конфигурацию приложения в файл конфигурации приложения.
     */
    void SaveConfiguration();

    /**
     * @brief Считывает конфигурацию приложения из файла конфигурации.
     */
    void ReadConfiguration();

   public:
    /**
     * @brief Создает объект ApplicationConfigObject.
     * @param obj Указатель на объект sdbus::IObject.
     * @param filePath Путь к файлу конфигурации приложения.
     */
    ApplicationConfigObject(std::unique_ptr<sdbus::IObject> obj, const std::string& filePath);

    ~ApplicationConfigObject(){};
};

/**
 * @brief Инициализирует и регистрирует приложения в сервисе DBus.
 * @param appObjects Карта с парами <имя приложения, объект ApplicationConfigObject>.
 * @param conn Соединение с сессионной шинной DBus.
 * @param folderPath Папка конфигураций приложений.
 */
void initObjects(std::map<std::string, std::unique_ptr<ApplicationConfigObject>>& appObjects,
                 sdbus::IConnection& conn, const std::string& folderPath);