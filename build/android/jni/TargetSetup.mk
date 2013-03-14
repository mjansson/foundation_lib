LOCAL_PATH       := $(FOUNDATION_LOCAL_PATH)/../../..

LOCAL_ARM_NEON   := true
LOCAL_ARM_MODE   := arm

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := -std=gnu99 -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Winline -Wpointer-arith -Wwrite-strings -Wundef -Wstrict-aliasing -Wno-shadow -Wno-redundant-decls -Wno-variadic-macros -Wno-long-long -Wno-format -Wno-unused -Wno-missing-field-initializers -Wno-missing-braces -Wno-unused-parameter -Werror -ftabstop=4 -fstrict-aliasing -fno-math-errno -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math

ifeq ($(NDK_DEBUG),1)
LOCAL_CFLAGS     += -g -DBUILD_DEBUG=1 -O0
TARGET_OBJS      := $(TARGET_OUT)/$(LOCAL_MODULE)-debug
else
ifeq ($(BUILD_DEPLOY),1)
LOCAL_CFLAGS     += -DNDEBUG=1 -DBUILD_DEPLOY=1 -O4
TARGET_OBJS      := $(TARGET_OUT)/$(LOCAL_MODULE)-deploy
else
ifeq ($(BUILD_PROFILE),1)
LOCAL_CFLAGS     += -DNDEBUG=1 -DBUILD_PROFILE=1 -O4
TARGET_OBJS      := $(TARGET_OUT)/$(LOCAL_MODULE)-profile
else
LOCAL_CFLAGS     += -DNDEBUG=1 -DBUILD_RELEASE=1 -O4
TARGET_OBJS      := $(TARGET_OUT)/$(LOCAL_MODULE)-release
endif
endif
endif
