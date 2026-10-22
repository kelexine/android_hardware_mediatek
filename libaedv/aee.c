//
// SPDX-FileCopyrightText: The LineageOS Project
// SPDX-License-Identifier: Apache-2.0
//

int aee_switch_ftrace(int status) {
    return 0;
}

int aee_system_exception(const char* lib, const char* path, unsigned int options,
                         const char* log_msg) {
    return 0;
}

int aee_system_warning(const char* lib, const char* path, unsigned int options,
                       const char* log_msg) {
    return 0;
}

int aee_log_msg(const char* lib, const char* path, unsigned int options, const char* log_msg,
                const char* ver) {
    return 0;
}

int aee_modem_warning(const char* lib, const char* path, unsigned int options, const char* log_msg,
                      const char* ver) {
    return 0;
}

// Data symbols imported by the stock /vendor/bin/aee_aedv{,64} daemons. Sizes match the
// stock libaedv.so (RegLength: 4 bytes, usersmaps_info: 65544 bytes, userstack_info: 8200 bytes)
// so the daemons can safely reference them through GLOB_DAT relocations.
int RegLength;
char usersmaps_info[65544];
char userstack_info[8200];
