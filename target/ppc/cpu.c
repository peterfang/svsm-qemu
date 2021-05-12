/*
 *  PowerPC CPU routines for qemu.
 *
 * Copyright (c) 2017 Nikunj A Dadhania, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "cpu-models.h"
#include "fpu/softfloat-helpers.h"

target_ulong cpu_read_xer(CPUPPCState *env)
{
    if (is_isa300(env)) {
        return env->xer | (env->so << XER_SO) |
            (env->ov << XER_OV) | (env->ca << XER_CA) |
            (env->ov32 << XER_OV32) | (env->ca32 << XER_CA32);
    }

    return env->xer | (env->so << XER_SO) | (env->ov << XER_OV) |
        (env->ca << XER_CA);
}

void cpu_write_xer(CPUPPCState *env, target_ulong xer)
{
    env->so = (xer >> XER_SO) & 1;
    env->ov = (xer >> XER_OV) & 1;
    env->ca = (xer >> XER_CA) & 1;
    /* write all the flags, while reading back check of isa300 */
    env->ov32 = (xer >> XER_OV32) & 1;
    env->ca32 = (xer >> XER_CA32) & 1;
    env->xer = xer & ~((1ul << XER_SO) |
                       (1ul << XER_OV) | (1ul << XER_CA) |
                       (1ul << XER_OV32) | (1ul << XER_CA32));
}

void ppc_store_vscr(CPUPPCState *env, uint32_t vscr)
{
    env->vscr = vscr & ~(1u << VSCR_SAT);
    /* Which bit we set is completely arbitrary, but clear the rest.  */
    env->vscr_sat.u64[0] = vscr & (1u << VSCR_SAT);
    env->vscr_sat.u64[1] = 0;
    set_flush_to_zero((vscr >> VSCR_NJ) & 1, &env->vec_status);
}

uint32_t ppc_get_vscr(CPUPPCState *env)
{
    uint32_t sat = (env->vscr_sat.u64[0] | env->vscr_sat.u64[1]) != 0;
    return env->vscr | (sat << VSCR_SAT);
}
