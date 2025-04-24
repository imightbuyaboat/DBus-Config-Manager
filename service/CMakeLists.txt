add_executable(config-service
    main.cpp
    service.cpp
)

target_include_directories(config-service PRIVATE
    ${PROJECT_SOURCE_DIR}/shared
    /usr/include/jsoncpp
)

target_link_libraries(config-service
    sdbus-c++
    jsoncpp
)