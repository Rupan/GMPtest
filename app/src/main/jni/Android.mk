JNI_PATH := $(call my-dir)

# Uncomment the following line to include GMP's C++ bindings in your APK
#GMP_WITH_CPLUSPLUS := yes
include $(JNI_PATH)/gmp/Android.mk

LOCAL_PATH := $(JNI_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := gmp-tests
LOCAL_SRC_FILES := test-bindings.c

LOCAL_LDLIBS += -llog
LOCAL_SHARED_LIBRARIES := gmp
include $(BUILD_SHARED_LIBRARY)
