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
    std::string path; // путь конфигурационного файла
    std::mutex configMutex; // мьютекс для доступа к переменным конфигурации
    std::unique_ptr<sdbus::IProxy> proxy; // прокси
    std::thread loopThread;               // поток с бексонечным циклом

    uint32_t Timeout;
    std::string TimeoutPhrase;

    // функция цикла вывода фразы в консоль
    void Loop();

    // обработчик сигнала configurationChanged
    void configurationChangedSignalHandler(std::map<std::string, sdbus::Variant>& dict);

   public:
    Application(const std::string& filePath, sdbus::IConnection& conn);
    ~Application();

    // чтение конфигурации из файла
    void ReadConfigFromFile();
};