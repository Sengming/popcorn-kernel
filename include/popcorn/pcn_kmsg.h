/*
 * Header file for Popcorn inter-kernel messaging layer
 *
 * (C) Ben Shelton <beshelto@vt.edu> 2013
 */

#ifndef __LINUX_PCN_KMSG_H
#define __LINUX_PCN_KMSG_H

#include <linux/types.h>

enum pcn_connection_status {
	PCN_CONN_WATING,
	PCN_CONN_CONNECTED,
	//PCN_CONN_TYPE_MAX
};
typedef unsigned long pcn_kmsg_mcast_id;
/* MESSAGING */

/* Enum for message types.  Modules should add types after
   PCN_KMSG_END. */
enum pcn_kmsg_type {
	/* RDMA handlers */
	PCN_KMSG_TYPE_RDMA_START,
	PCN_KMSG_TYPE_RDMA_READ_TEST_REQUEST,
	PCN_KMSG_TYPE_RDMA_READ_TEST_RESPONSE,
	PCN_KMSG_TYPE_RDMA_WRITE_TEST_REQUEST,
	PCN_KMSG_TYPE_RDMA_WRITE_TEST_RESPONSE,
	PCN_KMSG_TYPE_RDMA_KEY_EXCHANGE_REQUEST,
	PCN_KMSG_TYPE_RDMA_KEY_EXCHANGE_RESPONSE,
	PCN_KMSG_TYPE_RDMA_END,

	/* message layer testing */
	PCN_KMSG_TYPE_FIRST_TEST,
	PCN_KMSG_TYPE_TEST,
	PCN_KMSG_TYPE_TEST_LONG,

	/* Performance experiments */
	PCN_KMSG_TYPE_SELFIE_TEST,
	PCN_KMSG_TYPE_SEND_ROUND_READ_REQUEST,
	PCN_KMSG_TYPE_SEND_ROUND_READ_RESPONSE,
	PCN_KMSG_TYPE_SEND_ROUND_WRITE_REQUEST,
	PCN_KMSG_TYPE_SEND_ROUND_WRITE_RESPONSE,
	PCN_KMSG_TYPE_SHOW_REMOTE_TEST_BUF,

	/* Provide the single system image */
	PCN_KMSG_TYPE_REMOTE_PROC_CPUINFO_REQUEST,
	PCN_KMSG_TYPE_REMOTE_PROC_CPUINFO_RESPONSE,
	PCN_KMSG_TYPE_REMOTE_PROC_MEMINFO_REQUEST,
	PCN_KMSG_TYPE_REMOTE_PROC_MEMINFO_RESPONSE,
	PCN_KMSG_TYPE_REMOTE_PROC_PS_REQUEST,
	PCN_KMSG_TYPE_REMOTE_PROC_PS_RESPONSE,

	/* Thread migration */
	PCN_KMSG_TYPE_NODE_INFO,
	PCN_KMSG_TYPE_TASK_MIGRATE,
	PCN_KMSG_TYPE_TASK_MIGRATE_BACK,
	PCN_KMSG_TYPE_TASK_PAIRING,
	PCN_KMSG_TYPE_TASK_EXIT_ORIGIN,
	PCN_KMSG_TYPE_TASK_EXIT_REMOTE,

	/* VMA synchronization */
	PCN_KMSG_TYPE_VMA_INFO_REQUEST,
	PCN_KMSG_TYPE_VMA_INFO_RESPONSE,
	PCN_KMSG_TYPE_VMA_OP_REQUEST,
	PCN_KMSG_TYPE_VMA_OP_RESPONSE,

	/* Page consistency protocol */
	PCN_KMSG_TYPE_REMOTE_PAGE_REQUEST,
	PCN_KMSG_TYPE_REMOTE_PAGE_RESPONSE,
	PCN_KMSG_TYPE_REMOTE_PAGE_RESPONSE_SHORT,
	PCN_KMSG_TYPE_REMOTE_PAGE_FLUSH,
	PCN_KMSG_TYPE_REMOTE_PAGE_RELEASE,
	PCN_KMSG_TYPE_REMOTE_PAGE_FLUSH_ACK,
	PCN_KMSG_TYPE_PAGE_INVALIDATE_REQUEST,
	PCN_KMSG_TYPE_PAGE_INVALIDATE_RESPONSE,

