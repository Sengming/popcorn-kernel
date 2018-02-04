#ifndef __KERNEL_POPCORN_FS_SERVER_H__
#define __KERNEL_POPCORN_FS_SERVER_H__
#include "types.h"

struct task_struct;
struct files_struct;

int copy_over_open_files(struct files_struct* process_files, clone_request_t* req);

#endif
