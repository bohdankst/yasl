project (Yasl)

add_library(${PROJECT_NAME} STATIC
        yasl.c
        crc/crc8.c
        crc/crc16.c
)
add_library(yasl::yasl ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)
