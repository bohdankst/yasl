#ifndef YASL_H
#define YASL_H

#include <stdlib.h>
#include <yasl/yasl_types.h>

// <header> <payload> <payload crc16>
// <header>: byte0 | byte1 | id (1b) | pkt cnt 1b | payload length (2b) | crc8
// <payload> <uint16>

typedef enum {
  eYasl_pktAvailable = 1,
  eYasl_ok = 0,
  eYasl_error = -1,
} yasl_rezult_t;

void yasl_init(yasl_ctx_t * ctx, size_t maxSrPktSize, uint8_t *srPktBuf);

size_t yasl_getSerializedPktSize(yasl_ctx_t * ctx, size_t payloadSize);

yasl_rezult_t yasl_serialize(yasl_ctx_t * ctx, uint8_t * outBuf, size_t * outBufSize, size_t outBufMaxSize, uint8_t id, const uint8_t * payload, size_t payloadSize);

yasl_rezult_t yasl_deSerializeReset(yasl_ctx_t * ctx);

yasl_rezult_t yasl_deSerialize(yasl_ctx_t * ctx, uint8_t ** inBuf, size_t * inBufSize);

uint32_t yasl_getAvailablePktSize(const yasl_ctx_t * ctx);

yasl_rezult_t yasl_getAvailablePkt(yasl_ctx_t * ctx, uint8_t * data, size_t * dataSize, uint8_t * id, uint8_t * counter, size_t inBuffMaxSize);

#endif //YASL_H
