#include "fs_server.h"
#include "types.h"
#include <popcorn/fs_server.h>
#include <popcorn/pcn_kmsg.h>
#include <linux/uaccess.h>

///* TODO: Remove if not needed anymore */
//int copy_over_open_files(struct files_struct* process_files, clone_request_t* req)
//{
//    int retVal = 0;
//    int adder = 0;
//    struct process_filedata* req_files = &(req->process_open_files);
//    
//    /* Basic argument null pointer check */
//    BUG_ON(!process_files);
//    BUG_ON(!req);
//
//    /* Copy over values */
//    req_files->open_file_count = process_files->count;
//    printk("Open file count: %d\n", req_files->open_file_count.counter);
//    req_files->open_fds = *(process_files->fdt->open_fds);
//    printk("Open fd bitmap: %lu\n", req_files->open_fds);
//    req_files->full_fds_bits = *(process_files->fdt->full_fds_bits);
//    
//    /* Go to pointers and copy out file data directly to request */ 
//    for (adder = 0; adder < req_files->open_file_count.counter; ++adder)
//    {
//       req_files->fd_array[adder] = *(process_files->fd_array[adder]);
//    }
//
//    return retVal;
//}

int send_file_write_request(unsigned int fd, const char __user* buf, size_t count, int origin_nid)
{
    int ret = 0;
    remote_write_req_t* req = kmalloc(sizeof(remote_write_req_t), GFP_KERNEL);

	/* Build request */
	req->header.type = PCN_KMSG_TYPE_FILE_REMOTE_WRITE;
	req->header.prio = PCN_KMSG_PRIO_NORMAL;
    req->origin_pid = current->origin_pid; 
    req->fd = fd;
    req->write_len = count;
    if (!!copy_from_user(req->buf, buf, count)){
        //TODO (Smyte): Potentially handle this better by adding a series of writesto a kfifo
        printk("FS_Server: Overflow, some of the buffer was not sent!\n");
        ret = -ENOMEM;
        goto out_fail;
    }

    if (!!(ret = pcn_kmsg_send(origin_nid, req, sizeof(*req)))) {
        goto out_fail;
    }
        
    return ret;

out_fail:
    kfree(req);
    return ret;
}

extern ssize_t do_sys_file_write(unsigned int fd, const char __user* buf, size_t count);
int process_remote_write(struct pcn_kmsg_message *msg)
{
    remote_write_req_t* req = (remote_write_req_t*)msg;
    BUG_ON(!req);
    do_sys_file_write(req->fd, req->buf, req->write_len);
    //printk("Remote write called: Fd number given is: %d\n", req->fd);
    //printk("Buffer is: %s\n", req->buf); 

    return 0;
}

DEFINE_KMSG_RW_HANDLER(remote_write, remote_write_req_t, origin_pid);

/////////////////////////////////////////////////////////////////////
// READ FUNCTIONS
/////////////////////////////////////////////////////////////////////


int send_file_read_request(unsigned int fd, size_t count, int origin_nid)
{
    int ret = 0;
    int i = 0;
    remote_read_req_t* req = kmalloc(sizeof(remote_read_req_t), GFP_KERNEL);

	/* Build request */
	req->header.type = PCN_KMSG_TYPE_FILE_REMOTE_READ_REQ;
	req->header.prio = PCN_KMSG_PRIO_NORMAL;
    req->origin_pid = current->origin_pid; 
    req->fd = fd;
    req->read_len = count;

    if (!!(ret = pcn_kmsg_send(origin_nid, req, sizeof(*req)))) {
        goto out_fail;
    }
    
    for (i = 0; i < 4; ++i) {
        printk("Read test vals: %d\n", current->remote->remote_read_test[i]);
    }
    return ret;

out_fail:
    kfree(req);
    return ret;
}

int process_remote_read_req(struct pcn_kmsg_message *msg)
{
    int ret = 0;
    remote_read_req_t* req = (remote_read_req_t*)msg;
    BUG_ON(!req); 
    // TODO: Process the read request 
    return ret;
}

static int handle_remote_read_reply(struct pcn_kmsg_message *msg)
{
    int ret = 0;
    // TODO: Populate;
    return ret;
}

DEFINE_KMSG_RW_HANDLER(remote_read_req, remote_read_req_t, origin_pid);

int __init fs_server_init(void)
{
	/* Register handlers */
	REGISTER_KMSG_HANDLER(PCN_KMSG_TYPE_FILE_REMOTE_WRITE, remote_write);
    REGISTER_KMSG_HANDLER(PCN_KMSG_TYPE_FILE_REMOTE_READ_REQ, remote_read_req);
    REGISTER_KMSG_HANDLER(PCN_KMSG_TYPE_FILE_REMOTE_READ_REPLY, remote_read_reply);
	return 0;
}
