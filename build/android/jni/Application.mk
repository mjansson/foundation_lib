APP_PROJECT_PATH := $(call my-dir)/../../..
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/build/android/jni/Android.mk
APP_MODULES      := test-all test-array

#NDK_TOOLCHAIN_VERSION=clang3.1

APP_ABI       := armeabi-v7a
APP_PLATFORM  := android-10
APP_STL       := gnustl_static

LOCAL_SHARED_LIBRARIES := test-all test-array
