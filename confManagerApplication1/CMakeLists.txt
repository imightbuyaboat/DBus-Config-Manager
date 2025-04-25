add_executable(config-confManagerApplication1
    main.cpp
    application.cpp
)

target_include_directories(config-confManagerApplication1 PRIVATE
    ${PROJECT_SOURCE_DIR}/shared
    /usr/include/jsoncpp
)

target_link_libraries(config-confManagerApplication1
    sdbus-c++
    jsoncpp
)