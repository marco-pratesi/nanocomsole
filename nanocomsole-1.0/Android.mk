LOCAL_PATH:= $(call my-dir)

include $(call all-subdir-makefiles)
include $(CLEAR_VARS)

#######################################################
LOCAL_MODULE := nanoconsole
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES := menu.c mux.c nanocomsole.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
include $(BUILD_EXECUTABLE)

#######################################################

include $(call all-makefiles-under,$(LOCAL_PATH))
