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
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <nexioctls.h>          // ioctls added in the nexmon patch
#include <argprintf.h>          // allows to execute argprintf to print into the arg buffer
#include <local_wrapper.h>
#include <sample_collect.h>
#include <channels.h>
#include <signalgeneration.h>
#include <sendframe.h>
#include <rates.h>

#define PMU1_PLL0_PLLCTL0       0
#define SAMPLE_SYNC_CLK_BIT     17

#define ACPHY_DEFAULT           0x00001ff
#define ACCONF                  ACPHY_DEFAULT

#define CONF_HAS(config, val)   ((config) & (1U << (val)))
#define CONF_IS(config, val)    ((config) == (uint)(1U << (val)))
#define CONF_GE(config, val)    ((config) & (0 - (1U << (val))))
#define CONF_GT(config, val)    ((config) & (0 - 2 * (1U << (val))))
#define CONF_LT(config, val)    ((config) & ((1U << (val)) - 1))
#define CONF_LE(config, val)    ((config) & (2 * (1U << (val)) - 1))

#define ACCONF_HAS(val)     CONF_HAS(ACCONF, val)
#define ACCONF_MSK(mask)    CONF_MSK(ACCONF, mask)
#define ACCONF_IS(val)      CONF_IS(ACCONF, val)
#define ACCONF_GE(val)      CONF_GE(ACCONF, val)
#define ACCONF_GT(val)      CONF_GT(ACCONF, val)
#define ACCONF_LT(val)      CONF_LT(ACCONF, val)
#define ACCONF_LE(val)      CONF_LE(ACCONF, val)

#define ACREV_IS(var, val)  (ACCONF_HAS(val) && (ACCONF_IS(val) || ((var) == (val))))
#define ACREV_GE(var, val)  (ACCONF_GE(val) && (!ACCONF_LT(val) || ((var) >= (val))))
#define ACREV_GT(var, val)  (ACCONF_GT(val) && (!ACCONF_LE(val) || ((var) > (val))))
#define ACREV_LT(var, val)  (ACCONF_LT(val) && (!ACCONF_GE(val) || ((var) < (val))))
#define ACREV_LE(var, val)  (ACCONF_LE(val) && (!ACCONF_GT(val) || ((var) <= (val))))

#define ACMAJORREV_3(phy_rev) \
    ((ACREV_IS(phy_rev, 4) || ACREV_IS(phy_rev, 7)))

#define ACMAJORREV_2(phy_rev) \
    ((ACREV_IS(phy_rev, 3) || ACREV_IS(phy_rev, 8)))

#define ACMAJORREV_1(phy_rev) \
    ((ACREV_IS(phy_rev, 2) || ACREV_IS(phy_rev, 5) || ACREV_IS(phy_rev, 6)))

#define ACMAJORREV_0(phy_rev) \
    ((ACREV_IS(phy_rev, 0) || ACREV_IS(phy_rev, 1)))

#define wreg32(r, v)        (*(volatile uint32*)(r) = (uint32)(v))
#define rreg32(r)       (*(volatile uint32*)(r))
#define wreg16(r, v)        (*(volatile uint16*)(r) = (uint16)(v))
#define rreg16(r)       (*(volatile uint16*)(r))
#define wreg8(r, v)     (*(volatile uint8*)(r) = (uint8)(v))
#define rreg8(r)        (*(volatile uint8*)(r))

#define BCM_REFERENCE(data) ((void)(data))

#define ASSERT(x)

#define R_REG(osh, r) ({ \
    __typeof(*(r)) __osl_v; \
    BCM_REFERENCE(osh); \
    switch (sizeof(*(r))) { \
    case sizeof(uint8): __osl_v = rreg8((void *)(r)); break; \
    case sizeof(uint16):    __osl_v = rreg16((void *)(r)); break; \
    case sizeof(uint32):    __osl_v = rreg32((void *)(r)); break; \
    } \
    __osl_v; \
})
#define W_REG(osh, r, v) do { \
    BCM_REFERENCE(osh); \
    switch (sizeof(*(r))) { \
    case sizeof(uint8): wreg8((void *)(r), (v)); break; \
    case sizeof(uint16):    wreg16((void *)(r), (v)); break; \
    case sizeof(uint32):    wreg32((void *)(r), (v)); break; \
    } \
} while (0)

