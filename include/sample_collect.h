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
 * but WITHOUT ANY WARRANTY\n"
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with NexMon. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                         *
 **************************************************************************/

#ifndef SAMPLE_COLLECT_H
#define SAMPLE_COLLECT_H

#include <structs.h>            // structures that are used by the code in the firmware

#define TRIGGER_NOW                 0
#define TRIGGER_CRS                 0x01
#define TRIGGER_CRSDEASSERT         0x02
#define TRIGGER_GOODFCS             0x04
#define TRIGGER_BADFCS              0x08
#define TRIGGER_BADPLCP             0x10
#define TRIGGER_CRSGLITCH           0x20
#define WL_ACI_ARGS_LEGACY_LENGTH   16  /* bytes of pre NPHY aci args */
#define WL_SAMPLECOLLECT_T_VERSION  2   /* version of wl_samplecollect_args_t struct */
typedef struct wl_samplecollect_args {
    /* version 0 fields */
    uint8 coll_us;
    int cores;
    /* add'l version 1 fields */
    uint16 version;     /* see definition of WL_SAMPLECOLLECT_T_VERSION */
    uint16 length;      /* length of entire structure */
    int8 trigger;
    uint16 timeout;
    uint16 mode;
    uint32 pre_dur;
    uint32 post_dur;
    uint8 gpio_sel;
    bool downsamp;
    bool be_deaf;
    bool agc;       /* loop from init gain and going down */
    bool filter;        /* override high pass corners to lowest */
    /* add'l version 2 fields */
    uint8 trigger_state;
    uint8 module_sel1;
    uint8 module_sel2;
    uint16 nsamps;
    int bitStart;
    uint32 gpioCapMask;
} wl_samplecollect_args_t;

#define WL_SAMPLEDATA_HEADER_TYPE   1
#define WL_SAMPLEDATA_HEADER_SIZE   80  /* sample collect header size (bytes) */
#define WL_SAMPLEDATA_TYPE      2
#define WL_SAMPLEDATA_SEQ       0xff    /* sequence # */
#define WL_SAMPLEDATA_MORE_DATA     0x100   /* more data mask */
#define WL_SAMPLEDATA_T_VERSION     1   /* version of wl_samplecollect_args_t struct */
/* version for unpacked sample data, int16 {(I,Q),Core(0..N)} */
#define WL_SAMPLEDATA_T_VERSION_SPEC_AN 2

typedef struct wl_sampledata {
    uint16 version; /* structure version */
    uint16 size;    /* size of structure */
    uint16 tag; /* Header/Data */
    uint16 length;  /* data length */
    uint32 flag;    /* bit def */
} wl_sampledata_t;

#define SC_MODE_0_sd_adc        0
#define SC_MODE_1_sd_adc_5bits          1
#define SC_MODE_2_cic0              2
#define SC_MODE_3_cic1              3
#define SC_MODE_4s_rx_farrow_1core      4
#define SC_MODE_4m_rx_farrow        5
#define SC_MODE_5_iq_comp           6
#define SC_MODE_6_dc_filt           7
#define SC_MODE_7_rx_filt           8
#define SC_MODE_8_rssi              9
#define SC_MODE_9_rssi_all          10
#define SC_MODE_10_tx_farrow        11
#define SC_MODE_11_gpio             12
#define SC_MODE_12_gpio_trans       13
#define SC_MODE_14_spect_ana        14
#define SC_MODE_5s_iq_comp          15
#define SC_MODE_6s_dc_filt          16
#define SC_MODE_7s_rx_filt          17

#define PHYREF_SampleCollectStartPtr        u.d11acregs.SampleCollectStartPtr
#define PHYREF_SampleCollectStopPtr         u.d11acregs.SampleCollectStopPtr
#define PHYREF_SampleCollectCurPtr          u.d11acregs.SampleCollectCurPtr
#define PHYREF_SaveRestoreStartPtr          u.d11acregs.SaveRestoreStartPtr
#define PHYREF_SampleCollectPlayPtrHigh     u.d11acregs.SampleCollectPlayPtrHigh
#define PHYREF_SampleCollectCurPtrHigh      u.d11acregs.SampleCollectCurPtrHigh
#define PHYREF_SamplePlayStartPtr           u.d11acregs.SamplePlayStartPtr
#define PHYREF_SamplePlayStopPtr            u.d11acregs.SamplePlayStopPtr

#define PHYREF_XMTSEL                  u.d11regs.xmtsel
#define PHYREF_WEPCTL                  u.d11regs.wepctl
#define PHYREF_XMTTPLATETXPTR          u.d11regs.xmttplatetxptr
#define PHYREF_XMTTXCNT                u.d11regs.xmttxcnt
#define PHYREF_IFSSTAT                 u.d11regs.ifsstat
#define PHYREF_BTCX_STAT               u.d11regs.btcx_stat
#define PHYREF_BTCX_CTRL               u.d11regs.btcx_ctrl
#define PHYREF_BTCX_TRANS_CTRL         u.d11regs.btcx_trans_ctrl
#define PHYREF_BTCX_ECI_ADDR           u.d11regs.btcx_eci_addr
#define PHYREF_BTCX_ECI_DATA           u.d11regs.btcx_eci_data
#define PHYREF_SMPL_CLCT_STRPTR        u.d11regs.smpl_clct_strptr
#define PHYREF_SMPL_CLCT_STPPTR        u.d11regs.smpl_clct_stpptr
#define PHYREF_SMPL_CLCT_CURPTR        u.d11regs.smpl_clct_curptr
#define PHYREF_TSF_CLK_FRAC_L          u.d11regs.tsf_clk_frac_l
#define PHYREF_TSF_CLK_FRAC_H          u.d11regs.tsf_clk_frac_h
#define PHYREF_TSF_GPT2_CTR_L          u.d11regs.tsf_gpt2_ctr_l
#define PHYREF_TSF_GPT2_CTR_H          u.d11regs.tsf_gpt2_ctr_h
#define PHYREF_IFS_CTL_SEL_PRICRS      u.d11regs.ifs_ctl_sel_pricrs

int wlc_phy_chan2fc_acphy(unsigned int channel);
uint16 acphy_words_per_us(struct phy_info *pi, uint16 sd_adc_rate, uint16 mo);
void acphy_set_sc_startptr(struct phy_info *pi, uint32 start_idx);
uint32 acphy_get_sc_startptr(struct phy_info *pi);
void acphy_set_sc_stopptr(struct phy_info *pi, uint32 stop_idx);
uint32 acphy_get_sc_stopptr(struct phy_info *pi);
uint32 acphy_get_sc_curptr(struct phy_info *pi);
uint32 acphy_is_sc_done(struct phy_info *pi);
int acphy_sc_length(struct phy_info *pi);
int wlc_phy_sample_collect_acphy(struct phy_info *pi, struct wl_samplecollect_args *collect, uint32 *buf);
int wlc_phy_sample_collect(struct phy_info *pi, struct wl_samplecollect_args *collect, void *buf);
int wlc_phy_sample_data_acphy(struct phy_info *pi, wl_sampledata_t *sample_data, void *b);
int wlc_phy_sample_data(struct phy_info *pi, wl_sampledata_t *sample_data, void *b);

#endif /* SAMPLE_COLLECT_H */
