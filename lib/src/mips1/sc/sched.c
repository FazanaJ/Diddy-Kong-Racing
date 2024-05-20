/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80079350 */

#include <assert.h>
#include "sched.h"
#include "types.h"
#include "macros.h"
#include "libultra_internal.h"
#include "viint.h"
#include "main.h"
#include "game.h"
#include "lib/src/os/osint.h"

#ifdef PUPPYPRINT_DEBUG
u32 sRDPCount;
u32 sRSPCount;
#endif

static void __scTaskComplete(OSSched *sc, OSScTask *t) {
    if (t->list.t.type == M_GFXTASK) {
        if (sc->retraceCount > gConfig.frameCap && sc->scheduledFB == NULL) {
            sc->scheduledFB = t->framebuffer;
            osViSwapBuffer(t->framebuffer);
            sc->retraceCount = 0;
        } else {
            sc->queuedFB = t->framebuffer;
        }

        osSendMesg(sc->gfxmq, (OSMesg)OS_SC_DONE_MSG, OS_MESG_NOBLOCK);
        puppyprint_update_rsp(RSP_GFX_FINISHED);
    } else {
        osSendMesg(t->msgQ, t->msg, OS_MESG_NOBLOCK);
        puppyprint_update_rsp(RSP_AUDIO_FINISHED);
    }
}

static void __scExec(OSSched *sc, OSScTask *t) {
    if (t->list.t.type == M_AUDTASK) {
        puppyprint_update_rsp(RSP_AUDIO_START);
    } else {
        puppyprint_update_rsp(RSP_GFX_START);
    }
    osWritebackDCacheAll();

    t->state &= ~(OS_SC_YIELD | OS_SC_YIELDED);
    osSpTaskLoad(&t->list);
    while (__osSpDeviceBusy());

    IO_WRITE(SP_STATUS_REG, SP_SET_INTR_BREAK | SP_CLR_SSTEP | SP_CLR_BROKE | SP_CLR_HALT);
    

    sc->curRSPTask = t;

    if (t->state & OS_SC_NEEDS_RDP) {
        sc->curRDPTask = t;
        IO_WRITE(DPC_STATUS_REG, DPC_CLR_CLOCK_CTR | DPC_CLR_CMD_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_TMEM_CTR);
    }
}

static void __scTryDispatch(OSSched *sc) {
    if (sc->curRSPTask == NULL) {
        if (sc->nextAudTask) {
            OSScTask *t = sc->nextAudTask;
            sc->nextAudTask = NULL;
            __scExec(sc, t);
        } else if ((sc->curRDPTask == NULL || sc->curRDPTask == sc->nextGfxTask) && sc->queuedFB == NULL) {
            OSScTask *t = sc->nextGfxTask;

            if (t) {
                sc->nextGfxTask = sc->nextGfxTask2;
                sc->nextGfxTask2 = NULL;
                __scExec(sc, t);
            }
        }
    }
}

//------------------------------------------------------------------------------/
//-- Event handlers -----------------------------------------------------------/
//----------------------------------------------------------------------------/

static void __scHandlePrenmi(OSSched *sc) {
    osSendMesg(sc->audmq, (OSMesg) &sc->prenmiMsg, OS_MESG_NOBLOCK);
    osSendMesg(sc->gfxmq, (OSMesg) OS_SC_PRE_NMI_MSG, OS_MESG_NOBLOCK);
}

static void __scHandleRetrace(OSSched *sc) {
    UNUSED s32 i;
	sc->retraceCount++;
#ifdef PUPPYPRINT_DEBUG
    if (sc->curRSPTask && sRSPCount++ > 30) {
        puppyprint_assert("RSP has crashed.");
    }
    if (sc->curRDPTask && sRDPCount++ > 30) {
        puppyprint_assert("RDP has crashed.");
    }
#endif
    if (sc->retraceCount > gConfig.frameCap && sc->scheduledFB && osViGetCurrentFramebuffer() == sc->scheduledFB) {
        if (sc->queuedFB) {
            sc->scheduledFB = sc->queuedFB;
            sc->queuedFB = NULL;
            osViSwapBuffer(sc->scheduledFB);
            sc->retraceCount = 0;
        } else {
            sc->scheduledFB = NULL;
        }
    }

    sc->audioFlip ^= 1;

    if (sc->audmq && sc->audioFlip == 0) {
        osSendMesg(sc->audmq, &sc->retraceMsg, OS_MESG_NOBLOCK);

        if (sc->nextAudTask) {
            if (sc->curRSPTask && sc->curRSPTask->list.t.type == M_GFXTASK) {
                puppyprint_update_rsp(RSP_GFX_PAUSED);
                sc->curRSPTask->state |= OS_SC_YIELD;
                IO_WRITE(SP_STATUS_REG, SPSTATUS_SET_SIGNAL0);
            }
        }
    }

    __scTryDispatch(sc);
#ifdef PUPPYPRINT_DEBUG
    gSchedStack[0]++;
    gPokeThread[2] = 0;
    gSchedStack[THREAD5_STACK / sizeof(u64) - 1]++;
    if (gSchedStack[THREAD5_STACK / sizeof(u64) - 1] != gSchedStack[0]) {
        puppyprint_assert("Thread 5 Stack overflow");
    }
    /*for (i = 0; i < 4; i++) {
        gPokeThread[i]++;
        if (gPokeThread[i] > 250000) {
            s32 threadNums[] = {3, 4, 5, 30};
            puppyprint_assert("Thread %d unresponsive", threadNums[i]);
        }
    }*/
#endif
}

