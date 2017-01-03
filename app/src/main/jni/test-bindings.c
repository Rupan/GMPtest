/*
 *  This file is part of GMPtest, and Android app used to verify GMP builds.
 *  Copyright (C) 2015 Michael Mohr <akihana@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <jni.h>
#include <gmp.h>
#include <android/log.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAG "TestDriver"

static jclass oom_error = NULL;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jclass local_oom_error;

    if((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        __android_log_write(ANDROID_LOG_FATAL, TAG, "Call to GetEnv failed on load");
        return JNI_ERR;
    }

    local_oom_error = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
    if(local_oom_error == NULL) {
        __android_log_write(ANDROID_LOG_FATAL, TAG, "Could not find OutOfMemoryError");
        return JNI_ERR;
    }
    oom_error = (jclass) (*env)->NewGlobalRef(env, local_oom_error);
    if(oom_error == NULL) {
        __android_log_write(ANDROID_LOG_FATAL, TAG, "Could create global reference to OutOfMemoryError");
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv *env;

    if((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        __android_log_write(ANDROID_LOG_FATAL, TAG, "Call to GetEnv failed on unload");
        return;
    }
    if(oom_error != NULL) {
        (*env)->DeleteGlobalRef(env, oom_error);
        oom_error = NULL;
    }
}

/*
 *  A trivial example of how to use GMP from Java.  No error checking is performed.
 */
JNIEXPORT jstring JNICALL Java_org_gmplib_tests_TestDriver_add(JNIEnv *env, jobject thiz, jstring jst_a, jstring jst_b, jint base) {
    jboolean isCopy;
    mpz_t bn_a, bn_b, bn_sum;
    const char *cc_a, *cc_b;
    jstring jst_sum;
    char *c_sum;

    mpz_inits(bn_a, bn_b, bn_sum, NULL);

    cc_a = (*env)->GetStringUTFChars(env, jst_a, &isCopy);

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "a=%s; isCopy=%d", cc_a, (int)isCopy);
    mpz_set_str(bn_a, cc_a, (int)base);

    if(isCopy == JNI_TRUE) {
        __android_log_write(ANDROID_LOG_DEBUG, TAG, "Destroying cc_a");
        (*env)->ReleaseStringUTFChars(env, jst_a, cc_a);
    }
    cc_a = NULL;

    cc_b = (*env)->GetStringUTFChars(env, jst_b, &isCopy);

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "b=%s; isCopy=%d", cc_b, (int)isCopy);
    mpz_set_str(bn_b, cc_b, (int)base);

    if(isCopy == JNI_TRUE) {
        __android_log_write(ANDROID_LOG_DEBUG, TAG, "Destroying cc_b");
        (*env)->ReleaseStringUTFChars(env, jst_b, cc_b);
    }
    cc_b = NULL;

    mpz_add(bn_sum, bn_a, bn_b);
    c_sum = mpz_get_str(NULL, (int)base, bn_sum);
    if(c_sum == NULL) {
        (*env)->ThrowNew(env, oom_error, "Failed to convert result to string");
        return NULL;
    }

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "sum=%s", c_sum);

    jst_sum = (*env)->NewStringUTF(env, c_sum);

    free(c_sum);
    mpz_clears(bn_a, bn_b, bn_sum, NULL);
    return jst_sum;
}

#ifdef __cplusplus
}
#endif
