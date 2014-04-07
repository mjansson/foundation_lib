LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_MODULE     := test-$(FOUNDATION_TEST_MODULE)

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

#LOCAL_STATIC_LIBRARIES += test foundation android_native_app_glue cpufeatures

#LOCAL_LDLIBS     += -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lOpenSLES

LOCAL_SRC_FILES  := test/$(FOUNDATION_TEST_MODULE)/main.c

include $(BUILD_STATIC_LIBRARY)