static void __scHandleRSP(OSSched *sc) {
    OSScTask *t = sc->curRSPTask;
    sc->curRSPTask = NULL;

    if ((t->state & OS_SC_YIELD) && osSpTaskYielded(&t->list)) {
        t->state |= OS_SC_YIELDED;
        if ((t->flags & OS_SC_RCP_MASK) == OS_SC_XBUS) {
            sc->nextGfxTask2 = sc->nextGfxTask;
            sc->nextGfxTask = t;
            puppyprint_update_rsp(RSP_GFX_RESUME);
        }
        //sc->curRDPTask = NULL;
    } else {
        t->state &= ~OS_SC_NEEDS_RSP;
        if ((t->state & OS_SC_RCP_MASK) == 0) {
#ifdef PUPPYPRINT_DEBUG
            sRSPCount = 0;
#endif
            __scTaskComplete(sc, t);
        }
    }

    __scTryDispatch(sc);
}

static void __scHandleRDP(OSSched *sc) {
    OSScTask *t = sc->curRDPTask;
    sc->curRDPTask = NULL;

    t->state &= ~OS_SC_NEEDS_RDP;
    update_rdp_profiling();

    if ((t->state & OS_SC_RCP_MASK) == 0) {
#ifdef PUPPYPRINT_DEBUG
        sRDPCount = 0;
#endif
        __scTaskComplete(sc, t);
    }

    __scTryDispatch(sc);
}

static void __scMain(void *arg) {
    void (*msg)(OSSched *sc);
    OSSched *sc = (OSSched *)arg;

    while (1) {
        osRecvMesg(&sc->interruptQ, (OSMesg *)&msg, OS_MESG_BLOCK);
        profiler_snapshot(THREAD5_START);
        msg(sc);
        profiler_snapshot(THREAD5_END);
    }
}

//------------------------------------------------------------------------------/
//-- Public functions ---------------------------------------------------------/
//----------------------------------------------------------------------------/

void osScSubmitTask(OSSched *sc, OSScTask *t) {
    OSPri prevpri = __osRunningThread->priority;
    osSetThreadPri(OS_SC_PRIORITY + 1);

    if (t->list.t.type == M_AUDTASK) {
        t->state = OS_SC_NEEDS_RSP;
        sc->nextAudTask = t;
    } else {
        t->state = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP;

        if (sc->curRSPTask == NULL && sc->curRDPTask == NULL && sc->queuedFB == NULL) {
            __scExec(sc, t);
        } else {
            if (sc->nextGfxTask == NULL) {
                sc->nextGfxTask = t;
            } else {
                sc->nextGfxTask2 = t;
            }
        }
    }

    __scTryDispatch(sc);

    osSetThreadPri(prevpri);
}

void osScAddClient(OSSched *sc, UNUSED OSScClient *c, OSMesgQueue *msgQ, u8 id) {
    OSIntMask mask = osSetIntMask(OS_IM_NONE);

    if (id == OS_SC_ID_VIDEO) {
        sc->gfxmq = msgQ;
    } else if (id == OS_SC_ID_AUDIO) {
        sc->audmq = msgQ;
    }

    osSetIntMask(mask);
}

extern OSViMode osViModeNtscLpn1, osViModePalLpn1, osViModeMpalLpn1, osViModePalLan1, osViModeNtscLan1, osViModeMpalLan1;

void osCreateScheduler(OSSched *sc, void *stack, OSPri priority, UNUSED u8 mode, u8 numFields) {
    sc->audmq           = NULL;
    sc->gfxmq           = NULL;
    sc->curRSPTask      = NULL;
    sc->curRDPTask      = NULL;
    sc->nextAudTask     = NULL;
    sc->nextGfxTask     = NULL;
    sc->nextGfxTask2    = NULL;
    sc->scheduledFB     = NULL;
    sc->queuedFB        = NULL;
    sc->retraceMsg.type = OS_SC_RETRACE_MSG;
    sc->prenmiMsg.type  = OS_SC_PRE_NMI_MSG;
    sc->audioFlip       = 0;
    sc->retraceCount    = 0;

#ifdef PUPPYPRINT_DEBUG
    sRDPCount = 0;
    sRSPCount = 0;
#endif

    osCreateViManager(OS_PRIORITY_VIMGR);
    osViBlack(TRUE);
    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, OS_SC_MAX_MESGS);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, OS_SC_MAX_MESGS);
    osSetEventMesg(OS_EVENT_SP, &sc->interruptQ, (OSMesg)&__scHandleRSP);
    osSetEventMesg(OS_EVENT_DP, &sc->interruptQ, (OSMesg)&__scHandleRDP);
    osSetEventMesg(OS_EVENT_PRENMI, &sc->interruptQ, (OSMesg)&__scHandlePrenmi);

    osViSetEvent(&sc->interruptQ, (OSMesg)&__scHandleRetrace, numFields);

    osCreateThread(&sc->thread, 5, __scMain, (void *)sc, stack, priority);
    osStartThread(&sc->thread);
}
