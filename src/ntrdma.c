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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "ntrdma.h"

static struct ibv_context *ntrdma_alloc_context(struct ibv_device *ibdev,
						int cmd_fd)
{
	struct ibv_context *ibctx;
	struct ibv_get_context cmd;
	struct ibv_get_context_resp resp;

	ibctx = malloc(sizeof(*ibctx));
	if (!ibctx)
		return NULL;

	memset(ibctx, 0, sizeof(*ibctx));
	ibctx->cmd_fd = cmd_fd;

	if (ibv_cmd_get_context(ibctx,
				&cmd, sizeof(cmd),
				&resp, sizeof(resp)))
		goto err_free;

	ibctx->device = ibdev;
	ibctx->ops.query_device		= ntrdma_query_device;
	ibctx->ops.query_port		= ntrdma_query_port;
	ibctx->ops.alloc_pd		= ntrdma_alloc_pd;
	ibctx->ops.dealloc_pd		= ntrdma_dealloc_pd;
	ibctx->ops.reg_mr		= ntrdma_reg_mr;
	ibctx->ops.dereg_mr		= ntrdma_dereg_mr;
	ibctx->ops.create_cq		= ntrdma_create_cq;
	ibctx->ops.poll_cq		= ntrdma_poll_cq;
	ibctx->ops.destroy_cq		= ntrdma_destroy_cq;
	ibctx->ops.create_qp		= ntrdma_create_qp;
	ibctx->ops.modify_qp		= ntrdma_modify_qp;
	ibctx->ops.destroy_qp		= ntrdma_destroy_qp;
	ibctx->ops.query_qp		= ntrdma_query_qp;
	ibctx->ops.post_send		= ntrdma_post_send;
	ibctx->ops.post_recv		= ntrdma_post_recv;
	ibctx->ops.create_ah		= ntrdma_create_ah;
	ibctx->ops.destroy_ah		= ntrdma_destroy_ah;
	ibctx->ops.req_notify_cq	= ntrdma_req_notify_cq;

	return ibctx;

err_free:
	free(ibctx);
	return NULL;
}

static void ntrdma_free_context(struct ibv_context *ibctx)
{
	free(ibctx);
}

static struct ibv_device *ntrdma_driver_init(const char *uverbs_sys_path,
					     int abi_version)
{
	char value[32];
	int ibdevno;
	struct ntrdma_dev *dev;
	struct ibv_device *ibdev;

	fprintf(stderr, "%s(\"%s\", %d)\n", __FUNCTION__,
		uverbs_sys_path, abi_version);

	if (ibv_read_sysfs_file(uverbs_sys_path, "ibdev",
				value, sizeof(value)) < 0)
		return NULL;
	if (sscanf(value, "ntrdma_%i", &ibdevno) != 1)
		return NULL;

	dev = malloc(sizeof *dev);
	if (!dev)
		return NULL;

	memset(dev, 0, sizeof(*dev));
	ibdev = &dev->ibdev;

	ibdev->ops.alloc_context	= ntrdma_alloc_context;
	ibdev->ops.free_context		= ntrdma_free_context;

	return ibdev;
}

static __attribute__((constructor)) void ntrdma_register_driver(void)
{
	ibv_register_driver("ntrdma", ntrdma_driver_init);
}
