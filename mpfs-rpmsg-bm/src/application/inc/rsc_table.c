/*
 * Copyright (c) 2022, Microchip Technology. All rights reserved.
 * Copyright (c) 2014, Mentor Graphics Corporation. All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 * Copyright 2020 NXP. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remote
 * for use by the Linux master */

#include "rsc_table.h"
#include "rpmsg_lite.h"
#include <string.h>

#define NUM_VRINGS 0x02

/* Shared memory base for RPMsg communication. */
#define VDEV0_VRING_BASE      (0x91D00000)
#define RESOURCE_TABLE_OFFSET (0x50000)
#define VIRTIO_ID_RPMSG       (0x7)

const struct remote_resource_table __attribute__((section(".resource_table"))) resources = {
    /* Version */
    .version = 1,

    /* Number of table entries */
    .num = NO_RESOURCE_ENTRIES,
    /* reserved fields */
    .reserved = {0, 0},

    /* Offsets of rsc entries */
    .offset = {
        offsetof(struct remote_resource_table, rpmsg_vdev),
    },

    /* virtio device entry */
    .rpmsg_vdev = {
        RSC_VDEV,
        VIRTIO_ID_RPMSG,
        0,
        RSC_VDEV_FEATURE_NS,
        0,
        0,
        0,
        NUM_VRINGS,
        {0, 0},
    },

    /* Vring rsc entry - part of vdev rsc entry */
    .rpmsg_vring0 = {VDEV0_VRING_BASE, 0x1000U, 256, 0, 0},
    .rpmsg_vring1 = {VDEV0_VRING_BASE + VRING_SIZE, 0x1000U, 256, 1, 0},
};

void copyResourceTable(void)
{
    memcpy((void *)(VDEV0_VRING_BASE + RESOURCE_TABLE_OFFSET), &resources, sizeof(resources));
}
