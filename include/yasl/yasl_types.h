#ifndef YASL_TYPES_H
#define YASL_TYPES_H

#include <stdint.h>

typedef struct __attribute__((__packed__)) {
    uint8_t byte1;
    uint8_t byte2;
    uint8_t id;
    uint8_t counter;
    uint16_t length;
    uint8_t crc;
} yasl_header_t;

typedef uint16_t yasl_dataCrc_t;

typedef struct {
    struct {
        uint8_t * inMsgBuffer;
        size_t inMsgBufferSize;
        size_t currPos;
        // size_t size;
        uint32_t state;
    } deSrlz;
    struct {
        uint8_t counter;
    } srlz;
} yasl_ctx_t;

#endif // YASL_TYPES_H
