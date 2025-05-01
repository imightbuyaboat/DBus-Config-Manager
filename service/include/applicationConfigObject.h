#include <json/json.h>
#include <sdbus-c++/sdbus-c++.h>

#include <fstream>
#include <map>
#include <string>

// Объект приложения DBus
class ApplicationConfigObject {
   private:
    std::unique_ptr<sdbus::IObject> object;     // объект DBus
    std::map<std::string, sdbus::Variant> dict; // словарь
    std::string path; // путь к файлу конфигураций приложения

    sdbus::InterfaceName interfaceName{"com.system.configurationManager.Application.Configuration"};
    sdbus::MethodName changeMethodName{"ChangeConfiguration"};
    sdbus::MethodName getMethodName{"GetConfiguration"};
    sdbus::SignalName signalName{"configurationChanged"};

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
     * @param conncetion Подключение к сессионной шине DBus
     * @param filePath Путь к файлу конфигурации приложения.
     * @param objectPath Путь к объекту DBus.
     */
    ApplicationConfigObject(sdbus::IConnection& connection, const std::string& filePath,
                            sdbus::ObjectPath objectPath);

    ~ApplicationConfigObject(){};
};