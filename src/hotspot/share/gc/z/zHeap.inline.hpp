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

#ifndef SHARE_GC_Z_ZHEAP_INLINE_HPP
#define SHARE_GC_Z_ZHEAP_INLINE_HPP

#include "gc/z/zAddress.inline.hpp"
#include "gc/z/zForwardingTable.inline.hpp"
#include "gc/z/zHash.inline.hpp"
#include "gc/z/zHeap.hpp"
#include "gc/z/zMark.inline.hpp"
#include "gc/z/zPage.inline.hpp"
#include "gc/z/zPageTable.inline.hpp"
#include "utilities/debug.hpp"

inline ZHeap* ZHeap::heap() {
  assert(_heap != NULL, "Not initialized");
  return _heap;
}

inline ReferenceDiscoverer* ZHeap::reference_discoverer() {
  return &_reference_processor;
}

inline bool ZHeap::is_object_in_keep(uintptr_t addr){
    ZPage* page = _page_table.get(addr);
    if(page->is_keep())
        return true;
    else
        return false;
}

/*inline void ZHeap::setDirect(uintptr_t addr){
    ZPage* page = _page_table.get(addr);
    page->set_direct(true);
}*/

inline uint32_t ZHeap::hash_oop(uintptr_t addr) const {
  const uintptr_t offset = ZAddress::offset(addr);
  return ZHash::address_to_uint32(offset);
}

inline bool ZHeap::is_object_live(uintptr_t addr) const {
  ZPage* page = _page_table.get(addr);
  return page->is_object_live(addr);
}

inline bool ZHeap::page_is_marked(uintptr_t addr) const {
    ZPage* page = _page_table.get(addr);
    bool result = page->is_allocating()?true:page->is_marked();
    if(result) {
        bool check = page->is_allocating();
        //log_info(gc, heap)("No need!");
    }
    return result;
}

inline bool ZHeap::is_object_strongly_live(uintptr_t addr) const {
  ZPage* page = _page_table.get(addr);
  return page->is_object_strongly_live(addr);
}

template <bool follow, bool finalizable, bool publish>
inline void ZHeap::mark_object(uintptr_t addr) {
  assert(ZGlobalPhase == ZPhaseMark, "Mark not allowed");
  /*if(_page_table.get(addr)->is_keep()){
      return;
  }*/
  _mark.mark_object<follow, finalizable, publish>(addr);
}

inline uintptr_t ZHeap::alloc_tlab(size_t size) {
  guarantee(size <= max_tlab_size(), "TLAB too large");
  return _object_allocator.alloc_object(size);
}

inline uintptr_t ZHeap::alloc_tklab(size_t size) {
    //guarantee(size <= max_tlab_size(), "TLAB too large");
    return _object_allocator.alloc_tklab(size);
}

inline uintptr_t ZHeap::alloc_object(size_t size) {
  uintptr_t addr = _object_allocator.alloc_object(size);
  assert(ZAddress::is_good_or_null(addr), "Bad address");

  if (addr == 0) {
    out_of_memory();
  }

  return addr;
}

inline uintptr_t ZHeap::alloc_object_non_blocking(size_t size) {
  uintptr_t addr = _object_allocator.alloc_object_non_blocking(size);
  assert(ZAddress::is_good_or_null(addr), "Bad address");
  return addr;
}

inline uintptr_t ZHeap::alloc_keep_object_non_blocking(size_t size) {
    return _object_allocator.alloc_keep_object_non_blocking(size);
}

inline void ZHeap::undo_alloc_object(uintptr_t addr, size_t size) {
  ZPage* const page = _page_table.get(addr);
  _object_allocator.undo_alloc_object(page, addr, size);
}

inline uintptr_t ZHeap::relocate_object(uintptr_t addr) {
  assert(ZGlobalPhase == ZPhaseRelocate, "Relocate not allowed");

  ZForwarding* const forwarding = _forwarding_table.get(addr);
  if (forwarding == NULL) {
    // Not forwarding
    return ZAddress::good(addr);
  }

  // Relocate object
  return _relocate.relocate_object(forwarding, ZAddress::good(addr));
}

inline uintptr_t ZHeap::remap_object(uintptr_t addr) {
  assert(ZGlobalPhase == ZPhaseMark ||
         ZGlobalPhase == ZPhaseMarkCompleted, "Forward not allowed");

  ZForwarding* const forwarding = _forwarding_table.get(addr);
  if (forwarding == NULL) {
    // Not forwarding
    return ZAddress::good(addr);
  }

  // Forward object
  return _relocate.forward_object(forwarding, ZAddress::good(addr));
}

inline bool ZHeap::is_alloc_stalled() const {
  return _page_allocator.is_alloc_stalled();
}

inline void ZHeap::check_out_of_memory() {
  _page_allocator.check_out_of_memory();
}

inline bool ZHeap::is_oop(uintptr_t addr) const {
  return ZAddress::is_good(addr) && is_object_aligned(addr) && is_in(addr);
}

#endif // SHARE_GC_Z_ZHEAP_INLINE_HPP
