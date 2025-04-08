
#include "unity_fixture.h"
#include <stdio.h>
#include <string.h>
#include <yasl/yasl.h>
#include <yasl/yasl.h>
#include <yasl_internal.h>
#include <crc/checksum.h>

TEST_GROUP(YaslSerializeTestGroup);

TEST_SETUP(YaslSerializeTestGroup) {}

TEST_TEAR_DOWN(YaslSerializeTestGroup) {}

TEST(YaslSerializeTestGroup, test_serializeEmpty) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 20;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, maxSrPktSize, srPktBuf);

    // serialize empty buffer
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, 0)];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, NULL, 0));

    // check output size
    TEST_ASSERT_EQUAL_UINT32(outBufSize, yasl_getSerializedPktSize(&ctx, 0));

    // check payload
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_1, outBuf[0]);
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_2, outBuf[1]);
    TEST_ASSERT_EQUAL_UINT8(5, outBuf[2]); // ID
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[3]); // counter
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[4]); // size
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[5]); // size
    TEST_ASSERT_EQUAL_UINT8(crc_8(outBuf, 6), outBuf[6]); // crc

    // repeat the same, only counter should be increased
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, NULL, 0));

    // check output size
    TEST_ASSERT_EQUAL_UINT32(outBufSize, yasl_getSerializedPktSize(&ctx, 0));

    // check payload
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_1, outBuf[0]);
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_2, outBuf[1]);
    TEST_ASSERT_EQUAL_UINT8(5, outBuf[2]); // ID
    TEST_ASSERT_EQUAL_UINT8(1, outBuf[3]); // counter
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[4]); // size
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[5]); // size
    TEST_ASSERT_EQUAL_UINT8(crc_8(outBuf, 6), outBuf[6]); // crc
}

TEST(YaslSerializeTestGroup, test_serializeSmall) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 20;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, maxSrPktSize, srPktBuf);

    // serialize empty buffer
    uint8_t inBuff[2] = {20, 30};
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, 2)];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    // check output size
    TEST_ASSERT_EQUAL_UINT32(outBufSize, yasl_getSerializedPktSize(&ctx, sizeof(inBuff)));

    // check payload
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_1, outBuf[0]);
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_2, outBuf[1]);
    TEST_ASSERT_EQUAL_UINT8(5, outBuf[2]); // ID
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[3]); // counter
    TEST_ASSERT_EQUAL_UINT16(2, *(uint16_t *)&outBuf[4]); // size
    TEST_ASSERT_EQUAL_UINT8(crc_8(outBuf, 6), outBuf[6]); // crc

    // data
    TEST_ASSERT_EQUAL_UINT8(inBuff[0], outBuf[7]);
    TEST_ASSERT_EQUAL_UINT8(inBuff[1], outBuf[8]);

    TEST_ASSERT_EQUAL_UINT8(crc_16(inBuff, sizeof(inBuff)), *(uint16_t *)&outBuf[9]); // crc

    // repeat the same, only counter should be increased
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    // check output size
    TEST_ASSERT_EQUAL_UINT32(outBufSize, yasl_getSerializedPktSize(&ctx, sizeof(inBuff)));

    // check payload
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_1, outBuf[0]);
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_2, outBuf[1]);
    TEST_ASSERT_EQUAL_UINT8(5, outBuf[2]); // ID
    TEST_ASSERT_EQUAL_UINT8(1, outBuf[3]); // counter
    TEST_ASSERT_EQUAL_UINT16(2, *(uint16_t *)&outBuf[4]); // size
    TEST_ASSERT_EQUAL_UINT8(crc_8(outBuf, 6), outBuf[6]); // crc

    // data
    TEST_ASSERT_EQUAL_UINT8(inBuff[0], outBuf[7]);
    TEST_ASSERT_EQUAL_UINT8(inBuff[1], outBuf[8]);

    TEST_ASSERT_EQUAL_UINT8(crc_16(inBuff, sizeof(inBuff)), *(uint16_t *)&outBuf[9]); // crc
}

TEST(YaslSerializeTestGroup, test_serializeBig) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 1024;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, maxSrPktSize, srPktBuf);

    // serialize empty buffer
    uint8_t inBuff[512] = {0};
    inBuff[0] = 197;
    inBuff[511] = 73;
    uint8_t outBuf[yasl_getSerializedPktSize(&ctx, sizeof(inBuff))];
    size_t outBufSize;
    TEST_ASSERT_EQUAL_UINT32(eYasl_ok, yasl_serialize(&ctx, outBuf, &outBufSize, sizeof(outBuf), 5, inBuff, sizeof(inBuff)));

    // check output size
    TEST_ASSERT_EQUAL_UINT32(outBufSize, yasl_getSerializedPktSize(&ctx, sizeof(inBuff)));

    // check payload
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_1, outBuf[0]);
    TEST_ASSERT_EQUAL_UINT8(START_BYTE_2, outBuf[1]);
    TEST_ASSERT_EQUAL_UINT8(5, outBuf[2]); // ID
    TEST_ASSERT_EQUAL_UINT8(0, outBuf[3]); // counter
    TEST_ASSERT_EQUAL_UINT16(512, *(uint16_t *)&outBuf[4]); // size
    TEST_ASSERT_EQUAL_UINT8(crc_8(outBuf, 6), outBuf[6]); // crc

    // data
    TEST_ASSERT_EQUAL_MEMORY(&inBuff[0], &outBuf[7], sizeof(inBuff));
    TEST_ASSERT_EQUAL_UINT8(crc_16(inBuff, sizeof(inBuff)), *(uint16_t *)&outBuf[7 + 512]); // crc
}

TEST_GROUP_RUNNER(YaslSerializeTestGroup) {
    RUN_TEST_CASE(YaslSerializeTestGroup, test_serializeEmpty);
    RUN_TEST_CASE(YaslSerializeTestGroup, test_serializeSmall);
    RUN_TEST_CASE(YaslSerializeTestGroup, test_serializeBig);
}