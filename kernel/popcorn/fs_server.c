#include "fs_server.h"
#include "types.h"
#include <popcorn/fs_server.h>
#include <popcorn/pcn_kmsg.h>


/* TODO: Remove if not needed anymore */
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
    printk("Open file count: %d\n", req_files->open_file_count.counter);
    req_files->open_fds = *(process_files->fdt->open_fds);
    printk("Open fd bitmap: %lu\n", req_files->open_fds);
    req_files->full_fds_bits = *(process_files->fdt->full_fds_bits);
    
    /* Go to pointers and copy out file data directly to request */ 
    for (adder = 0; adder < req_files->open_file_count.counter; ++adder)
    {
       req_files->fd_array[adder] = *(process_files->fd_array[adder]);
    }

    return retVal;
}

int send_file_write_request(unsigned int fd, const char __user* buf, size_t count)
{
    int retVal = 0;
    remote_write_req_t* req = kmalloc(sizeof(remote_write_req_t), GFP_KERNEL);

	/* Build request */
	req->header.type = PCN_KMSG_TYPE_FILE_REMOTE_WRITE;
	req->header.prio = PCN_KMSG_PRIO_NORMAL;
   
    //pcn_kmsg_send(); 
    return retVal;
}

static int handle_remote_write(struct pcn_kmsg_message *msg)
{

    return 0;
}

int __init fs_server_init(void)
{
	/* Register handlers */
	REGISTER_KMSG_HANDLER(PCN_KMSG_TYPE_FILE_REMOTE_WRITE, remote_write);

	return 0;
}
//SYSCALL_DEFINE3(write, unsigned int, fd, const char __user *, buf,
//		size_t, count)
