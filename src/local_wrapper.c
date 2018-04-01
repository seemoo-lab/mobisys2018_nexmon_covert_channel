/***************************************************************************
 *                                                                         *
 *          ###########   ###########   ##########    ##########           *
 *         ############  ############  ############  ############          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ###########   ####  ######  ##   ##   ##  ##    ######          *
 *          ###########  ####  #       ##   ##   ##  ##    #    #          *
 *                   ##  ##    ######  ##   ##   ##  ##    #    #          *
 *                   ##  ##    #       ##   ##   ##  ##    #    #          *
 *         ############  ##### ######  ##   ##   ##  ##### ######          *
 *         ###########    ###########  ##   ##   ##   ##########           *
 *                                                                         *
 *            S E C U R E   M O B I L E   N E T W O R K I N G              *
 *                                                                         *
 * This file is part of NexMon.                                            *
 *                                                                         *
 * Copyright (c) 2016 NexMon Team                                          *
 *                                                                         *
 * NexMon is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * NexMon is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with NexMon. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                         *
 **************************************************************************/

#ifndef LOCAL_WRAPPER_C
#define LOCAL_WRAPPER_C

#include <firmware_version.h>
#include <structs.h>
#include <stdarg.h>

#ifndef WRAPPER_H
    // if this file is not included in the wrapper.h file, create dummy functions
    #define VOID_DUMMY { ; }
    #define RETURN_DUMMY { ; return 0; }

    #define AT(CHIPVER, FWVER, ADDR) __attribute__((weak, at(ADDR, "dummy", CHIPVER, FWVER)))
#else
    // if this file is included in the wrapper.h file, create prototypes
    #define VOID_DUMMY ;
    #define RETURN_DUMMY ;
    #define AT(CHIPVER, FWVER, ADDR)
#endif

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x184968)
unsigned int
si_pmu_chipcontrol(void *sih, unsigned int reg, unsigned int mask, unsigned int val)
RETURN_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c6e7e)
void
wlapi_bmac_write_shm(void *physhim, unsigned int offset, unsigned short v)
VOID_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c1dba)
void
wlc_phy_lpf_hpc_override_acphy(void *pi, bool setup_not_cleanup)
VOID_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c6ea2)
void
wlapi_enable_mac(void *physhim)
VOID_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c6e96)
void
wlapi_suspend_mac_and_wait(void *physhim)
VOID_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c53d6)
bool
wlc_phy_get_rxgainerr_phy(void *pi, int16 *gainerr)
RETURN_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_ALL, 0x4fad8)
void
wlc_bmac_set_clk(void *wlc_hw, bool on)
VOID_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704, 0x1c6962)
int
wlc_phy_iovar_dispatch(void *pi, uint32 actionid, uint16 type, void *p, uint plen, void *a, int alen, int vsize)
RETURN_DUMMY

AT(CHIP_VER_BCM4339, FW_VER_ALL, 0x35094)
int
wlc_module_register(void *pub, const void *iovars, const char *name, void *hdl, void *iovar_fn, void *watchdog_fn, void *up_fn, void *down_fn)
RETURN_DUMMY

#undef VOID_DUMMY
#undef RETURN_DUMMY
#undef AT

#endif /*LOCAL_WRAPPER_C*/
