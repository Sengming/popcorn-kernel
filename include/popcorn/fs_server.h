#ifndef __POPCORN_INCLUDE_FS_SERVER_H_
#define __POPCORN_INCLUDE_FS_SERVER_H_
#include <linux/fs.h>
#define WRITE_KMSG_LEN (100)

//struct process_filedata
//{
//    /* Relevant data from open files */
//    struct files_struct open_files;
//    atomic_t open_file_count;
//
//    /* Fdtable*/
//	struct file fd_array[NR_OPEN_DEFAULT];
//    unsigned long open_fds;
//    unsigned long full_fds_bits;
//};

int send_file_write_request(unsigned int fd, const char __user* buf, 
        size_t count, int origin_nid);
#endif
