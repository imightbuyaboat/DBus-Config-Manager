#include <json/json.h>
#include <sdbus-c++/sdbus-c++.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// класс приложения confManagerApplication1
class Application {
   private:
    std::string path; // путь конфигурационного файла
    std::mutex configMutex; // мьютекс для доступа к переменным конфигурации
    std::unique_ptr<sdbus::IProxy> proxy; // прокси

    uint32_t Timeout;
    std::string TimeoutPhrase;

    // функция цикла вывода фразы в консоль
    void Loop();

   public:
    Application(const std::string& filePath, sdbus::IConnection& conn);

    // чтение конфигурации из файла
    void ReadConfigFromFile();
};