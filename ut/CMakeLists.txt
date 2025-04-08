project(UT C CXX ASM)

include(../cmake/FetchUnity.cmake)

add_executable(${PROJECT_NAME}
        main.c
        test_yasl_general.c
        test_yasl_serialize.c
        test_yasl_deserialize.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
        yasl::yasl
        unity::framework
)

