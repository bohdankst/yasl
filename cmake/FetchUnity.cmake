include(FetchContent)

# Also specify GIT_SHALLOW to avoid cloning branch we don't care about
FetchContent_Declare(
        Unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity
        GIT_TAG        v2.6.1
        GIT_SHALLOW    1
)

set(UNITY_EXTENSION_FIXTURE ON CACHE BOOL "Unity: Build with fixtures" FORCE)

# Download cmocka, and execute its cmakelists.txt
FetchContent_MakeAvailable(Unity)