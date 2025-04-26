add_executable(confManagerApplication1
    main.cpp
    application.cpp
)

target_include_directories(confManagerApplication1 PRIVATE
    ${PROJECT_SOURCE_DIR}/shared
    /usr/include/jsoncpp
)

target_link_libraries(confManagerApplication1
    sdbus-c++
    jsoncpp
)