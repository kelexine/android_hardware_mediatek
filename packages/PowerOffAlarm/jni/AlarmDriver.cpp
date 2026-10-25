/*
 * SPDX-FileCopyrightText: 2023 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/system_properties.h>
#include <linux/ioctl.h>
#include <string>

#include <android-base/unique_fd.h>
#include <jni.h>
#include <log/log.h>

#define ALARM_IOW(c, type, size) _IOW('a', (c) | ((type) << 4), size)
#define ANDROID_ALARM_SET(type) ALARM_IOW(2, type, struct timespec)
#define ANDROID_ALARM_POWER_ON 6
#define ANDROID_ALARM_POWER_ON_LOGO 7

static std::string alarm_dev = "/dev/alarm";

static bool is_logo_mode_enabled() {
    char val[PROP_VALUE_MAX] = {};
    if (__system_property_get("ro.vendor.poweroffalarm.use_logo", val) > 0) {
        return strcmp(val, "1") == 0 || strcmp(val, "true") == 0 ||
               strcmp(val, "yes") == 0 || strcmp(val, "on") == 0;
    }
    return false;
}

extern "C" jboolean Java_org_lineageos_poweroffalarm_AlarmDriver_isSupported(JNIEnv*, jobject) {
    return access(alarm_dev.c_str(), F_OK) == 0;
}

extern "C" void Java_org_lineageos_poweroffalarm_AlarmDriver_set(JNIEnv*, jobject, jlong time) {
    android::base::unique_fd fd{open(alarm_dev.c_str(), O_WRONLY)};
    if (!fd.ok()) {
        ALOGE("Unable to open %s: %s", alarm_dev.c_str(), strerror(errno));
        return;
    }

    struct timespec ts = {};
    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;

    // Devices with Little Kernel (LK) like MT6765 require ANDROID_ALARM_POWER_ON (Type 6, no-logo)
    // to boot into ALARM_BOOT (mode 7). Allow property override if logo mode is explicitly desired.
    int alarm_type = is_logo_mode_enabled()
            ? ANDROID_ALARM_POWER_ON_LOGO : ANDROID_ALARM_POWER_ON;

    if (ioctl(fd, ANDROID_ALARM_SET(alarm_type), &ts) == -1) {
        ALOGV("ANDROID_ALARM_SET (type %d) failed: %s, trying fallback", alarm_type, strerror(errno));
        int fallback_type = (alarm_type == ANDROID_ALARM_POWER_ON)
                ? ANDROID_ALARM_POWER_ON_LOGO : ANDROID_ALARM_POWER_ON;
        if (ioctl(fd, ANDROID_ALARM_SET(fallback_type), &ts) == -1) {
            ALOGE("ANDROID_ALARM_SET ioctl failed: %s", strerror(errno));
            return;
        }
    }
}

extern "C" void Java_org_lineageos_poweroffalarm_AlarmDriver_cancel(JNIEnv* env, jobject thiz) {
    Java_org_lineageos_poweroffalarm_AlarmDriver_set(env, thiz, 0);
}

extern "C" jlong Java_org_lineageos_poweroffalarm_AlarmDriver_getRtcTimeOptional(JNIEnv*, jobject) {
    return -1L;
}
