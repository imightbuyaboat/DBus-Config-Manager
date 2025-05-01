set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(confManagerApplication1
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/application.cpp
)

target_include_directories(confManagerApplication1 PRIVATE
    ${INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/shared
    ${SDBUS_C++_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS} 
)

target_link_libraries(confManagerApplication1
    ${SDBUS_C++_LIBRARIES}
    ${JSONCPP_LIBRARIES}   
)

target_compile_options(confManagerApplication1 PRIVATE ${SDBUS_C++_CFLAGS})
target_compile_options(confManagerApplication1 PRIVATE ${JSONCPP_CFLAGS})