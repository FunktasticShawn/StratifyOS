/* Copyright 2011-2016 Tyler Gilbert; 
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
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>. */


#ifndef _MCU_TYPES_H_
#define _MCU_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#if !defined __link
#include <sys/stat.h>
#endif

#include "ioctl.h"

typedef uint8_t u8;
typedef int8_t s8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;

#ifdef __cplusplus
extern "C" {
#endif


#define MCU_ALIAS(f) __attribute__((weak, alias (#f)))
#define MCU_WEAK __attribute__((weak))

#if defined __MINGW32__
#define MCU_PACK __attribute__((packed, gcc_struct))
#else
#define MCU_PACK __attribute__((packed))
#endif

#define MCU_NAKED __attribute__((naked))
#define MCU_ALIGN(x) __attribute__((aligned (x)))
#define MCU_ALWAYS_INLINE __attribute__((always_inline))
#define MCU_NEVER_INLINE __attribute__((noinline))


enum mcu_event_codes {
	MCU_EVENT_OP_CANCELLED = 0xABCDEF,
};


typedef struct MCU_PACK {
	u32 o_events /*! Event or events that happened */;
	void * event_data /*! A pointer to the device specific event data */;
} mcu_event_data_t;

typedef enum {
	MCU_EVENT_CANCELLED = (1<<31)
} mcu_even_t_t;

typedef void * mcu_event_t;

#define MCU_EVENT_SET_CODE(x) ((mcu_event_t)((x<<1) | 0x01))
#define MCU_EVENT_CODE(x) ((u32)x >> 1)


typedef int (*mcu_callback_t)(void*, mcu_event_t);

typedef struct {
	mcu_callback_t callback;
	void * context;
} mcu_event_handler_t;

/*! \details This attribute can be applied to RAM so
 * that it is stored in system memory that is universally
 * readable but can only be written in privileged mode.
 *
 * Example:
 * \code
 * static char buffer[512] MCU_SYS_MEM;
 * \endcode
 */
#define MCU_SYS_MEM __attribute__((section(".sysmem")))

#ifdef __link
#define MCU_PRIV_CODE
#else
#define MCU_PRIV_CODE __attribute__((section(".priv_code")))
#endif

#ifdef __link
#define MCU_PRIV_EXEC_CODE
#else
#define MCU_PRIV_EXEC_CODE __attribute__((section(".priv_exec_code")))
#endif

/*! \details This structure defines an action
 * to take when an asynchronous event occurs (such as
 * a pin change interrupt).
 *
 * \sa I_SETACTION
 */
typedef struct {
	u8 channel /*! The channel (a GPIO pin or timer channel) */;
	s8 prio /*! The interrupt priority */;
	u16 o_events /*! The peripheral specific event */;
	mcu_event_handler_t handler /*! Event handler */;
} mcu_action_t;


typedef struct MCU_PACK {
	u8 port;
	u8 pin;
} mcu_pin_t;

static inline int mcu_is_port_valid(u8 port){
	return (port != 0xff);
}

typedef struct MCU_PACK {
	u32 channel;
	u32 value;
} mcu_channel_t;




#define I_MCU_GETINFO 0
#define I_MCU_SETATTR 1
#define I_MCU_SETACTION (2 | _IOCTL_ROOT)
#define I_MCU_TOTAL 3

#define MCU_TEST_BIT(x,y) (x & (1<<y))
#define MCU_SET_BIT(x,y) (x |= (1<<y))
#define MCU_SET_MASK(x,y) (x |= y)
#define MCU_CLR_BIT(x,y) (x &= ~(1<<y))
#define MCU_CLR_MASK(x,y) (x &= ~y)

typedef struct {
	u32 sn[4];
} mcu_sn_t;




#ifdef __cplusplus
}
#endif


#endif /* _MCU_TYPES_H_ */
