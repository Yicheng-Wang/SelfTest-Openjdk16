/*
 * Copyright (c) 2015, 2020, Oracle and/or its affiliates. All rights reserved.
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
 */

#ifndef SHARE_GC_Z_ZOBJECTALLOCATOR_HPP
#define SHARE_GC_Z_ZOBJECTALLOCATOR_HPP

#include "gc/z/zAllocationFlags.hpp"
#include "gc/z/zValue.hpp"

class ZPage;

class ZObjectAllocator {
private:
  const bool         _use_per_cpu_shared_small_pages;
  ZPerCPU<size_t>    _used;
  ZPerCPU<size_t>    _undone;
  ZContended<ZPage*> _shared_medium_page;
  //ZContended<ZPage*> _shared_medium_keep_page;
  //ZPerCPU<ZPage*>    _shared_small_keep_page;
  ZPerCPU<ZPage*>    _shared_small_page;

  ZPage** shared_small_page_addr();
  ZPage* const* shared_small_page_addr() const;

  //ZPage** shared_small_keep_page_addr();
  //ZPage* const* shared_small_keep_page_addr() const;

  ZPage* alloc_page(uint8_t type, size_t size, ZAllocationFlags flags);
  void undo_alloc_page(ZPage* page);

  // Allocate an object in a shared page. Allocate and
  // atomically install a new page if necessary.
  uintptr_t alloc_object_in_shared_page(ZPage** shared_page,
                                        uint8_t page_type,
                                        size_t page_size,
                                        size_t size,
                                        ZAllocationFlags flags);

    uintptr_t alloc_in_keep_page(uint8_t page_type,
                                 size_t page_size,
                                 size_t size,
                                 ZAllocationFlags flags);

  uintptr_t alloc_large_object(size_t size, ZAllocationFlags flags);
  uintptr_t alloc_medium_object(size_t size, ZAllocationFlags flags);
  //uintptr_t alloc_medium_keep_object(size_t size, ZAllocationFlags flags);
  uintptr_t alloc_small_keep_object(size_t size, ZAllocationFlags flags);
  uintptr_t alloc_small_object(size_t size, ZAllocationFlags flags);
  //uintptr_t alloc_small_share_keep_object(size_t size, ZAllocationFlags flags);
  uintptr_t alloc_object(size_t size, ZAllocationFlags flags);

public:
  ZObjectAllocator();

  uintptr_t alloc_object(size_t size);
  uintptr_t alloc_tklab(size_t size);
  uintptr_t alloc_object_non_blocking(size_t size);
  uintptr_t alloc_keep_object_non_blocking(size_t size);
  void undo_alloc_object(ZPage* page, uintptr_t addr, size_t size);

  size_t used() const;
  size_t remaining() const;

  void retire_pages();
};

#endif // SHARE_GC_Z_ZOBJECTALLOCATOR_HPP
