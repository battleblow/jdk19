/*
 * Copyright (c) 1999, 2021, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

// no precompiled headers
#include "memory/allocation.inline.hpp"
#include "runtime/mutexLocker.hpp"
#include "runtime/osThread.hpp"

#include <signal.h>

void OSThread::pd_initialize() {
  assert(this != NULL, "check");
  _thread_id        = 0;
  _unique_thread_id = 0;
  _pthread_id       = NULL;
  _siginfo          = NULL;
  _ucontext         = NULL;
  _expanding_stack  = 0;
  _alt_sig_stack    = NULL;

  sigemptyset(&_caller_sigmask);

  _startThread_lock = new Monitor(Mutex::event, "startThread_lock");
  assert(_startThread_lock !=NULL, "check");
}

// Additional thread_id used to correlate threads in SA
void OSThread::set_unique_thread_id() {
#ifdef __APPLE__
  thread_identifier_info_data_t m_ident_info;
  mach_msg_type_number_t count = THREAD_IDENTIFIER_INFO_COUNT;

  mach_port_t mach_thread_port = mach_thread_self();
  guarantee(mach_thread_port != 0, "just checking");
  thread_info(mach_thread_port, THREAD_IDENTIFIER_INFO,
              (thread_info_t) &m_ident_info, &count);
  mach_port_deallocate(mach_task_self(), mach_thread_port);

  _unique_thread_id = m_ident_info.thread_id;
#endif
}

void OSThread::pd_destroy() {
  delete _startThread_lock;
}
