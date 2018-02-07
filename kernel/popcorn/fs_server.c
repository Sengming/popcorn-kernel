#include "fs_server.h"
#include "types.h"
#include <popcorn/fs_server.h>

int copy_over_open_files(struct files_struct* process_files, clone_request_t* req)
{
    int retVal = 0;
    int adder = 0;
    struct process_filedata* req_files = &(req->process_open_files);
    
    /* Basic argument null pointer check */
    BUG_ON(!process_files);
    BUG_ON(!req);

    /* Copy over values */
    req_files->open_file_count = process_files->count;
    req_files->open_fds = *(process_files->fdt->open_fds);
    req_files->full_fds_bits = *(process_files->fdt->full_fds_bits);
    
    /* Go to pointers and copy out file data directly to request */ 
    for (adder = 0; adder < req_files->open_file_count.counter; ++adder)
    {
       req_files->fd_array[adder] = *(process_files->fd_array[adder]); 
    }

    return retVal;
}

