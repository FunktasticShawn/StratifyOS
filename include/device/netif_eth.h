/* Copyright 2011-2017 Tyler Gilbert;
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifndef DEVICE_NETIF_ENET_H_
#define DEVICE_NETIF_ENET_H_

#include "sos/fs/devfs.h"
#include "sos/dev/drive.h"
#include "mcu/eth.h"

typedef struct {
	const char * buf;
	int * nbyte;
	int count;
	int timeout;
	uint8_t cmd[16];
	devfs_async_t op;
	mcu_event_handler_t handler;
	u32 flags;
} netif_eth_state_t;

typedef struct {
    eth_config_t sdio;
} netif_eth_config_t;


int netif_eth_open(const devfs_handle_t * handle);
int netif_eth_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int netif_eth_read(const devfs_handle_t * handle, devfs_async_t * rop);
int netif_eth_write(const devfs_handle_t * handle, devfs_async_t * wop);
int netif_eth_close(const devfs_handle_t * handle);

int netif_eth_dma_open(const devfs_handle_t * handle);
int netif_eth_dma_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int netif_eth_dma_read(const devfs_handle_t * handle, devfs_async_t * rop);
int netif_eth_dma_write(const devfs_handle_t * handle, devfs_async_t * wop);
int netif_eth_dma_close(const devfs_handle_t * handle);


#endif /* DEVICE_NETIF_ENET_H_ */
