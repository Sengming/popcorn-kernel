#ifndef __KERNEL_POPCORN_FS_SERVER_H__
#define __KERNEL_POPCORN_FS_SERVER_H__
#include "types.h"

//int copy_over_open_files(struct files_struct* process_files, clone_request_t* req);
int process_remote_write(struct pcn_kmsg_message *msg);

#endif
