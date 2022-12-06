/*
 * @file mmpa_linuxtypedef_linux.h
 *
 * Copyright (C) <2019> <Huawei Technologies Co., Ltd.>. All Rights Reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Description:MMPA库 Linux部分类型定义头文件
 * Create:2017-12-22
 */

#ifndef MMPA_TYPEDEF_LINUX_H
#define MMPA_TYPEDEF_LINUX_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef FALSE
#define FALSE                     0
#endif

#ifndef TRUE
#define TRUE                      1
#endif

#define HANDLE_INVALID_VALUE   (-1)
#define MMPA_MEM_MAX_LEN       (0x7fffffff)
#define MMPA_PROCESS_ERROR     (0x7fffffff)
#define PATH_SIZE                   256
#define MAX_IOVEC_SIZE              32
#define MMPA_MAX_SLEEP_MILLSECOND 4294967
#define MAX_PIPE_COUNT            2
#define MMPA_PIPE_COUNT           2
#define MMPA_THREADNAME_SIZE      16
#define MMPA_MIN_OS_NAME_SIZE     64
#define MMPA_MIN_OS_VERSION_SIZE    128

#define MMPA_ONE_THOUSAND         1000
#define MMPA_ONE_BILLION          1000000000
#define MMPA_COMPUTER_BEGIN_YEAR  1900
#define MMPA_ZERO                 0
#define MMPA_MAX_THREAD_PIO       99
#define MMPA_MIN_THREAD_PIO       1
#define MMPA_DEFAULT_PIPE_PERMISSION 0777
#define MMPA_DEFAULT_MSG_TYPE 1

#define MMPA_THREAD_SCHED_RR SCHED_RR
#define MMPA_THREAD_SCHED_FIFO SCHED_FIFO
#define MMPA_THREAD_SCHED_OTHER SCHED_OTHER
#define MMPA_THREAD_MIN_STACK_SIZE PTHREAD_STACK_MIN

static const int EN_OK = 0;
static const int EN_ERROR = -1;
static const int EN_INVALID_PARAM = -2;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif /* _MMPA_TYPEDEF_LINUX_H_ */

