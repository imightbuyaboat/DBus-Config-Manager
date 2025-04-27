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
