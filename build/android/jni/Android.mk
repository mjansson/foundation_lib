FOUNDATION_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

include $(CLEAR_VARS)

# Make static libraries installable
$(call module-class-register-installable,STATIC_LIBRARY,lib,$(TARGET_LIB_EXTENSION))


#Build foundation library
include $(CLEAR_VARS)

LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

TARGET_PLATFORM  := android-10

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/debug/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/deploy/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/profile/$(TARGET_ARCH_ABI)
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/release/$(TARGET_ARCH_ABI)
endif
endif
endif

LOCAL_MODULE     := foundation

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_SRC_FILES  := \
	foundation/android.c foundation/array.c foundation/assert.c foundation/assetstream.c foundation/atomic.c \
	foundation/base64.c foundation/bitbuffer.c foundation/blowfish.c foundation/bufferstream.c foundation/config.c \
	foundation/crash.c foundation/environment.c foundation/error.c foundation/event.c foundation/foundation.c \
	foundation/fs.c foundation/hash.c foundation/hashmap.c foundation/hashtable.c foundation/library.c foundation/log.c \
	foundation/main.c foundation/md5.c foundation/memory.c foundation/mutex.c foundation/objectmap.c foundation/path.c \
	foundation/pipe.c foundation/process.c foundation/profile.c foundation/radixsort.c foundation/random.c \
	foundation/regex.c foundation/ringbuffer.c foundation/semaphore.c foundation/stacktrace.c foundation/stream.c \
	foundation/string.c foundation/system.c foundation/thread.c foundation/time.c foundation/uuid.c

LOCAL_STATIC_LIBRARIES := android_native_app_glue cpufeatures

include $(BUILD_STATIC_LIBRARY)


#Build test library
include $(CLEAR_VARS)

LOCAL_PATH := $(FOUNDATION_LOCAL_PATH)/../../..

TARGET_PLATFORM  := android-10

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/debug/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/deploy/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/profile/$(TARGET_ARCH_ABI)
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/release/$(TARGET_ARCH_ABI)
endif
endif
endif

LOCAL_MODULE     := test

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_SRC_FILES  := test/test/test.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/test

include $(BUILD_STATIC_LIBRARY)


#Build test modules
include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := app
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := array
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := atomic
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := base64
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := bitbuffer
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := blowfish
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := bufferstream
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := config
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := crash
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := environment
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := error
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := event
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := fs
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := hash
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := hashmap
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := hashtable
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := library
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := math
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := md5
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := mutex
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := objectmap
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := path
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := pipe
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := profile
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := radixsort
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := random
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := regex
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := ringbuffer
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := semaphore
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := stacktrace
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := string
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk

include $(CLEAR_VARS)
FOUNDATION_TEST_MODULE := uuid
include $(FOUNDATION_LOCAL_PATH)/TestModule.mk


#Build main test launcher
include $(CLEAR_VARS)

LOCAL_MODULE     := test-all

include $(FOUNDATION_LOCAL_PATH)/TargetSetup.mk

LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_C_INCLUDES += $(LOCAL_PATH)/test

ifeq ($(NDK_DEBUG),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/debug/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_DEPLOY),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/deploy/$(TARGET_ARCH_ABI)
else
ifeq ($(BUILD_PROFILE),1)
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/profile/$(TARGET_ARCH_ABI)
else
NDK_APP_DST_DIR  := $(LOCAL_PATH)/lib/android/release/$(TARGET_ARCH_ABI)
endif
endif
endif

LOCAL_STATIC_LIBRARIES += test-app test-atomic test-array test-base64 test-blowfish test-bitbuffer test-bufferstream test-config test-crash test-environment test-error test-event test-fs test-hash test-hashmap test-hashtable test-library test-math test-md5 test-mutex test-objectmap test-path test-pipe test-profile test-radixsort test-random test-regex test-ringbuffer test-semaphore test-stacktrace test-string test-uuid test foundation android_native_app_glue cpufeatures

LOCAL_LDLIBS     += -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lOpenSLES

LOCAL_SRC_FILES  := test/all/main.c

include $(BUILD_SHARED_LIBRARY)



$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
