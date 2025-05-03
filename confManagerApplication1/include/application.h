#include <json/json.h>
#include <sdbus-c++/sdbus-c++.h>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>

// класс приложения confManagerApplication1
class Application {
   private:
    std::unique_ptr<sdbus::IConnection> connection; // соединение с сессионной шиной DBus
    std::string path; // путь конфигурационного файла
    std::mutex configMutex; // мьютекс для доступа к переменным конфигурации
    std::unique_ptr<sdbus::IProxy> proxy; // прокси
    std::thread loopThread;               // поток с бексонечным циклом

    uint32_t Timeout;
    std::string TimeoutPhrase;

    sdbus::ServiceName serviceName{"com.system.configurationManager"};
    sdbus::ObjectPath objectPath{
        "/com/system/configurationManager/Application/confManagerApplication1"};
    sdbus::SignalName signalName{"configurationChanged"};
    sdbus::InterfaceName interfaceName{"com.system.configurationManager.Application.Configuration"};

    /**
     * @brief Выводит в консоль фразу TimeoutPhrase раз в Timeout мс.
     */
    void Loop();

    /**
     * @brief Обрабатывает сигнал configurationChanged.
     * @param dict Измененная конфигурация приложения.
     */
    void configurationChangedSignalHandler(std::map<std::string, sdbus::Variant>& dict);

   public:
    /**
     * @brief Создает объект Application.
     * @param filePath Путь к файлу конфигурации приложения.
     * @param conn Соединение с сессионной шинной DBus.
     */
    Application(const std::string& filePath);

    /**
     * @brief Завершает работу потока с функцией Loop().
     */
    ~Application();

    /**
     * @brief Считывает конфигурацию из файла.
     */
    void ReadConfigFromFile();

    /**
     * @brief Запускает DBus цикл.
     */
    void StartEventLoop();
};