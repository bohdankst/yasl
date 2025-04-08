# Yet Another Serialization Library

Simple and lightweight library for serialization and deserialization

## Serialization

```c

#include <yasl/yasl.h>

...

yasl_ctx_t ctx;
size_t maxSerializedPktSize = 512;
uint8_t serializedPktBuf[maxSerializedPktSize];

yasl_init(&ctx, maxSerializedPktSize, serializedPktBuf);

...

uint8_t outBuf[yasl_getSerializedPktSize(&ctx, <payload_size>)];
size_t outBufSize;
yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), <id>, <payload>, <payload size>));

```

## De-serialization

```c

#include <yasl/yasl.h>

...

yasl_ctx_t ctx;
size_t maxSerializedPktSize = 512;
uint8_t serializedPktBuf[maxSerializedPktSize];

yasl_init(&ctx, maxSerializedPktSize, serializedPktBuf);

...

uint8_t inBuf[<any size>];
size_t inBufSize = <current input size>;
uint8_t * inBufPtr = inBuf;

while (NULL != inBufPtr) {
    if (eYasl_pktAvailable == yasl_deSerialize(&ctx, &inBufPtr, &inBufSize)) {
        // get and process packet
        size_t dataOutSize;
        uint8_t dataOut[yasl_getAvailablePktSize(&ctx)];
        uint8_t id;
        uint8_t counter;
        yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
        // data is in dataOut
    }
}

```