/* fifoaddr */
#define D64_FA_OFF_MASK     0xffff      /* offset */
#define D64_FA_SEL_MASK     0xf0000     /* select */
#define D64_FA_SEL_SHIFT    16
#define D64_FA_SEL_XDD      0x00000     /* transmit dma data */
#define D64_FA_SEL_XDP      0x10000     /* transmit dma pointers */
#define D64_FA_SEL_RDD      0x40000     /* receive dma data */
#define D64_FA_SEL_RDP      0x50000     /* receive dma pointers */
#define D64_FA_SEL_XFD      0x80000     /* transmit fifo data */
#define D64_FA_SEL_XFP      0x90000     /* transmit fifo pointers */
#define D64_FA_SEL_RFD      0xc0000     /* receive fifo data */
#define D64_FA_SEL_RFP      0xd0000     /* receive fifo pointers */
#define D64_FA_SEL_RSD      0xe0000     /* receive frame status data */
#define D64_FA_SEL_RSP      0xf0000     /* receive frame status pointers */

#define ACPHY_TBL_ID_IQLOCAL                     12

#define ACPHY_macbasedDACPlay(rev)                           (0x471)

static void
exp_set_gains_by_index(struct phy_info *pi, int8 index)
{
    ac_txgain_setting_t gains = { 0 };
    wlc_phy_txpwrctrl_enable_acphy(pi, 0);
    wlc_phy_get_txgain_settings_by_index_acphy(pi, &gains, index);
    wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
}

static void
exp_set_gains(struct phy_info *pi, uint8 ipa, uint8 txlpf, uint8 pga, uint8 pad, uint8 txgm, uint8 bbmult)
{
    wlc_phy_txpwrctrl_enable_acphy(pi, 0);
    ac_txgain_setting_t gains = { 0 };
    gains.ipa = ipa;  // default: 255 (650 mW)
    gains.txlpf = txlpf;  // default: 0
    gains.pga = pga;
    gains.pad = pad;  // default: 255 (100 mW)
    gains.txgm = txgm; // default: 255
    gains.bbmult = bbmult; // maximum value: 255, for iq transmissions set to 64, normally below
    wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
}

__attribute__((naked))
void
bbmult_write_0(uint32 *phyregs, uint32 addrdata0, uint32 addrdata1, uint32 offset)
{
    asm(
        "str r3, [r0]\n"
        "str r1, [r0]\n"
        "b bbmult_write_1\n"
        );
}

__attribute__((naked))
void
bbmult_write_1(uint32 *phyregs, uint32 addrdata0, uint32 addrdata1, uint32 offset)
{
    asm(
        "str r3, [r0]\n"
        "str r2, [r0]\n"
        "b bbmult_write_0\n"
        );
}

