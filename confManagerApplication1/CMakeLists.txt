add_executable(confManagerApplication1
    main.cpp
    application.cpp
)

target_include_directories(confManagerApplication1 PRIVATE
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