#include "fs_server.h"
#include "types.h"

int copy_over_open_files(struct files_struct* process_files, clone_request_t* req)
{
    int retVal = 0;
    /* Copy over values */
    req->process_open_files.count = process_files->count;
    req->process_open_files.fdtable = process_files->fdtable;

    return retVal;
}
