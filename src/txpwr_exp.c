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

#pragma NEXMON targetregion "patch"

#include <firmware_version.h>   // definition of firmware version macros
#include <debug.h>              // contains macros to access the debug hardware
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <rates.h>              // rates used to build the ratespec for frame injection
#include <capabilities.h>		// capabilities included in a nexmon patch

/* TX Power index defines */
#define WL_NUM_RATES_CCK			4 /* 1, 2, 5.5, 11 Mbps */
#define WL_NUM_RATES_OFDM			8 /* 6, 9, 12, 18, 24, 36, 48, 54 Mbps SISO/CDD */
#define WL_NUM_RATES_MCS_1STREAM	8 /* MCS 0-7 1-stream rates - SISO/CDD/STBC/MCS */
#define WL_NUM_RATES_EXTRA_VHT		2 /* Additional VHT 11AC rates */
#define WL_NUM_RATES_VHT			10
#define WL_NUM_RATES_MCS32			1

#define WLC_NUM_RATES_CCK       WL_NUM_RATES_CCK
#define WLC_NUM_RATES_OFDM      WL_NUM_RATES_OFDM
#define WLC_NUM_RATES_MCS_1_STREAM  WL_NUM_RATES_MCS_1STREAM
#define WLC_NUM_RATES_MCS_2_STREAM  WL_NUM_RATES_MCS_1STREAM
#define WLC_NUM_RATES_MCS32     WL_NUM_RATES_MCS32
#define WL_TX_POWER_CCK_NUM     WL_NUM_RATES_CCK
#define WL_TX_POWER_OFDM_NUM        WL_NUM_RATES_OFDM
#define WL_TX_POWER_MCS_1_STREAM_NUM    WL_NUM_RATES_MCS_1STREAM
#define WL_TX_POWER_MCS_2_STREAM_NUM    WL_NUM_RATES_MCS_1STREAM
#define WL_TX_POWER_MCS_32_NUM      WL_NUM_RATES_MCS32

#define WL_NUM_2x2_ELEMENTS		4
#define WL_NUM_3x3_ELEMENTS		6

