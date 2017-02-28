LOCAL_PATH:= $(call my-dir)

include $(call all-subdir-makefiles)
include $(CLEAR_VARS)
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
#######################################################
LOCAL_MODULE := nanoconsole
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
include $(BUILD_EXECUTABLE)

#######################################################

include $(call all-makefiles-under,$(LOCAL_PATH))
