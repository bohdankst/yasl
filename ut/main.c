#include "unity_fixture.h"
#include <stdio.h>

static void RunAllTests(void) {
    RUN_TEST_GROUP(YaslGeneralTestGroup);
    RUN_TEST_GROUP(YaslSerializeTestGroup);
    RUN_TEST_GROUP(YaslDeSerializeTestGroup);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, RunAllTests);
}