typedef struct txppr {
	/* start of 20MHz tx power limits */
	int8 b20_1x1dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20_1x1ofdm[WL_NUM_RATES_OFDM];		/* Legacy OFDM transmission */
	int8 b20_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];		/* SISO MCS 0-7 */

	int8 b20_1x2dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b20_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b20_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b20_1x3dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b20_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b20_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b20_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b20_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b20_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b20_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b20_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b20_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b20_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b20_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b20_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	/* start of 40MHz tx power limits */
	int8 b40_dummy1x1dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40_1x1ofdm[WL_NUM_RATES_OFDM];		/* Legacy OFDM transmission */
	int8 b40_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];		/* SISO MCS 0-7 */

	int8 b40_dummy1x2dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b40_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b40_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b40_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b40_dummy1x3dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b40_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b40_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b40_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b40_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b40_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b40_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b40_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b40_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b40_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b40_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b40_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b40_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	/* start of 20in40MHz tx power limits */
	int8 b20in40_1x1dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b20in40_1x1ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM transmission */
	int8 b20in40_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];	/* SISO MCS 0-7 */

	int8 b20in40_1x2dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20in40_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b20in40_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b20in40_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20in40_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b20in40_1x3dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20in40_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* 20 in 40 MHz Legacy OFDM CDD */
	int8 b20in40_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b20in40_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20in40_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b20in40_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b20in40_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b20in40_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b20in40_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b20in40_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b20in40_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b20in40_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b20in40_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b20in40_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	/* start of 80MHz tx power limits */
	int8 b80_dummy1x1dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b80_1x1ofdm[WL_NUM_RATES_OFDM];			/* Legacy OFDM transmission */
	int8 b80_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];	/* SISO MCS 0-7 */

	int8 b80_dummy1x2dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b80_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b80_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b80_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b80_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b80_dummy1x3dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b80_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b80_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b80_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b80_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b80_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b80_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b80_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b80_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b80_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b80_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b80_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b80_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b80_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	/* start of 20in80MHz tx power limits */
	int8 b20in80_1x1dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b20in80_1x1ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM transmission */
	int8 b20in80_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];	/* SISO MCS 0-7 */

	int8 b20in80_1x2dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20in80_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b20in80_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b20in80_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20in80_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b20in80_1x3dsss[WL_NUM_RATES_CCK];		/* Legacy CCK/DSSS */
	int8 b20in80_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b20in80_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b20in80_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b20in80_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b20in80_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b20in80_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b20in80_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b20in80_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b20in80_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b20in80_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b20in80_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b20in80_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b20in80_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	/* start of 40in80MHz tx power limits */
	int8 b40in80_dummy1x1dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40in80_1x1ofdm[WL_NUM_RATES_OFDM];		/* Legacy OFDM transmission */
	int8 b40in80_1x1mcs0[WL_NUM_RATES_MCS_1STREAM];	/* SISO MCS 0-7 */

	int8 b40in80_dummy1x2dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40in80_1x2cdd_ofdm[WL_NUM_RATES_OFDM];	/* Legacy OFDM CDD transmission */
	int8 b40in80_1x2cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* CDD MCS 0-7 */
	int8 b40in80_2x2stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b40in80_2x2sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* MCS 8-15 */

	int8 b40in80_dummy1x3dsss[WL_NUM_RATES_CCK];	/* Legacy CCK/DSSS */
	int8 b40in80_1x3cdd_ofdm[WL_NUM_RATES_OFDM];	/* MHz Legacy OFDM CDD */
	int8 b40in80_1x3cdd_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* 1 Nsts to 3 Tx Chain */
	int8 b40in80_2x3stbc_mcs0[WL_NUM_RATES_MCS_1STREAM];	/* STBC MCS 0-7 */
	int8 b40in80_2x3sdm_mcs8[WL_NUM_RATES_MCS_1STREAM];	/* 2 Nsts to 3 Tx Chain */
	int8 b40in80_3x3sdm_mcs16[WL_NUM_RATES_MCS_1STREAM];	/* 3 Nsts to 3 Tx Chain */

	int8 b40in80_1x1vht[WL_NUM_RATES_EXTRA_VHT];		/* VHT8_9SS1 */
	int8 b40in80_1x2cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD1 */
	int8 b40in80_2x2stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC */
	int8 b40in80_2x2sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2 */
	int8 b40in80_1x3cdd_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_CDD2 */
	int8 b40in80_2x3stbc_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS1_STBC_SPEXP1 */
	int8 b40in80_2x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS2_SPEXP1 */
	int8 b40in80_3x3sdm_vht[WL_NUM_RATES_EXTRA_VHT];	/* VHT8_9SS3 */

	int8 mcs32; /* C_CHECK - THIS NEEDS TO BE REMOVED THROUGHOUT THE CODE */
} txppr_t;

void
wlc_bmac_set_chanspec_hook(void *wlc_hw, int chanspec, int mute, txppr_t *txpwr)
{
	//printf("%s: %08x %08x %08p\n", __FUNCTION__, chanspec, mute, txpwr);

	if (txpwr > 0 && 0) {
		printf("b20_1x1dsss: %02x %02x %02x %02x\n", (uint8) txpwr->b20_1x1dsss[0], (uint8) txpwr->b20_1x1dsss[1], (uint8) txpwr->b20_1x1dsss[2], (uint8) txpwr->b20_1x1dsss[3]);
		printf("b20_1x1ofdm: %02x %02x %02x %02x %02x %02x %02x %02x\n", (uint8) txpwr->b20_1x1ofdm[0], (uint8) txpwr->b20_1x1ofdm[1], (uint8) txpwr->b20_1x1ofdm[2], (uint8) txpwr->b20_1x1ofdm[3], (uint8) txpwr->b20_1x1ofdm[4], (uint8) txpwr->b20_1x1ofdm[5], (uint8) txpwr->b20_1x1ofdm[6], (uint8) txpwr->b20_1x1ofdm[7]);
		printf("mcs32: %02x\n", (uint8) txpwr->mcs32);
	}
}

void
wlc_channel_set_chanspec_hook(void *wlc_cm, int chanspec, int local_constraint_qdbm)
{
	//printf("%s: %08x %08x\n", __FUNCTION__, chanspec, local_constraint_qdbm);
}

__attribute__((at(0x1ABA2C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
HookPatch4(wlc_bmac_set_chanspec_hook, wlc_bmac_set_chanspec_hook, "push {r3-r9,lr}");

__attribute__((at(0x1AE40C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
HookPatch4(wlc_channel_set_chanspec_hook, wlc_channel_set_chanspec_hook, "push {r4-r8,lr}");