static void
continuous_tx_tone_transmission(struct hndrte_timer *t) {
    struct wlc_info *wlc = (struct wlc_info *) t->data;
    struct phy_info *pi = wlc->hw->band->pi;
    struct d11regs *regs = wlc->hw->regs;
    uint32 *phyregs = (uint32 *) &regs->phyregaddr;

    set_scansuppress(wlc, 1);
    set_mpc(wlc, 0);
    set_chanspec(wlc, CH20MHZ_CHSPEC(1));
    exp_set_gains_by_index(pi, 40);
    //wlc_phy_set_tx_bbmult_acphy(pi, 0, 0);
    wlc_phy_stay_in_carriersearch_acphy(pi, 1);
    
    int32 num_samps = 512;
    cint16ap *freq_dom_samps = (cint16ap *) malloc(sizeof(cint16ap) * num_samps, 0);
    memset(freq_dom_samps, 0, sizeof(cint16ap) * num_samps);
    freq_dom_samps[IDFTCARRIER(100, num_samps)].amplitude = 100;
    freq_dom_samps[IDFTCARRIER(100, num_samps)].phase = 0;
    //my_phy_tx_ifft_acphy_ext(pi, freq_dom_samps, 0 /* iqmode */, 0 /* mac_based */, 0 /* modify_bbmult */, 1 /* runsamples */, 0xffff /* loops */, num_samps);
    my_phy_tx_tone_acphy_ext(pi, 4000, 160, 0, 0 /* mac_based */, 0, 1, 0xffff, 480);

    char x = 0;
    phy_reg_write(pi, 13, 12);
    //phy_reg_write(pi, 14, 99);

    //bbmult_write_0(phyregs, 15 | (0 << 16), 15 | (64 << 16), 14 | (99 << 16));

    pi->fn_txiqccset(pi, 0, 0x200);
    while(1);
#if 0
    while(1) {
        if (x == 1) {
            //pi->fn_txiqccset(pi, 0, 0);
            phy_reg_write(pi, 14, 96);
            phy_reg_write(pi, 15, 0xff);
            phy_reg_write(pi, 15, 0xff);
            x = 0;
        } else {
            //pi->fn_txiqccset(pi, 0xfff, 0xfff);
            phy_reg_write(pi, 14, 96);
            phy_reg_write(pi, 15, 0);
            phy_reg_write(pi, 15, 0);
            x = 1;
        }
    }
#endif
#if 0
    while(1) {
        
        if (x == 1) {
            //gains.bbmult = 0;
            //uint8 stall_val = phy_reg_read(pi, 0x19E);
            //phy_reg_mod(pi, 0x19E, 2, 2);
            //wlc_phy_set_tx_bbmult_acphy(pi, &gains.bbmult, 0);
            //wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_IQLOCAL, 1, 99, 16, &gains.bbmult);
            //phy_reg_write(pi, 13, 12);
            
            phy_reg_write(pi, 14, 99);
            phy_reg_write(pi, 15, 0);
            //*phyregs = 14 | (99 << 16);
            //*phyregs = 15 | (0 << 16);

            //phy_reg_mod(pi, 0x19E, 2, stall_val & 2);
            //wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
            //exp_set_gains(pi, 255, 0, 50, 255, 255, 0);
            //exp_set_gains_by_index(pi, 120);
            x = 0;
        } else {
            //gains.bbmult = 64;
            //uint8 stall_val = phy_reg_read(pi, 0x19E);
            //phy_reg_mod(pi, 0x19E, 2, 2);
            //wlc_phy_set_tx_bbmult_acphy(pi, &gains.bbmult, 0);
            //wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_IQLOCAL, 1, 99, 16, &gains.bbmult);
            //phy_reg_write(pi, 13, 12);
            
            phy_reg_write(pi, 14, 99);
            phy_reg_write(pi, 15, 64);
            //*phyregs = 14 | (99 << 16);
            //*phyregs = 15 | (64 << 16);

            //phy_reg_mod(pi, 0x19E, 2, stall_val & 2);
            //wlc_phy_txcal_txgain_cleanup_acphy(pi, &gains);
            //exp_set_gains(pi, 255, 0, 50, 255, 255, 64);
            //exp_set_gains_by_index(pi, 40);
            x = 1;
        }
        
    }
#endif
}

struct beacon {
  char dummy[40];
  char ssid_len;
  char ssid[32];
} beacon = {
  .dummy = {
    0x80, 0x00, 0x00, 0x00, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x11, 0x22, 0x33, 0x44, 
    0x55, 0x00, 0x11, 0x22, 0x33, 
    0x44, 0x55, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0x01, 0x82, 0x00
  },
  .ssid_len = 0,
  .ssid = { 0 }
};

struct beacon beacon2 = {
  .dummy = {
    0x88, 0x00, 0x00, 0x00, 0xff, 
    0x00, 0x11, 0x22, 0x33, 0x44, 
    0x00, 0x11, 0x22, 0x33, 0x44, 
    0x00, 0x11, 0x22, 0x33, 0x44, 
    0x44, 0x55, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0x01, 0x82, 0x00
  },
  .ssid_len = 0,
  .ssid = { 0 }
};

