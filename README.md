# DBus-Config-Manager

В данном репозитории представлен DBus сервис с именем `com.system.configurationManager` и приложение `confManagerApplication1`, написанные на языке C++ в рамках тестового задания к летней практике.

## Описание

1. Реализован DBus сервис с именем `com.system.configurationManager` на сессионной шине, который:
   - при запуске считывает файлы конфигураций приложений из папки `~/com.system.configurationManager/` и     
     создает для каждого файла конфигурации D-Bus объект
     `com.system.configurationManager.Application.{applicationName}` с интерфейсом   
     `com.system.configurationManager.Application.Configuration`;
     
   - предоставляет интерфейс `com.system.configurationManager.Application.Configuration` с методами:
     
     - `void ChangeConfiguration(key: string, value: variant)`,
       который изменяет определенный параметр для приложения и возвращает D-Bus ошибку в случае ошибки;
       
     - `map<string, variant> GetConfiguration()`, который возвращает полную конфигурацию приложения.

   - предоставляет интерфейс `com.system.configurationManager.Application.Configuration` с сигналом:

     - `configurationChanged(configuration: dict)`, где dict является D-Bus типом a{sv}.

2. Реализовать приложение `confManagerApplication1`, которое:

   - подписывается на сигнал `com.system.configurationManager.Application.configurationChanged` у
     объекта `com.system.configurationManager.Application.confManagerApplication1` на сервисе
     `com.system.configurationManager` и применяет новые параметры в случае их изменения

   - имеет управляемую конфигурацию
     `~/com.system.configurationManager/confManagerApplication1.json`, в которой есть два параметра:

     1) `"Timeout"`: uint - время (мс), через которое в stdout будет выведена какая-то строка, определяемая 
         вторым параметром;

     2) `"TimeoutPhrase"`: string - строка, которая будет выведена через "Timeout" мс.

## Требования

- `CMake` версии 3.10 и выше;
- Компилятор, поддерживающий стандарт `С++17`;
- Библиотека `sdbus-c++` v2.1.0;
- Библиотека `jsoncpp` v1.9.5;
- Библиотека `pkg-config`.

## Сборка

1. Клонируйте репозиторий:

   ```bash
   git clone https://github.com/imightbuyaboat/DBus-Config-Manager
   cd DBus-Config-Manager
   ```

2. Создайте директорию для сборки:

   ```bash
   mkdir build
   cd build
   ```

3. Запустите CMake для конфигурации сборки и постройте проект:

   ```bash
   cmake ..
   make
   ```

## Использование

После успешной сборки вы можете запустить:

1. Сервис `com.system.configurationManager`:

   1) запуск без параметров (в таком случае конфигурационные файлы будут браться из папки       
   `~/com.system.configurationManager/`, расположенной в домашней директории)
   
      ```bash
      ./service/configurationManager
      ```
      
   2) запуск с параметром
   
      ```bash
      ./service/configurationManager -d ~/com.system.configurationManager/
      ```

      Где:
         - `-d` - путь к директории с конфигурационными файлами.

   При успешном запуске в консоль будут выведены имена файлов конфигураций и их параметры.
     
2. Приложение `confManagerApplication1`:

   1) запуск без параметров (в таком случае файлом конфигурации приложения будет файл    
   `~/com.system.configurationManager/confManagerApplication1.json`)
   
      ```bash
      ./confManagerApplication1/confManagerApplication1
      ```
      
   2) запуск с параметром
   
      ```bash
      ./confManagerApplication1/confManagerApplication1 -c ~/com.system.configurationManager/confManagerApplication1.json
      ```

      Где:
         - `-c` - путь к конфигурационному файлу приложения.

   При успешном запуске в консоль начнет выводится значение параметра TimeoutPhrase раз в Timeout мс.
     
## Пример использования

1. Подготовьте папку с файлами конфигураций:

   ```bash
   cd ~
   mkdir com.system.configurationManager
   cd com.system.configurationManager/
   ```

2. Создайте файл `confManagerApplication1.json` вручную или с использованием команды:

   ```bash
   echo '{"Timeout": 2000, "TimeoutPhrase": "timeout"}' > confManagerApplication1.json
   ```

3. Перейдите в папку с проектом и запустите сервис `com.system.configurationManager`

   ```bash
   cd ~/DBus-Config-Manager/build/
   ./service/configurationManager
   ```

4. В терминале 2 запустите приложение `confManagerApplication1`:

   ```bash
   cd ~/DBus-Config-Manager/build/
   ./confManagerApplication1/confManagerApplication1
   ```

5. В терминале 3 выполните команду:

   ```bash
   gdbus call --session --dest com.system.configurationManager --object-path /com/system/configurationManager/Application/confManagerApplication1 --method com.system.configurationManager.Application.Configuration.ChangeConfiguration "TimeoutPhrase" "<'Please stop me'>"
   ```

   После выполнения команды в терминале 2 должна начать выводится фраза "Please stop me" раз в Timeout мс.
