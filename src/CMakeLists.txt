project (Yasl)

add_library(${PROJECT_NAME} STATIC
        yasl.c
        crc/crc8.c
        crc/crc16.c
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

set(${PROJECT_NAME}_PUBLIC_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/yasl/yasl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/yasl/yasl_types.h
)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        C_STANDARD          11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS        OFF
        HEADER_DIRS         ${CMAKE_SOURCE_DIR}/include
        EXPORT_NAME         yasl
)