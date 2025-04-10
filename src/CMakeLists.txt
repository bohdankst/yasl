project (Yasl)

add_library(${PROJECT_NAME} STATIC
        yasl.c
        crc/crc8.c
        crc/crc16.c
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../include
)
