add_executable(configurationManager
    main.cpp
    service.cpp
)

target_include_directories(configurationManager PRIVATE
    ${PROJECT_SOURCE_DIR}/shared
    ${SDBUS_C++_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
)

target_link_libraries(configurationManager
    ${SDBUS_C++_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)

target_compile_options(configurationManager PRIVATE ${SDBUS_C++_CFLAGS})
target_compile_options(configurationManager PRIVATE ${JSONCPP_CFLAGS})