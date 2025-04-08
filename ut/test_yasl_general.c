
#include "unity_fixture.h"
#include <stdio.h>
#include <string.h>
#include <yasl/yasl.h>

TEST_GROUP(YaslGeneralTestGroup);

TEST_SETUP(YaslGeneralTestGroup) {}

TEST_TEAR_DOWN(YaslGeneralTestGroup) {}

TEST(YaslGeneralTestGroup, test_yaslGeneralSize) {
    yasl_ctx_t ctx;
    size_t maxSrPktSize = 10;
    uint8_t srPktBuf[maxSrPktSize];

    yasl_init(&ctx, maxSrPktSize, srPktBuf);

    // header only, no payload, no data crc
    TEST_ASSERT_EQUAL_UINT32(7, yasl_getSerializedPktSize(&ctx, 0));

    // header + 1b data + 2b CRC
    TEST_ASSERT_EQUAL_UINT32(10, yasl_getSerializedPktSize(&ctx, 1));

    // size is bigger than was during init
    TEST_ASSERT_EQUAL_UINT32(0, yasl_getSerializedPktSize(&ctx, 2));
}

TEST_GROUP_RUNNER(YaslGeneralTestGroup) {
    RUN_TEST_CASE(YaslGeneralTestGroup, test_yaslGeneralSize);
}