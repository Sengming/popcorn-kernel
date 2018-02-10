/*
 * @file init.c
 *
 * Copyright (c) 2013 - 2014, Akshay
 * modified by Antonio Barbalace, 2014
 * rewritten by Sang-Hoon Kim, 2016-2017
 */

#include <linux/kernel.h>
#include <linux/workqueue.h>

#include <popcorn/debug.h>
#include "types.h"

struct workqueue_struct *popcorn_wq;
struct workqueue_struct *popcorn_ordered_wq;
EXPORT_SYMBOL(popcorn_wq);
EXPORT_SYMBOL(popcorn_ordered_wq);

extern int pcn_kmsg_init(void);
extern int popcorn_nodes_init(void);
extern int sched_server_init(void);
extern int process_server_init(void);
extern int vma_server_init(void);
extern int page_server_init(void);
extern int remote_info_init(void);
extern int fs_server_init(void);

#ifdef CONFIG_POPCORN_STAT
extern int statistics_init(void);
#endif

static int __init popcorn_init(void)
{
	PRINTK("Initialize Popcorn subsystems...\n");

	/**
	 * Create work queues so that we can do bottom side
	 * processing on data that was brought in by the
	 * communications module interrupt handlers.
	 */
	popcorn_ordered_wq = create_singlethread_workqueue("pcn_wq_ordered");
	popcorn_wq = alloc_workqueue("pcn_wq", WQ_MEM_RECLAIM, 0);

	pcn_kmsg_init();

	popcorn_nodes_init();
	vma_server_init();
	process_server_init();
	page_server_init();
	sched_server_init();
        fs_server_init();
	remote_info_init();

#ifdef CONFIG_POPCORN_STAT
	statistics_init();
#endif
	return 0;
}
late_initcall(popcorn_init);
