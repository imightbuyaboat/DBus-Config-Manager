set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(configurationManager
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/service.cpp
    ${SOURCE_DIR}/applicationConfigObject.cpp
)

target_include_directories(configurationManager PRIVATE
    ${INCLUDE_DIR}
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