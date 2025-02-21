// Copyright (C) 2013-2021 Intel Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ACL_HAL_MMD_H
#define ACL_HAL_MMD_H

#ifndef ACL_HAL_MMD_EXPORT
#define ACL_HAL_MMD_EXPORT __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Implementation of HAL that builds on top of aocl_mmd (board vendor    //
// visible), acl_pll, and acl_kernel_if.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdio.h>
#include <string>

#include "acl.h"
#include "acl_hal.h"

#include <MMD/aocl_mmd.h>

extern acl_kernel_update_callback acl_kernel_update_fn;
extern acl_process_printf_buffer_callback acl_process_printf_buffer_fn;

// Interface to simulator to describe memory on the 'real' device.
typedef struct {
  unsigned long long start; /* memory start location */
  unsigned long long size;  /* size of this memory */
} aocl_mmd_memory_info_t;

// MMD Version checking
// Since MMD version changes only with major releases it is safe to assume
// this is a float with at most one decimal
#define MMDVERSION_MATCH(A, B) ((float)(A) == (float)(B))
#define MMDVERSION_LESSTHAN(A, B) ((float)(A) < (float)(B))

typedef struct {
  std::string library_name;
  void *mmd_library;

  int (*aocl_mmd_get_offline_info)(
      aocl_mmd_offline_info_t, // requested_info_id,
      size_t,                  // param_value_size,
      void *,                  // param_value,
      size_t *                 // param_size_ret
  );

  int (*aocl_mmd_get_info)(int handle, aocl_mmd_info_t requested_info_id,
                           size_t param_value_size, void *param_value,
                           size_t *param_size_ret);

  int (*aocl_mmd_open)(const char *name);

  int (*aocl_mmd_close)(int handle);

  int (*aocl_mmd_set_interrupt_handler)(int handle,
                                        aocl_mmd_interrupt_handler_fn fn,
                                        void *user_data);

  int (*aocl_mmd_set_device_interrupt_handler)(
      int handle, aocl_mmd_device_interrupt_handler_fn, void *user_data);

  int (*aocl_mmd_set_status_handler)(int handle, aocl_mmd_status_handler_fn fn,
                                     void *user_data);

  int (*aocl_mmd_yield)(int handle);

  int (*aocl_mmd_read)(int handle, aocl_mmd_op_t op, size_t len, void *dst,
                       int mmd_interface, size_t offset);

  int (*aocl_mmd_write)(int handle, aocl_mmd_op_t op, size_t len,
                        const void *src, int mmd_interface, size_t offset);

  int (*aocl_mmd_copy)(int handle, aocl_mmd_op_t op, size_t len,
                       int mmd_interface, size_t src_offset, size_t dst_offset);

  // DEPRECATED. Use aocl_mmd_program instead
  int (*aocl_mmd_reprogram)(int handle, void *user_data, size_t size);

  void *(*aocl_mmd_shared_mem_alloc)(int handle, size_t size,
                                     unsigned long long *device_ptr_out);

  void (*aocl_mmd_shared_mem_free)(int handle, void *host_ptr, size_t size);

  // Only populated if MPSIM is active.
  void (*aocl_mmd_simulation_device_info)(int handle, int num_memories,
                                          aocl_mmd_memory_info_t *mem_info);

  // Host Channel calls
  int (*aocl_mmd_hostchannel_create)(int handle, char *channel_name,
                                     size_t queue_depth, int direction);

  int (*aocl_mmd_hostchannel_destroy)(int handle, int channel);

  void *(*aocl_mmd_hostchannel_get_buffer)(int handle, int channel,
                                           size_t *buffer_size, int *status);

  size_t (*aocl_mmd_hostchannel_ack_buffer)(int handle, int channel,
                                            size_t send_size, int *status);

  // This is the new aocl_mmd_program API for MMD later than 14.1. Acl hal will
  // choose either one according to mmd version
  int (*aocl_mmd_program)(int handle, void *user_data, size_t size,
                          aocl_mmd_program_mode_t program_mode);

  void *(*aocl_mmd_host_alloc)(int *handles, size_t num_devices, size_t size,
                               size_t alignment,
                               aocl_mmd_mem_properties_t *properties,
                               int *error);

  int (*aocl_mmd_free)(void *mem);

  void *(*aocl_mmd_shared_alloc)(int handle, size_t size, size_t alignment,
                                 aocl_mmd_mem_properties_t *properties,
                                 int *error);

  double mmd_version;
} acl_mmd_dispatch_t;

typedef struct {
  int handle;
  std::string name;
  acl_mmd_dispatch_t *mmd_dispatch;
} acl_mmd_device_t;

#endif
