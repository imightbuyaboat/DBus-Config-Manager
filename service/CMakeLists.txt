add_executable(configurationManager
    main.cpp
    service.cpp
)

target_include_directories(configurationManager PRIVATE
    ${PROJECT_SOURCE_DIR}/shared
    /usr/include/jsoncpp
)

target_link_libraries(configurationManager
    sdbus-c++
    jsoncpp
)