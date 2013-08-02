LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_MODULE     := test-$(FOUNDATION_TEST_MODULE)

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

LOCAL_SRC_FILES  := test/$(FOUNDATION_TEST_MODULE)/main.c

include $(BUILD_EXECUTABLE)