char packet_bytes[] = {
  0x88, 0x42, 0x2c, 0x00, 
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // dest
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // transmitter
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // src
  0x20, 0x21, 0x00, 0x00, 0x46, 0x09, 0x00, 0x20, 0x00, 0x00,
  0x00, 0x00, 
  0x32, 0xf5, 0x22, 0xdf, 0xf1, 0xb2, 0xf5, 0x9b,
  0x19, 0x20, 0x0e, 0x56, 0x9e, 0x27, 0xac, 0x7c,
  0x6c, 0xb0, 0xca, 0x4b, 0x56, 0x10, 0x10, 0x51,
  0x8e, 0xe2, 0x19, 0x75, 0x4f, 0x80, 0x44, 0x7d,
  0x87, 0x73, 0xc1, 0x0e, 0x2f, 0xf5, 0x2e, 0x7c,
  0xdc, 0x05, 0xba, 0x91, 0x3e, 0xe0, 0x94, 0xd3,
  0x82, 0x2a, 0x25, 0x3c, 0xe1, 0xbb, 0xb4, 0xef,
  0x83, 0x60, 0xef, 0x3e, 0xf0, 0x79
};

char ack_packet_bytes[] = {
  0xd4, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33,
  0xee, 0xee
};

void
send_beacon(struct wlc_info *wlc)
{
    int len = sizeof(beacon) - 32 + beacon.ssid_len;
    sk_buff *p = pkt_buf_get_skb(wlc->osh, len + 202);
    struct beacon *beacon_skb;
    beacon_skb = (struct beacon *) skb_pull(p, 202);
    memcpy(beacon_skb, &beacon, len);
    sendframe(wlc, p, 1, 0);
}

void
send_packet(struct wlc_info *wlc)
{
    int len = sizeof(packet_bytes);
    sk_buff *p = pkt_buf_get_skb(wlc->osh, len + 202);
    char *packet_skb;
    packet_skb = (char *) skb_pull(p, 202);
    memcpy(packet_skb, &packet_bytes, len);
    //sendframe(wlc, p, 1, 0);
    uint32 rate = RATES_RATE_6M;
    sendframe_with_timer(wlc, p, 1, rate, 0, -1, 5);
}

void
send_ack_packet_once(struct wlc_info *wlc, uint32 rate)
{
    set_intioctl(wlc, WLC_SET_LRL, 1);
    set_intioctl(wlc, WLC_SET_SRL, 1);
    exp_set_gains_by_index(wlc->band->pi, 30);

    int len = sizeof(ack_packet_bytes);
    sk_buff *p = pkt_buf_get_skb(wlc->osh, len + 202);
    char *packet_skb;
    packet_skb = (char *) skb_pull(p, 202);
    memcpy(packet_skb, &ack_packet_bytes, len);

    switch (CHSPEC_BW(wlc->band->pi->radio_chanspec)) {
        case WL_CHANSPEC_BW_20:
            rate |= RATES_BW_20MHZ;
            break;
        case WL_CHANSPEC_BW_40:
            rate |= RATES_BW_40MHZ;
            break;
        case WL_CHANSPEC_BW_80:
            rate |= RATES_BW_80MHZ;
            break;
    }

    wlc->band->rspec_override = rate;
    //sendframe_with_timer(struct wlc_info *wlc, struct sk_buff *p, unsigned int fifo, unsigned int rate, int txdelay, int txrepetitions, int txperiodicity)
    sendframe_with_timer(wlc, p, 1, rate, 6, 1, 0);
}


void
send_ack_packet(struct wlc_info *wlc)
{
    set_intioctl(wlc, WLC_SET_LRL, 1);
    set_intioctl(wlc, WLC_SET_SRL, 1);
    exp_set_gains_by_index(wlc->band->pi, 30);

    //int len = sizeof(ack_packet_bytes);
    //int len = sizeof(packet_bytes);
    int len = 100;
    sk_buff *p = pkt_buf_get_skb(wlc->osh, len + 202);
    char *packet_skb;
    packet_skb = (char *) skb_pull(p, 202);
    //memcpy(packet_skb, &ack_packet_bytes, len);
    memcpy(packet_skb, &ack_packet_bytes, len);

    uint32 rate;
    rate = RATES_RATE_54M;
    //rate = RATES_BW_20MHZ | RATES_OVERRIDE_MODE | RATES_ENCODE_HT | RATES_HT_MCS(0);
    //rate = RATES_BW_20MHZ | RATES_OVERRIDE_MODE | RATES_ENCODE_HT | RATES_HT_MCS(0) | RATES_LDPC_CODING;
    //rate = RATES_OVERRIDE_MODE | RATES_ENCODE_VHT | RATES_VHT_MCS(1) | RATES_VHT_NSS(1);

    switch (CHSPEC_BW(wlc->band->pi->radio_chanspec)) {
        case WL_CHANSPEC_BW_20:
            rate |= RATES_BW_20MHZ;
            break;
        case WL_CHANSPEC_BW_40:
            rate |= RATES_BW_40MHZ;
            break;
        case WL_CHANSPEC_BW_80:
            rate |= RATES_BW_80MHZ;
            break;
    }

    wlc->band->rspec_override = rate;
    sendframe(wlc, p, 1, 0);
    //sendframe_with_timer(wlc, p, 1, RATES_RATE_6M, 0, -1, 1);
    //sendframe_with_timer(wlc, p, 1, RATES_RATE_54M, 0, -1, 1);
    //sendframe_with_timer(wlc, p, 1, rate, 0, -1, 1);
}

