FOUNDATION_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

include $(CLEAR_VARS)

# Make static libraries installable
$(call module-class-register-installable,STATIC_LIBRARY,lib,.a)


#Build foundation library
include $(CLEAR_VARS)

LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

TARGET_ARCH_ABI  := armeabi-v7a
TARGET_PLATFORM  := android-10

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/debug
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/rtm
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/profile
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/release
endif
endif
endif

LOCAL_MODULE     := foundation

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_SRC_FILES  := \
	foundation/android.c foundation/array.c foundation/assert.c foundation/assetstream.c foundation/base64.c foundation/blowfish.c \
	foundation/bufferstream.c foundation/config.c foundation/crash.c foundation/environment.c foundation/error.c foundation/event.c \
	foundation/foundation.c foundation/fs.c foundation/hash.c foundation/library.c foundation/log.c foundation/main.c \
	foundation/md5.c foundation/memory.c foundation/mutex.c foundation/objectmap.c foundation/path.c foundation/process.c \
	foundation/profile.c foundation/radixsort.c foundation/random.c foundation/ringbuffer.c foundation/semaphore.c \
	foundation/stream.c foundation/string.c foundation/system.c foundation/thread.c foundation/time.c

LOCAL_STATIC_LIBRARIES := android_native_app_glue cpufeatures

include $(BUILD_STATIC_LIBRARY)


#Build test library
include $(CLEAR_VARS)

LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

TARGET_ARCH_ABI  := armeabi-v7a
TARGET_PLATFORM  := android-10

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/debug
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/rtm
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/profile
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/release
endif
endif
endif

LOCAL_MODULE     := test

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_SRC_FILES  := test/test/test.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/test

include $(BUILD_STATIC_LIBRARY)


#Build main test launcher
include $(CLEAR_VARS)

LOCAL_MODULE     := test-all

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_C_INCLUDES += $(LOCAL_PATH)/test
LOCAL_CFLAGS     := 

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/debug
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/rtm
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/profile
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/release
endif
endif
endif

LOCAL_STATIC_LIBRARIES += foundation android_native_app_glue cpufeatures

LOCAL_LDLIBS     += -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lOpenSLES

LOCAL_SRC_FILES  := test/all/main.c

include $(BUILD_SHARED_LIBRARY)


#Build test modules
include $(CLEAR_VARS)

LOCAL_MODULE     := test-array

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_C_INCLUDES += $(LOCAL_PATH)/test
LOCAL_CFLAGS     := 

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/debug
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/rtm
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/profile
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/bin/android/release
endif
endif
endif

LOCAL_STATIC_LIBRARIES += test foundation android_native_app_glue cpufeatures

LOCAL_LDLIBS     += -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lOpenSLES

LOCAL_SRC_FILES  := test/array/main.c

include $(BUILD_SHARED_LIBRARY)


$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
