#include "unity_fixture.h"
#include <crc/checksum.h>
#include <stdio.h>
#include <string.h>
#include <yasl/yasl.h>
#include <yasl_internal.h>

TEST_GROUP(YaslDeSerializeTestGroup);

TEST_SETUP(YaslDeSerializeTestGroup) {}

TEST_TEAR_DOWN(YaslDeSerializeTestGroup) {}

TEST(YaslDeSerializeTestGroup, test_deSerializeEmpty) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 20;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, 0)];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, NULL, 0));

    uint8_t * inBuf = outBuf;
    size_t inBufSize = outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(0, yasl_getAvailablePktSize(&ctx));

    size_t dataOutSize;
    uint8_t dataOut[1];
    uint8_t id;
    uint8_t counter;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
    TEST_ASSERT_EQUAL_UINT32(0, dataOutSize);
    TEST_ASSERT_EQUAL_UINT8(5, id);
    TEST_ASSERT_EQUAL_UINT8(0, counter);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeSmall) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 20;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[2] = {20, 30};
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, 2)];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 10, inBuff, sizeof(inBuff)));

    uint8_t * inBuf = outBuf;
    size_t inBufSize = outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(2, yasl_getAvailablePktSize(&ctx));

    size_t dataOutSize;
    uint8_t dataOut[yasl_getAvailablePktSize(&ctx)];
    uint8_t id;
    uint8_t counter;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
    TEST_ASSERT_EQUAL_UINT32(2, dataOutSize);
    TEST_ASSERT_EQUAL_UINT8(10, id);
    TEST_ASSERT_EQUAL_UINT8(0, counter);
    TEST_ASSERT_EQUAL_MEMORY(inBuff, dataOut, dataOutSize);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeBig) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 1024;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[512] = {0};
    inBuff[0] = 197;
    inBuff[511] = 73;
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, sizeof(inBuff))];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 100, inBuff, sizeof(inBuff)));

    uint8_t * inBuf = outBuf;
    size_t inBufSize = outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(512, yasl_getAvailablePktSize(&ctx));

    size_t dataOutSize;
    uint8_t dataOut[yasl_getAvailablePktSize(&ctx)];
    uint8_t id;
    uint8_t counter;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
    TEST_ASSERT_EQUAL_UINT32(512, dataOutSize);
    TEST_ASSERT_EQUAL_UINT8(100, id);
    TEST_ASSERT_EQUAL_UINT8(0, counter);
    TEST_ASSERT_EQUAL_MEMORY(inBuff, dataOut, dataOutSize);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeWrongCrc) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 50;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[5] = {10, 20, 30, 40, 50};
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, 5)];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 10, inBuff, sizeof(inBuff)));

    // corrupt header CRC
    outBuf[3] += 1;

    uint8_t * inBuf = outBuf;
    size_t inBufSize = outBufSize;
    // no data
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(0, yasl_getAvailablePktSize(&ctx));

    // fix header CRC
    outBuf[3] -= 1;

    // corrupt data CRC
    outBuf[sizeof(yasl_header_t) + 1] += 1;
    // no data
    inBuf = outBuf;
    inBufSize = outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(0, yasl_getAvailablePktSize(&ctx));

    // fix data CRC
    outBuf[sizeof(yasl_header_t) + 1] -= 1;
    // packet is correct and available
    inBuf = outBuf;
    inBufSize = outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
}

