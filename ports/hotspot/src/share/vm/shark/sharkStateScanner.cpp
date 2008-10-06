/*
 * Copyright 1999-2007 Sun Microsystems, Inc.  All Rights Reserved.
 * Copyright 2008 Red Hat, Inc.
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
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 *
 */

#include "incls/_precompiled.incl"
#include "incls/_sharkStateScanner.cpp.incl"

using namespace llvm;

void SharkStateScanner::scan(SharkState* state)
{
  start_frame();

  // Expression stack
  stack_integrity_checks(state);
  start_stack(state->stack_depth(), state->max_stack());
  for (int i = state->stack_depth() - 1; i >= 0; i--) {
    process_stack_slot(
      i,
      state->stack_addr(i),
      function()->stack_slots_offset() +
        i + state->max_stack() - state->stack_depth());
  }
  end_stack();

  // Monitors
  start_monitors(function()->monitor_count());
  for (int i = 0; i < function()->monitor_count(); i++) {
    process_monitor(
      i,
      function()->monitors_slots_offset() +
        i * frame::interpreter_frame_monitor_size());
  }
  end_monitors();

  // Frame header
  start_frame_header();
  process_exception_slot(function()->exception_slot_offset());
  process_method_slot(state->method_addr(), function()->method_slot_offset());
  process_pc_slot(function()->pc_slot_offset());
  end_frame_header();

  // Local variables
  locals_integrity_checks(state);
  start_locals(state->max_locals());
  for (int i = 0; i < state->max_locals(); i++) {
    process_local_slot(
      i,
      state->local_addr(i),
      function()->locals_slots_offset() + state->max_locals() - 1 - i);
  }
  end_locals();

  end_frame();
}

#ifndef PRODUCT
void SharkStateScanner::stack_integrity_checks(SharkState* state)
{
  for (int i = 0; i < state->stack_depth(); i++) {
    if (state->stack(i)) {
      if (state->stack(i)->is_two_word())
        assert(state->stack(i - 1) == NULL, "should be");
    }
    else {
      assert(state->stack(i + 1)->is_two_word(), "should be");
    }
  }
}
  
void SharkStateScanner::locals_integrity_checks(SharkState* state)
{
  for (int i = 0; i < state->max_locals(); i++) {
    if (state->local(i)) {
      if (state->local(i)->is_two_word())
        assert(state->local(i + 1) == NULL, "should be");
    }
  } 
}
#endif // !PRODUCT