/*
 * Copyright (c) 2014, 2015 EMC Corporation.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef NTRDMA_H
#define NTRDMA_H

#include <infiniband/driver.h>

#ifndef offsetof
#define offsetof(type, member) \
	__builtin_offsetof (type, member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
	({ \
	 const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	 (type *)( (char *)__mptr - offsetof(type, member) ); \
	 })
#endif

struct ntrdma_dev {
	struct ibv_device	ibdev;
};

static inline struct ntrdma_dev *to_ntrdma_dev(struct ibv_device *ibdev)
{
	return container_of(ibdev, struct ntrdma_dev, ibdev);
}

int ntrdma_query_device(struct ibv_context *context,
			struct ibv_device_attr *device_attr);
int ntrdma_query_port(struct ibv_context *context, uint8_t port_num,
		      struct ibv_port_attr *port_attr);
struct ibv_pd *ntrdma_alloc_pd(struct ibv_context *context);
int ntrdma_dealloc_pd(struct ibv_pd *pd);
struct ibv_mr *ntrdma_reg_mr(struct ibv_pd *pd, void *addr,
			     size_t length, int access);
int ntrdma_dereg_mr(struct ibv_mr *mr);
struct ibv_cq *ntrdma_create_cq(struct ibv_context *context, int cqe,
				struct ibv_comp_channel *channel,
				int comp_vector);
int ntrdma_poll_cq(struct ibv_cq *cq, int num_entries, struct ibv_wc *wc);
int ntrdma_destroy_cq(struct ibv_cq *cq);
struct ibv_qp *ntrdma_create_qp(struct ibv_pd *pd,
				struct ibv_qp_init_attr *attr);
int ntrdma_modify_qp(struct ibv_qp *qp, struct ibv_qp_attr *attr, int attr_mask);
int ntrdma_destroy_qp(struct ibv_qp *qp);
int ntrdma_query_qp(struct ibv_qp *qp, struct ibv_qp_attr *attr,
		    int attr_mask, struct ibv_qp_init_attr *init_attr);
int ntrdma_post_send(struct ibv_qp *qp, struct ibv_send_wr *swr,
		     struct ibv_send_wr **bad);
int ntrdma_post_recv(struct ibv_qp *qp, struct ibv_recv_wr *rwr,
		     struct ibv_recv_wr **bad);
struct ibv_ah *ntrdma_create_ah(struct ibv_pd *pd, struct ibv_ah_attr *attr);
int ntrdma_destroy_ah(struct ibv_ah *ah);
int ntrdma_req_notify_cq(struct ibv_cq *cq, int solicited_only);

#endif
