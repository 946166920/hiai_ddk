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

#ifndef _MMPA_API_H_
#define _MMPA_API_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <signal.h>
#include <pthread.h>
#include <syslog.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <net/if.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <dirent.h>
#include <getopt.h>
#include <libgen.h>

#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/fs.h>
#include <linux/limits.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/statvfs.h>
#include <sys/prctl.h>

#include "infra/mmpa/sub_inc/mmpa_typedef_linux.h"
#include "infra/mmpa/sub_inc/mmpa_linux.h"

#endif /* _MMPA_API_H_ */

