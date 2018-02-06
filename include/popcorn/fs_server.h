#ifndef __POPCORN_INCLUDE_FS_SERVER_H_
#define __POPCORN_INCLUDE_FS_SERVER_H_

#include <linux/fdtable.h>
#include <linux/fs.h>

struct process_filedata
{
    /* Relevant data from open files */
    struct files_struct open_files;
    atomic_t open_file_count;

    /* Fdtable*/
	struct file fd_array[NR_OPEN_DEFAULT];
    unsigned long open_fds;
    unsigned long full_fds_bits;
};


#endif