extern int 
wlc_ioctl_hook(struct wlc_info *wlc, int cmd, char *arg, int len, void *wlc_if);

uint32 timer_counter = 1;

static void
periodically_send_rawack(struct hndrte_timer *t)
{
    wlc_ioctl_hook(t->data, 723, 0, 0, 0);

    if (--timer_counter == 0) {
        hndrte_del_timer(t);
        hndrte_free_timer(t);
        printf("%s: done\n", __FUNCTION__);
    }
}

static void
while_one(struct hndrte_timer *t)
{
    while(1);
}

uint32 callcnt = 0;
uint32 realackrate = 0;

int
wlc_ioctl_7xx(struct wlc_info *wlc, int cmd, char *arg, int len, void *wlc_if)
{
    int ret = IOCTL_ERROR;
    struct phy_info *pi = wlc->band->pi;

    switch (cmd) {

        case 711: // write template ram
        {
            struct phy_info *pi = wlc->hw->band->pi;

            set_scansuppress(wlc, 1);
            set_mpc(wlc, 0);

            set_chanspec(wlc, CH20MHZ_CHSPEC(1));

//            exp_set_gains_by_index(pi, 30);

//            wlc_phy_stay_in_carriersearch_acphy(pi, 1);

            if (len >= 8) {
                int32 *arg32 = (int32 *) arg; // 0: offset, 1: length, 2: data
                printf("offset=%d length=%d data[0]=%08x\n", arg32[0], arg32[1], arg32[3]);
                if (len >= 8 + arg32[1]) {
                    wlc_bmac_write_template_ram(wlc->hw, arg32[0], arg32[1], &arg32[2]);
                    ret = IOCTL_SUCCESS;
                }
            }
        }
        break;

        case 713: // transmit single buffer
        {
            uint32 szbytes;
            uint32 SC_BUFF_LENGTH, SC_BUFF_OFFS, SC_BUFF_START, SC_BUFF_END;
            uint32 phy_ctl;
            struct phy_info *pi = wlc->hw->band->pi;
            volatile struct d11regs *regs = wlc->hw->regs;

            set_scansuppress(wlc, 1);
            set_mpc(wlc, 0);

            int start_offset;
            int num_samps;

            realackrate = 0;
            switch (len) {
                case 16:
                    realackrate = ((uint32 *) arg)[3];
                    printf("realackrate=%08x ", ((uint32 *) arg)[3]);
                case 12:
                    set_chanspec(wlc, ((uint32 *) arg)[0]);
                    start_offset = ((int32 *) arg)[1];
                    num_samps = ((int32 *) arg)[2];
                    printf("chanspec=%08x start_offset=%d num_samps=%d\n", ((uint32 *) arg)[0], ((int32 *) arg)[1], ((int32 *) arg)[2]);
                    break;
                default:
                    set_chanspec(wlc, CH20MHZ_CHSPEC(1));
                    start_offset = 1500;
                    num_samps = 4000;
                    break;
            }

            //uint16 bbmult = 200;
            //wlc_phy_set_tx_bbmult_acphy(pi, &bbmult, 0);

//            wlc_phy_stay_in_carriersearch_acphy(pi, 1);

            exp_set_gains_by_index(pi, 30);

            W_REG(pi->sh->osh, &pi->regs->PHYREF_SamplePlayStartPtr, ((start_offset + 0) & 0xFFFF));
            W_REG(pi->sh->osh, &pi->regs->PHYREF_SamplePlayStopPtr, ((start_offset + num_samps) & 0xFFFF));

            phy_reg_or(pi, ACPHY_macbasedDACPlay(0), 1); // macbasedDACPlay enable
            switch (CHSPEC_BW(wlc->band->pi->radio_chanspec)) {
                case WL_CHANSPEC_BW_20:
                    phy_reg_or(pi, ACPHY_macbasedDACPlay(0), 2); // 20 MHz macBasedDACPlayMode
                    break;
                case WL_CHANSPEC_BW_40:
                    phy_reg_or(pi, ACPHY_macbasedDACPlay(0), 4); // 40 MHz macBasedDACPlayMode
                    break;
                case WL_CHANSPEC_BW_80:
                    phy_reg_or(pi, ACPHY_macbasedDACPlay(0), 6); // 80 MHz macBasedDACPlayMode
                    break;
            }

            wlc_ampdu_tx_set(wlc->ampdu_tx, 0);
            set_intioctl(wlc, WLC_SET_LRL, 1);
            set_intioctl(wlc, WLC_SET_SRL, 1);

            // moved to ioctl 764 for experiment 4
            //wlc_write_shm(wlc, 0x1342, 1); // ENABLE
            wlc_write_shm(wlc, 0x1382, 70); // ACK LENGTH us
            

            // do it in ioctl 723
            //wlc_phy_runsamples_acphy(pi, num_samps, 0xffff, 0, 0, 1);

            // activate macbased transmission
            //printf("phy_ctl1=%08x\n", R_REG(pi->sh->osh, &pi->regs->psm_phy_hdr_param));
//            W_REG(pi->sh->osh, &pi->regs->psm_phy_hdr_param, (1 << 11) | (1 << 1)); // here we loop over the transmission buffer
            // do it in ioctl 723
            //W_REG(pi->sh->osh, &pi->regs->psm_phy_hdr_param, (1 << 11) | (1 << 1) | (1 << 12)); // here we start a single transmission
            //printf("phy_ctl2=%08x\n", R_REG(pi->sh->osh, &pi->regs->psm_phy_hdr_param));

            //wlc_suspend_mac_and_wait(wlc);
            //pi->pi_ac->deaf_count = 10000;
            //wlc_phy_clip_det_acphy(pi, 0);

            ret = IOCTL_SUCCESS;
        }
        break;

        case 726:
        {
            timer_counter = 100000;
            schedule_work(0, wlc, periodically_send_rawack, 1, 1);

            ret = IOCTL_SUCCESS;
        }
        break;

        case 764:
        {
            uint32 dlycnt = 20000;
            while ((wlc_read_shm(wlc, 0x13A0) != 0) && (--dlycnt != 0)) {
                udelay(10);
            }

            if (dlycnt > 0) {
                wlc_write_shm(wlc, 0x13A0, 1);     // [zero => finished | one => working]
                wlc_write_shm(wlc, 0x1394, 1500);  // start addr for start message
                wlc_write_shm(wlc, 0x1396, 7500);  // start addr for data message 1 (bits 01)
                wlc_write_shm(wlc, 0x1398, 10500); // start addr for data message 2 (bits 23)
                wlc_write_shm(wlc, 0x139A, 13500); // start addr for data message 3 (bits 45)
                wlc_write_shm(wlc, 0x139C, 13500); // start addr for data message 4 (bits 67)
                wlc_write_shm(wlc, 0x139E, 4500);  // start addr for stop message
                wlc_write_shm(wlc, 0x1392, 3000);  // common length
                wlc_write_shm(wlc, 0x1390, 1);     // [zero => do nothing | one => start transmit the sequence]
                wlc_write_shm(wlc, 0x1342, 1);     // ENABLE
                ret = IOCTL_SUCCESS;
            } else {
                printf("error timeout 0x13a0=%04x\n", wlc_read_shm(wlc, 0x13A0));
                ret = IOCTL_ERROR;
            }
        }
        break;
    }

    return ret;
}
