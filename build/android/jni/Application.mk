APP_PROJECT_PATH := $(call my-dir)/../../..
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/build/android/jni/Android.mk
APP_MODULES      := test-all test-app test-array test-atomic test-base64 test-blowfish test-bufferstream test-config test-crash test-environment test-error test-event test-fs test-hash test-hashmap test-hashtable test-library test-math test-md5 test-mutex test-objectmap test-path test-radixsort test-random test-ringbuffer test-semaphore test-stacktrace test-string

#NDK_TOOLCHAIN_VERSION=clang3.1

APP_ABI       := armeabi-v7a
APP_PLATFORM  := android-10
APP_STL       := gnustl_static

LOCAL_SHARED_LIBRARIES := test-all test-app test-array test-atomic test-base64 test-blowfish test-bufferstream test-config test-crash test-environment test-error test-event test-fs test-hash test-hashmap test-hashtable test-library test-math test-md5 test-mutex test-objectmap test-path test-radixsort test-random test-ringbuffer test-semaphore test-stacktrace test-string
