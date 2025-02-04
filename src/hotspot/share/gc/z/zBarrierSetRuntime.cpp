/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "precompiled.hpp"
#include "gc/z/zBarrier.inline.hpp"
#include "gc/z/zBarrierSetRuntime.hpp"
#include "runtime/interfaceSupport.inline.hpp"

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::load_barrier_on_oop_field_preloaded(oopDesc* o, oop* p))
    /*if(ZAddress::is_keep(ZOop::to_address(o))){
        log_info(gc, heap)("Throw Barrier:");
    }*/
    /*if(ZAddress::is_good(ZOop::to_address(o))){
        ZBarrier::skipweakbarrier++;
        if(ZBarrier::skipweakbarrier/(1024*32*4)!=(ZBarrier::skipweakbarrier-1)/(1024*32*4)) {
            log_info(gc, heap)("Good load: " SIZE_FORMAT, ZBarrier::skipweakbarrier);
        }
    }

    if(ZAddress::is_null(ZOop::to_address(o))){
        ZBarrier::non_skipweakbarrier++;
        if(ZBarrier::non_skipweakbarrier/(1024*32*4)!=(ZBarrier::non_skipweakbarrier-1)/(1024*32*4)) {
            log_info(gc, heap)("Null load: " SIZE_FORMAT, ZBarrier::non_skipweakbarrier);
        }
    }*/
    /*if(o->klass_or_null() == NULL)
        return o;*/
  return ZBarrier::load_barrier_on_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(void, ZBarrierSetRuntime::check_value(oopDesc* o, oop* p))
    if(ZAddress::is_keep(ZOop::to_address(o))){
        log_info(gc, heap)("Skip Load:");
    }
JRT_END

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::weak_load_barrier_on_oop_field_preloaded(oopDesc* o, oop* p))
  return ZBarrier::weak_load_barrier_on_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::weak_load_barrier_on_weak_oop_field_preloaded(oopDesc* o, oop* p))
  return ZBarrier::weak_load_barrier_on_weak_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::weak_load_barrier_on_phantom_oop_field_preloaded(oopDesc* o, oop* p))
  return ZBarrier::weak_load_barrier_on_phantom_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::load_barrier_on_weak_oop_field_preloaded(oopDesc* o, oop* p))
  return ZBarrier::load_barrier_on_weak_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(oopDesc*, ZBarrierSetRuntime::load_barrier_on_phantom_oop_field_preloaded(oopDesc* o, oop* p))
  return ZBarrier::load_barrier_on_phantom_oop_field_preloaded(p, o);
JRT_END

JRT_LEAF(void, ZBarrierSetRuntime::load_barrier_on_oop_array(oop* p, size_t length))
  ZBarrier::load_barrier_on_oop_array(p, length);
  /*if(ZAddress::is_keep(ZOop::to_address(*p))){
      log_info(gc, heap)("Array Barrier:");
  }*/
JRT_END

JRT_LEAF(void, ZBarrierSetRuntime::clone(oopDesc* src, oopDesc* dst, size_t size))
  HeapAccess<>::clone(src, dst, size);
JRT_END

address ZBarrierSetRuntime::load_barrier_on_oop_field_preloaded_addr(DecoratorSet decorators) {
  if (decorators & ON_PHANTOM_OOP_REF) {
    if (decorators & AS_NO_KEEPALIVE) {
      return weak_load_barrier_on_phantom_oop_field_preloaded_addr();
    } else {
      return load_barrier_on_phantom_oop_field_preloaded_addr();
    }
  } else if (decorators & ON_WEAK_OOP_REF) {
    if (decorators & AS_NO_KEEPALIVE) {
      return weak_load_barrier_on_weak_oop_field_preloaded_addr();
    } else {
      return load_barrier_on_weak_oop_field_preloaded_addr();
    }
  } else {
    if (decorators & AS_NO_KEEPALIVE) {
      return weak_load_barrier_on_oop_field_preloaded_addr();
    } else {
      return load_barrier_on_oop_field_preloaded_addr();
    }
  }
}

address ZBarrierSetRuntime::load_barrier_on_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(load_barrier_on_oop_field_preloaded);
}

address ZBarrierSetRuntime::load_barrier_on_weak_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(load_barrier_on_weak_oop_field_preloaded);
}

address ZBarrierSetRuntime::load_barrier_on_phantom_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(load_barrier_on_phantom_oop_field_preloaded);
}

address ZBarrierSetRuntime::weak_load_barrier_on_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(weak_load_barrier_on_oop_field_preloaded);
}

address ZBarrierSetRuntime::weak_load_barrier_on_weak_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(weak_load_barrier_on_weak_oop_field_preloaded);
}

address ZBarrierSetRuntime::weak_load_barrier_on_phantom_oop_field_preloaded_addr() {
  return reinterpret_cast<address>(weak_load_barrier_on_phantom_oop_field_preloaded);
}

address ZBarrierSetRuntime::load_barrier_on_oop_array_addr() {
  return reinterpret_cast<address>(load_barrier_on_oop_array);
}

address ZBarrierSetRuntime::clone_addr() {
  return reinterpret_cast<address>(clone);
}

address ZBarrierSetRuntime::check_address_value(){
    return reinterpret_cast<address>(check_value);
}