TEST(YaslDeSerializeTestGroup, test_deSerializeFewPackets) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 20;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    size_t bufSize = 3;
    uint8_t inBuff1[3] = {10, 20, 30};
    uint8_t inBuff2[3] = {40, 50, 60};
    uint8_t outBuf[2 + 2 * yasl_getSerializedPktSize(&ctx, bufSize)]; // 2b trash + 2 packets
    size_t outBufSize;
    outBuf[0] = 73;
    outBuf[1] = 91;
    TEST_ASSERT_EQUAL_UINT32(
        eYasl_ok, yasl_serialize(&ctx, &outBuf[2], &outBufSize, sizeof(outBuf), 10, inBuff1, sizeof(inBuff1)));
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, &outBuf[2 + yasl_getSerializedPktSize(&ctx, bufSize)],
                                                      &outBufSize, sizeof(outBuf), 20, inBuff2, sizeof(inBuff2)));

    uint8_t * inBuf = outBuf;
    size_t inBufSize = sizeof(outBuf);
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NOT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(yasl_getSerializedPktSize(&ctx, bufSize), inBufSize);
    TEST_ASSERT_EQUAL_UINT32(bufSize, yasl_getAvailablePktSize(&ctx));

    // get the first packet
    size_t dataOutSize;
    uint8_t dataOut[yasl_getAvailablePktSize(&ctx)];
    uint8_t id;
    uint8_t counter;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
    TEST_ASSERT_EQUAL_UINT32(bufSize, dataOutSize);
    TEST_ASSERT_EQUAL_UINT8(10, id);
    TEST_ASSERT_EQUAL_UINT8(0, counter);
    TEST_ASSERT_EQUAL_MEMORY(inBuff1, dataOut, bufSize);

    // get the second packet
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
    TEST_ASSERT_EQUAL_UINT32(bufSize, yasl_getAvailablePktSize(&ctx));

    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_getAvailablePkt(&ctx, dataOut, &dataOutSize, &id, &counter, sizeof(dataOut)));
    TEST_ASSERT_EQUAL_UINT32(bufSize, dataOutSize);
    TEST_ASSERT_EQUAL_UINT8(20, id);
    TEST_ASSERT_EQUAL_UINT8(1, counter);
    TEST_ASSERT_EQUAL_MEMORY(inBuff2, dataOut, bufSize);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeStream) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 1024;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[127] = {0};
    inBuff[0] = 25;
    inBuff[126] = 35;
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, sizeof(inBuff))];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    uint8_t * inBuf = outBuf;
    size_t inBufSize = 3;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    inBuf = &outBuf[3];
    inBufSize = 5;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    inBuf = &outBuf[3 + 5];
    inBufSize = 20;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    inBuf = &outBuf[3 + 5 + 20];
    inBufSize = sizeof(outBuf) - (3 + 5 + 20);
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeStreamErrHeader) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 1024;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[127] = {0};
    inBuff[0] = 25;
    inBuff[126] = 35;
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, sizeof(inBuff))];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    // only part of the header
    uint8_t * inBuf = outBuf;
    size_t inBufSize = 3;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    // new packet, so both will be lost
    inBuf = outBuf;
    inBufSize = sizeof(outBuf);
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    // this one should be properly parsed
    inBuf = outBuf;
    inBufSize = sizeof(outBuf);
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
}

TEST(YaslDeSerializeTestGroup, test_deSerializeStreamErrData) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 1024;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, srPktBuf, maxSrPktSize);

    // serialize empty buffer
    uint8_t inBuff[127] = {0};
    inBuff[0] = 25;
    inBuff[126] = 35;
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, sizeof(inBuff))];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok,
                             yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    // header and almost all data
    uint8_t * inBuf = outBuf;
    size_t inBufSize = 120;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    // new packet, so both will be lost
    inBuf = outBuf;
    inBufSize = sizeof(outBuf);
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);

    // this one should be properly parsed
    inBuf = outBuf;
    inBufSize = sizeof(outBuf);
    TEST_ASSERT_EQUAL_UINT32(eYasl_pktAvailable, yasl_deSerialize(&ctx, &inBuf, &inBufSize));
    TEST_ASSERT_NULL(inBuf);
    TEST_ASSERT_EQUAL_UINT32(0, inBufSize);
}

TEST_GROUP_RUNNER(YaslDeSerializeTestGroup) {
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeEmpty);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeSmall);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeBig);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeWrongCrc);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeFewPackets);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeStream);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeStreamErrHeader);
    RUN_TEST_CASE(YaslDeSerializeTestGroup, test_deSerializeStreamErrData);
}