	/* Distributed futex */
	PCN_KMSG_TYPE_FUTEX_REQUEST,
	PCN_KMSG_TYPE_FUTEX_RESPONSE,

	/* Schedule server */
	PCN_KMSG_TYPE_SCHED_PERIODIC,

    /* Files server */
    PCN_KMSG_TYPE_FILE_REMOTE_WRITE,

	PCN_KMSG_TYPE_MAX
};

/* Enum for message priority. */
enum pcn_kmsg_prio {
	PCN_KMSG_PRIO_HIGH,
	PCN_KMSG_PRIO_NORMAL
};

#define __READY_SIZE 1
#define LG_SEQNUM_SIZE  (8 - __READY_SIZE)

/* Message header */
struct pcn_kmsg_hdr {
	unsigned int from_nid	:8;
	enum pcn_kmsg_type type	:8;
	enum pcn_kmsg_prio prio	:7;
	bool is_rdma			:1;
	unsigned int size;
	void *recv_ptr;
}__attribute__((packed));

#define CACHE_LINE_SIZE 64
#define PCN_KMSG_PAYLOAD_SIZE (CACHE_LINE_SIZE - sizeof(struct pcn_kmsg_hdr))
#define PCN_KMSG_LONG_PAYLOAD_SIZE 65536
#define PCN_KMSG_MAX_SIZE PCN_KMSG_LONG_PAYLOAD_SIZE + \
							sizeof(struct pcn_kmsg_hdr) + \
							sizeof(struct pcn_kmsg_rdma_hdr)

#define DEFINE_PCN_KMSG(type, fields) \
	typedef struct {				\
		struct pcn_kmsg_hdr header;	\
		fields				\
	}__attribute__((packed)) type

#define DEFINE_PCN_RDMA_KMSG(type, fields) \
	typedef struct {				\
		struct pcn_kmsg_hdr header;	\
		struct pcn_kmsg_rdma_hdr rdma_header; \
		void *private; \
		fields				\
	}__attribute__((packed)) type

/* rdma header */
struct pcn_kmsg_rdma_hdr {
    bool rdma_ack;
    bool is_write;
    uint32_t remote_rkey;
    uint32_t rw_size;
    uint64_t remote_addr;
    void *your_buf_ptr;			/* will be copied to R/W buffer */
    enum pcn_kmsg_type rmda_type_res;	/* response callback func */
};

/* Struct for the actual messages.  Note that hdr and payload are flipped
   when this actually goes out, so the receiver can poll on the ready bit
   in the header. */
struct pcn_kmsg_message {
	struct pcn_kmsg_hdr header;
	struct pcn_kmsg_rdma_hdr rdma_header;
	unsigned char payload[PCN_KMSG_LONG_PAYLOAD_SIZE];
}__attribute__((packed)) __attribute__((aligned(CACHE_LINE_SIZE)));

#define RDMA_TEMPLATE ;
DEFINE_PCN_RDMA_KMSG(pcn_kmsg_rdma_t, RDMA_TEMPLATE);
    
#define RDMA_TEST \
	int remote_ws; \
	u64 dma_addr_act; \
	u32 mr_id; \
	int t_num;
DEFINE_PCN_RDMA_KMSG(pcn_kmsg_perf_rdma_t, RDMA_TEST);

/* TYPES OF MESSAGES */

/* Message struct for guest kernels to check in with each other. */
struct pcn_kmsg_checkin_message {
	struct pcn_kmsg_hdr header;
	unsigned long window_phys_addr;
	unsigned char cpu_to_add;
	char pad[51];
}__attribute__((packed)) __attribute__((aligned(CACHE_LINE_SIZE)));

/* FUNCTIONS */

/* Typedef for function pointer to callback functions */
typedef int (*pcn_kmsg_cbftn)(struct pcn_kmsg_message *);

