#include "fs_server.h"
#include "types.h"

int copy_over_open_files(struct files_struct* process_files, clone_request_t* req)
{
    int retVal = 0;
    struct files_struct* req_files = req->process_open_files;
    
    /* Basic argument null pointer check */
    BUG_ON(!process_files);
    BUG_ON(!req);

    /* Copy over values */
    req_files->open_file_count = process_files->count;
    req_files->open_fds = *(process_files->fdt->open_fds);

    return retVal;
}

/* TODO: Remove */
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
