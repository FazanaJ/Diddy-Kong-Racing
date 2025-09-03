
/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1994, Silicon Graphics, Inc.               *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright  law.  They  may not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *                                                                        *
 *************************************************************************/

/**************************************************************************
 *
 *  $Revision: 1.10 $
 *  $Date: 1997/02/11 08:37:33 $
 *  $Source: /exdisk2/cvs/N64OS/Master/cvsmdev2/PR/include/ultra64.h,v $
 *
 **************************************************************************/

#ifndef _ULTRA64_H_
#define _ULTRA64_H_

#include <PR/ultratypes.h>
#include <PR/rcp.h>
#include <PR/os.h>
#include <PR/region.h>
#include <PR/rmon.h>
#include <PR/sptask.h>
#include <PR/mbi.h>
#include <PR/libaudio.h>
#include <PR/gu.h>
#include <PR/ramrom.h>
#if 0
/* This file fails in DKR because of the same named sprite structs. */
#include <PR/sp.h>
#endif
#include <PR/ucode.h>
#include <PR/ultraerror.h>
#include <PR/ultralog.h>
#include "PR/os_libc.h"
#include "overlays.h"

extern u32 __osBbEepromAddress;
extern u32 __osBbEepromSize;
extern u32 __osBbFlashAddress;
extern u32 __osBbFlashSize;
extern u32 __osBbSramAddress;
extern u32 __osBbSramSize;
extern u32 __osBbPakAddress[4];
extern u32 __osBbPakSize;
extern u32 __osBbIsBb;
extern u32 __osBbHackFlags;
extern u32 __osBbStashMagic;
extern u32 __osBbPakBindings[4];
extern char __osBbStateName[10];
extern u32 __osBbStateDirty;
extern u32 __osBbAuxDataLimit;

#endif