/* Typedef for function pointer to callback functions */
typedef int (*send_ftn)(unsigned int,
						struct pcn_kmsg_message *,
						unsigned int);
typedef void* (*request_rdma_ftn)(unsigned int,
								pcn_kmsg_rdma_t *,
								unsigned int, unsigned int);
typedef void (*respond_rdma_ftn)(pcn_kmsg_rdma_t *, void *, u32 rw_size);
typedef void (*free_ftn)(struct pcn_kmsg_message *);

/* SETUP */

/* Register a callback function to handle a new message type.  Intended to
   be called when a kernel module is loaded. */
int pcn_kmsg_register_callback(enum pcn_kmsg_type type,
								pcn_kmsg_cbftn callback);

/* Unregister a callback function for a message type.  Intended to
   be called when a kernel module is unloaded. */
int pcn_kmsg_unregister_callback(enum pcn_kmsg_type type);

/* MESSAGING */

/* Send a message to the specified destination CPU. */
int pcn_kmsg_send(unsigned int dest_cpu, void *lmsg, unsigned int msg_size);
void *pcn_kmsg_request_rdma(unsigned int dest_cpu, void *msg,
						unsigned int msg_size, unsigned int rw_size);
void pcn_kmsg_respond_rdma(void *msg, void *paddr, u32 rw_size);

/* Free a received message (called at the end of the callback function) */
void pcn_kmsg_free_msg(void *msg);

/* Allocate a received message */
void *pcn_kmsg_alloc_msg(size_t size);

/* MULTICAST GROUPS */

/* Enum for mcast message type. */
enum pcn_kmsg_mcast_type {
	PCN_KMSG_MCAST_OPEN,
	PCN_KMSG_MCAST_ADD_MEMBERS,
	PCN_KMSG_MCAST_DEL_MEMBERS,
	PCN_KMSG_MCAST_CLOSE,
	PCN_KMSG_MCAST_MAX
};

/* Message struct for guest kernels to check in with each other. */
struct pcn_kmsg_mcast_message {
	struct pcn_kmsg_hdr hdr;
	enum pcn_kmsg_mcast_type type :32;
	pcn_kmsg_mcast_id id;
	unsigned long mask;
	unsigned int num_members;
	unsigned long window_phys_addr;
	char pad[28];
}__attribute__((packed)) __attribute__((aligned(CACHE_LINE_SIZE)));

/* Open a multicast group containing the CPUs specified in the mask. */
int pcn_kmsg_mcast_open(pcn_kmsg_mcast_id *id, unsigned long mask);

/* Add new members to a multicast group. */
int pcn_kmsg_mcast_add_members(pcn_kmsg_mcast_id id, unsigned long mask);

/* Remove existing members from a multicast group. */
int pcn_kmsg_mcast_delete_members(pcn_kmsg_mcast_id id, unsigned long mask);

/* Close a multicast group. */
int pcn_kmsg_mcast_close(pcn_kmsg_mcast_id id);

/* Send a message to the specified multicast group. */
int pcn_kmsg_mcast_send(pcn_kmsg_mcast_id id, struct pcn_kmsg_message *msg);

/* Send a long message to the specified multicast group. */
int pcn_kmsg_mcast_send_long(pcn_kmsg_mcast_id id, void *msg,
		unsigned int payload_size);

extern send_ftn pcn_kmsg_send_ftn;
extern request_rdma_ftn pcn_kmsg_request_rdma_ftn;
extern respond_rdma_ftn pcn_kmsg_respond_rdma_ftn;
extern pcn_kmsg_cbftn pcn_kmsg_cbftns[PCN_KMSG_TYPE_MAX];
extern free_ftn pcn_kmsg_free_ftn;

enum pcn_kmsg_layer_types {
	PCN_KMSG_LAYER_TYPE_UNKNOWN = -1,
	PCN_KMSG_LAYER_TYPE_SOCKET = 0,
	PCN_KMSG_LAYER_TYPE_DOLPHIN,
	PCN_KMSG_LAYER_TYPE_IB,
};
extern enum pcn_kmsg_layer_types pcn_kmsg_layer_type;

#endif /* __LINUX_PCN_KMSG_H */
