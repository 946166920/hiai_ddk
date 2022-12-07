/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MMPA_LINUX_MMPA_LINUX_H
#define MMPA_LINUX_MMPA_LINUX_H
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>

#include "mmpa_typedef_linux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

typedef struct dirent mmDirent;

typedef struct {
    int         wSecond;       // Seconds. [0-60] (1 leap second)
    int         wMinute;       // Minutes. [0-59]
    int         wHour;         // Hours. [0-23]
    int         wDay;          // Day. [1-31]
    int         wMonth;        // Month. [1-12]
    int         wYear;         // Year
    int         wDayOfWeek;    // Day of week. [0-6]
    int         tm_yday;       // Days in year.[0-365]
    int         tm_isdst;      // DST. [-1/0/1]
    long int    wMilliseconds; // milliseconds
}mmSystemTime_t;

typedef struct {
    int32_t tz_minuteswest;   // 和greenwich 时间差了多少分钟
    int32_t tz_dsttime;       // type of DST correction
} mmTimezone;

typedef struct {
    long  tv_sec;
    long  tv_usec;
} mmTimeval;


typedef struct {
    long  tv_sec;
    long  tv_nsec;
} mmTimespec;

typedef mode_t mmMode_t;
typedef mode_t     MODE;

#ifdef __ANDROID__
#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#endif

#define M_FILE_RDONLY O_RDONLY
#define M_FILE_WRONLY O_WRONLY
#define M_FILE_RDWR O_RDWR
#define M_FILE_CREAT O_CREAT


#define M_RDONLY O_RDONLY
#define M_WRONLY O_WRONLY
#define M_RDWR O_RDWR
#define M_CREAT O_CREAT
#define M_BINARY O_RDONLY

#define M_IREAD   S_IREAD
#define M_IRUSR   S_IRUSR
#define M_IWRITE  S_IWRITE
#define M_IWUSR   S_IWUSR
#define M_IXUSR   S_IXUSR
#define FDSIZE    64
#define M_MSG_CREAT IPC_CREAT
#define M_MSG_EXCL (IPC_CREAT | IPC_EXCL)
#define M_MSG_NOWAIT IPC_NOWAIT

#define M_WAIT_NOHANG WNOHANG       // 不阻塞等待
#define M_WAIT_UNTRACED WUNTRACED   // 若子进程进入暂停状态，则马上返回，
                                    // 但子进程的结束状态不予以理会
#define M_UMASK_USRREAD S_IRUSR
#define M_UMASK_GRPREAD S_IRGRP
#define M_UMASK_OTHREAD S_IROTH

#define M_UMASK_USRWRITE S_IWUSR
#define M_UMASK_GRPWRITE S_IWGRP
#define M_UMASK_OTHWRITE S_IWOTH

#define M_UMASK_USREXEC S_IXUSR
#define M_UMASK_GRPEXEC S_IXGRP
#define M_UMASK_OTHEXEC S_IXOTH

#define mmConstructor(x) __attribute__((constructor)) void x()
#define mmDestructor(x) __attribute__((destructor)) void x()

#define MMPA_MAX_PATH PATH_MAX

extern int32_t mmGetLocalTime(mmSystemTime_t *sysTime);

extern int32_t mmOpen(const char *pathName, int32_t flags);
extern int32_t mmOpen2(const char *pathName, int32_t flags, MODE mode);
extern int32_t mmClose(int32_t fd);
extern int32_t mmWrite(int32_t fd, const void *mmBuf, uint32_t mmCount);
extern int32_t mmRead(int32_t fd, void *mmBuf, uint32_t mmCount);
extern void* mmDlopen(const char *fileName, int32_t mode);
extern void* mmDlsym(void *handle, const char *funcName);
extern int32_t mmDlclose(void *handle);
extern char* mmDlerror(void);
extern int32_t mmMkdir(const char *pathName, mmMode_t mode);
extern int32_t mmSleep(uint32_t milliSecond);

extern int32_t mmAccess(const char *pathName);
extern int32_t mmRmdir(const char *pathName);

extern int32_t mmGetTimeOfDay(mmTimeval *timeVal, mmTimezone *timeZone);
extern mmTimespec mmGetTickCount(void);
extern int32_t mmGetRealPath(char *path, char *realPath); // 待删除
extern int32_t mmRealPath(const char *path, char *realPath, int32_t realPathLen);
extern int32_t mmFileno(FILE *stream);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _MMPA_LINUX_MMPA_LINUX_H_ */

