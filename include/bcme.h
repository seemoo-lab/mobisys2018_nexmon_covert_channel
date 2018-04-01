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

#ifndef BCME_H
#define BCME_H

#define BCME_OK             0   /* Success */
#define BCME_ERROR          -1  /* Error generic */
#define BCME_BADARG         -2  /* Bad Argument */
#define BCME_BADOPTION          -3  /* Bad option */
#define BCME_NOTUP          -4  /* Not up */
#define BCME_NOTDOWN            -5  /* Not down */
#define BCME_NOTAP          -6  /* Not AP */
#define BCME_NOTSTA         -7  /* Not STA  */
#define BCME_BADKEYIDX          -8  /* BAD Key Index */
#define BCME_RADIOOFF           -9  /* Radio Off */
#define BCME_NOTBANDLOCKED      -10 /* Not  band locked */
#define BCME_NOCLK          -11 /* No Clock */
#define BCME_BADRATESET         -12 /* BAD Rate valueset */
#define BCME_BADBAND            -13 /* BAD Band */
#define BCME_BUFTOOSHORT        -14 /* Buffer too short */
#define BCME_BUFTOOLONG         -15 /* Buffer too long */
#define BCME_BUSY           -16 /* Busy */
#define BCME_NOTASSOCIATED      -17 /* Not Associated */
#define BCME_BADSSIDLEN         -18 /* Bad SSID len */
#define BCME_OUTOFRANGECHAN     -19 /* Out of Range Channel */
#define BCME_BADCHAN            -20 /* Bad Channel */
#define BCME_BADADDR            -21 /* Bad Address */
#define BCME_NORESOURCE         -22 /* Not Enough Resources */
#define BCME_UNSUPPORTED        -23 /* Unsupported */
#define BCME_BADLEN         -24 /* Bad length */
#define BCME_NOTREADY           -25 /* Not Ready */
#define BCME_EPERM          -26 /* Not Permitted */
#define BCME_NOMEM          -27 /* No Memory */
#define BCME_ASSOCIATED         -28 /* Associated */
#define BCME_RANGE          -29 /* Not In Range */
#define BCME_NOTFOUND           -30 /* Not Found */
#define BCME_WME_NOT_ENABLED        -31 /* WME Not Enabled */
#define BCME_TSPEC_NOTFOUND     -32 /* TSPEC Not Found */
#define BCME_ACM_NOTSUPPORTED       -33 /* ACM Not Supported */
#define BCME_NOT_WME_ASSOCIATION    -34 /* Not WME Association */
#define BCME_SDIO_ERROR         -35 /* SDIO Bus Error */
#define BCME_DONGLE_DOWN        -36 /* Dongle Not Accessible */
#define BCME_VERSION            -37     /* Incorrect version */
#define BCME_TXFAIL         -38     /* TX failure */
#define BCME_RXFAIL         -39 /* RX failure */
#define BCME_NODEVICE           -40     /* Device not present */
#define BCME_NMODE_DISABLED     -41     /* NMODE disabled */
#define BCME_NONRESIDENT        -42 /* access to nonresident overlay */
#define BCME_SCANREJECT         -43     /* reject scan request */
/* Leave gap between -44 and -46 to synchronize with trunk. */
#define BCME_DISABLED                   -47     /* Disabled in this build */
#define BCME_LAST           BCME_DISABLED

#endif /* BCME_H */
