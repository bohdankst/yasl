#include "crc/checksum.h"
#include "yasl_internal.h"
#include <string.h>
#include <yasl/yasl.h>

typedef enum {
    eFsm_byte1 = 0,
    eFsm_byte2,
    eFsm_waitHeaderCrc,
    eFsm_waitDataCrc,
    eFsm_dataReady,
} fsmState_t;

void yasl_init(yasl_ctx_t * ctx, uint8_t * srPktBuf, size_t maxSrPktSize) {
    if (NULL == ctx || 0 == maxSrPktSize || NULL == srPktBuf) {
        return;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->deSrlz.inMsgBuffer = srPktBuf;
    ctx->deSrlz.inMsgBufferSize = maxSrPktSize;
}

size_t yasl_getSerializedPktSize(yasl_ctx_t * ctx, size_t payloadSize) {
    if (NULL == ctx) {
        return 0;
    }

    size_t size = payloadSize + sizeof(yasl_header_t);

    size += 0 == payloadSize ? 0 : sizeof(yasl_dataCrc_t);

    return size <= ctx->deSrlz.inMsgBufferSize ? size : 0;
}

yasl_rezult_t yasl_serialize(yasl_ctx_t * ctx, uint8_t * outBuf, size_t * outBufSize, size_t outBufMaxSize, uint8_t id,
                             const uint8_t * payload, size_t payloadSize) {
    if (NULL == ctx || NULL == outBufSize || NULL == outBuf ||
        outBufMaxSize < yasl_getSerializedPktSize(ctx, payloadSize) || (NULL == payload && payloadSize > 0)) {
        return eYasl_error;
    }

    yasl_header_t * header = (yasl_header_t *)outBuf;
    header->byte1 = START_BYTE_1;
    header->byte2 = START_BYTE_2;
    header->id = id;
    header->counter = ctx->srlz.counter;
    header->length = 0;

    ctx->srlz.counter += 1;

    *outBufSize = sizeof(yasl_header_t);

    if (payloadSize > 0) {
        header->length = payloadSize;
        memcpy(&outBuf[sizeof(yasl_header_t)], payload, payloadSize);
        *((uint16_t *)&outBuf[sizeof(yasl_header_t) + payloadSize]) = crc_16(payload, payloadSize);
        *outBufSize += payloadSize + sizeof(uint16_t);
    }

    header->crc = crc_8((uint8_t *)header, sizeof(yasl_header_t) - sizeof(header->crc));

    return eYasl_ok;
}

static void yasl_resetFsm(yasl_ctx_t * ctx) {
    ctx->deSrlz.state = eFsm_byte1;
    ctx->deSrlz.currPos = 0;
}

yasl_rezult_t yasl_deSerializeReset(yasl_ctx_t * ctx) {
    if (NULL == ctx) {
        return eYasl_error;
    }

    yasl_resetFsm(ctx);

    return eYasl_ok;
}

static void yasl_increaseFsmInput(uint8_t ** inBuf, size_t * inBufSize, size_t step) {
    *inBuf += step;
    *inBufSize -= step;
    if (0 == *inBufSize) {
        *inBuf = NULL;
    }
}

static void yasl_addDataToInBuffer(yasl_ctx_t * ctx, const uint8_t * buff, size_t size) {
    memcpy(&ctx->deSrlz.inMsgBuffer[ctx->deSrlz.currPos], buff, size);
    ctx->deSrlz.currPos += size;
}

yasl_rezult_t yasl_deSerialize(yasl_ctx_t * ctx, uint8_t ** inBuf, size_t * inBufSize) {
    if (NULL == ctx || NULL == inBufSize || NULL == inBuf) {
        return eYasl_error;
    }

    size_t remainingSize;
    size_t copySize;
    yasl_header_t * header = (yasl_header_t *)ctx->deSrlz.inMsgBuffer;

    while (*inBufSize > 0) {
        switch (ctx->deSrlz.state) {
        case eFsm_byte1:
            if (START_BYTE_1 == **inBuf) {
                yasl_addDataToInBuffer(ctx, *inBuf, 1);
                ctx->deSrlz.state = eFsm_byte2;
            }
            yasl_increaseFsmInput(inBuf, inBufSize, 1);
            break;

        case eFsm_byte2:
            if (START_BYTE_2 == **inBuf) {
                yasl_addDataToInBuffer(ctx, *inBuf, 1);
                ctx->deSrlz.state = eFsm_waitHeaderCrc;
            } else {
                yasl_resetFsm(ctx);
            }
            yasl_increaseFsmInput(inBuf, inBufSize, 1);
            break;

        case eFsm_waitHeaderCrc:
            remainingSize = sizeof(yasl_header_t) - ctx->deSrlz.currPos;
            copySize = remainingSize <= *inBufSize ? remainingSize : *inBufSize;
            yasl_addDataToInBuffer(ctx, *inBuf, copySize);
            yasl_increaseFsmInput(inBuf, inBufSize, copySize);

            if (ctx->deSrlz.currPos != sizeof(yasl_header_t)) {
                break; // not enough data yet
            }

            // we have header CRC now, check it
            uint8_t crc8 = crc_8(ctx->deSrlz.inMsgBuffer, sizeof(yasl_header_t) - sizeof(uint8_t));
            if (ctx->deSrlz.inMsgBuffer[sizeof(yasl_header_t) - sizeof(uint8_t)] == crc8) {
                ctx->deSrlz.state = 0 == header->length ? eFsm_dataReady : eFsm_waitDataCrc;
            } else {
                yasl_resetFsm(ctx);
            }
            break;

        case eFsm_waitDataCrc:
            remainingSize = ctx->deSrlz.currPos - sizeof(yasl_header_t) + header->length + sizeof(yasl_dataCrc_t);
            copySize = remainingSize <= *inBufSize ? remainingSize : *inBufSize;
            yasl_addDataToInBuffer(ctx, *inBuf, copySize);
            yasl_increaseFsmInput(inBuf, inBufSize, copySize);

            if (ctx->deSrlz.currPos != sizeof(yasl_header_t) + header->length + sizeof(yasl_dataCrc_t)) {
                break; // not enough data yet
            }

            // we have data and data CRC now, check it
            uint16_t crc16 = crc_16(&ctx->deSrlz.inMsgBuffer[sizeof(yasl_header_t)], header->length);
            if (*(uint16_t *)&ctx->deSrlz.inMsgBuffer[sizeof(yasl_header_t) + header->length] == crc16) {
                ctx->deSrlz.state = eFsm_dataReady;
            } else {
                yasl_resetFsm(ctx);
            }
            break;

        case eFsm_dataReady:
            // wait here until data reading
            break;

        default:
            ctx->deSrlz.state = eFsm_byte1;
            ctx->deSrlz.currPos = 0;
            break;
        }

        // break parsing if we have whole packet
        if (eFsm_dataReady == ctx->deSrlz.state) {
            break;
        }
    }

    return eFsm_dataReady == ctx->deSrlz.state ? eYasl_pktAvailable : eYasl_ok;
}

uint32_t yasl_getAvailablePktSize(const yasl_ctx_t * ctx) {
    if (NULL == ctx) {
        return 0;
    }

    return eFsm_dataReady == ctx->deSrlz.state ? ((yasl_header_t *)(ctx->deSrlz.inMsgBuffer))->length : 0;
}

yasl_rezult_t yasl_getAvailablePkt(yasl_ctx_t * ctx, uint8_t * data, size_t * dataSize, uint8_t * id, uint8_t * counter,
                                   size_t inBuffMaxSize) {
    if (NULL == ctx || NULL == data || NULL == dataSize || NULL == id) {
        return eYasl_error;
    }

    if (eFsm_dataReady != ctx->deSrlz.state) {
        return eYasl_error;
    }

    yasl_header_t * header = (yasl_header_t *)ctx->deSrlz.inMsgBuffer;
    if (inBuffMaxSize < header->length) {
        return eYasl_error;
    }

    *id = header->id;
    *dataSize = header->length;
    memcpy(data, &ctx->deSrlz.inMsgBuffer[sizeof(*header)], header->length);

    if (NULL != counter) {
        *counter = header->counter;
    }

    yasl_resetFsm(ctx);

    return eYasl_ok;
}
