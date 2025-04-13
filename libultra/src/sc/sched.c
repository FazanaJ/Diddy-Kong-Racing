/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80079350 */

#include <ultralog.h>
#include <assert.h>
#include <sched.h>
#include "PR/os_thread.h"
#include "PRinternal/osint.h"
#include "src/main.h"

/*
 * private typedefs and defines
 */
#define UNK_MSG         99
#define VIDEO_MSG       666
#define RSP_DONE_MSG    667
#define RDP_DONE_MSG    668
#define PRE_NMI_MSG     669
#define SCHED_THREAD_ID 5

/*
 * OSScTask state
 */
#define OS_SC_DP                0x0001  /* set if still needs dp        */
#define OS_SC_SP                0x0002  /* set if still needs sp        */
#define OS_SC_YIELD             0x0010  /* set if yield requested       */
#define OS_SC_YIELDED           0x0020  /* set if yield completed       */

/*
 * OSScTask->flags type identifier
 */
#define OS_SC_XBUS      (OS_SC_SP | OS_SC_DP)
#define OS_SC_DRAM      (OS_SC_SP | OS_SC_DP | OS_SC_DRAM_DLIST)
#define OS_SC_DP_XBUS   (OS_SC_SP)
#define OS_SC_DP_DRAM   (OS_SC_SP | OS_SC_DRAM_DLIST)
#define OS_SC_SP_XBUS   (OS_SC_DP)
#define OS_SC_SP_DRAM   (OS_SC_DP | OS_SC_DRAM_DLIST)

#ifdef PUPPYPRINT_DEBUG
OSTimer sRSPGfxHangTimer;
OSTimer sRSPAudHangTimer;
OSTimer sRDPHangTimer;
OSTimer sSchedHangTimer;
u8 sTimerChecks[4];
u8 sWroteRDP;
#endif

u8 gSchedFrameCap;

void sched_framecap(s32 cap) {
    if (cap < 0) {
        cap = 0;
    } else if (cap > 2) {
        cap = 2;
    }
    gSchedFrameCap = cap;
}

static void __scTaskComplete(OSSched *sc, OSScTask *t) {
    if (t->list.t.type == M_GFXTASK) {
        if (sc->scheduledFB == NULL) {
            sc->scheduledFB = t->framebuffer;
            osViSwapBuffer(t->framebuffer);
            sc->retraceCount = 0;
        } else {
            sc->queuedFB = t->framebuffer;
        }
        osSendMesg(sc->gfxmq, (OSMesg)OS_SC_DONE_MSG, OS_MESG_NOBLOCK);
    } else {
        osSendMesg(t->msgQ, t->msg, OS_MESG_NOBLOCK);
    }
}

static void __scExec(OSSched *sc, OSScTask *t) {
    if (t->list.t.type == M_AUDTASK) {
#ifdef PUPPYPRINT_DEBUG
        if (sTimerChecks[0] == FALSE && gPlatform & CONSOLE) {
            osSetTimer(&sRSPAudHangTimer, OS_USEC_TO_CYCLES(350000), (OSTime) 0, &gCrashScreen.mesgQueue, (OSMesg) MESG_RSP_AUD_HUNG);
            sTimerChecks[0] = TRUE;
        }
        puppyprint_update_rsp(RSP_AUDIO_START);
#endif
    } else {
#ifdef PUPPYPRINT_DEBUG
        if (sTimerChecks[1] == FALSE && gPlatform & CONSOLE) {
            osSetTimer(&sRSPGfxHangTimer, OS_USEC_TO_CYCLES(300000), (OSTime) 0, &gCrashScreen.mesgQueue, (OSMesg) MESG_RSP_GFX_HUNG);
            sTimerChecks[1] = TRUE;
        }
        puppyprint_update_rsp(RSP_GFX_START);
#endif
    }
    osWritebackDCacheAll();

    t->state &= ~(OS_SC_YIELD | OS_SC_YIELDED);
    osSpTaskLoad(&t->list);
    while (__osSpDeviceBusy());

    IO_WRITE(SP_STATUS_REG, SP_SET_INTR_BREAK | SP_CLR_SSTEP | SP_CLR_BROKE | SP_CLR_HALT);
    

    sc->curRSPTask = t;

    if (t->state & OS_SC_NEEDS_RDP) {
        sc->curRDPTask = t;
#ifdef PUPPYPRINT_DEBUG
        if (sWroteRDP) {
            if (sTimerChecks[2] == FALSE && gPlatform & CONSOLE) {
                osSetTimer(&sRDPHangTimer, OS_USEC_TO_CYCLES(300000), (OSTime) 0, &gCrashScreen.mesgQueue, (OSMesg) MESG_RDP_HUNG);
                sTimerChecks[2] = TRUE;
            }
            IO_WRITE(DPC_STATUS_REG, DPC_CLR_CLOCK_CTR | DPC_CLR_CMD_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_TMEM_CTR);
            sWroteRDP = 0;
        }
#endif
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

extern OSMesgQueue gErrorQueue;

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
    if (sc->retraceCount > gSchedFrameCap && sc->scheduledFB && osViGetCurrentFramebuffer() == sc->scheduledFB) {
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
                //puppyprint_update_rsp(RSP_GFX_PAUSED);
                sc->curRSPTask->state |= OS_SC_YIELD;
                IO_WRITE(SP_STATUS_REG, SP_SET_SIG0);
            }
        }
    }

    __scTryDispatch(sc);
#ifdef PUPPYPRINT_DEBUG
    gSchedStack[0]++;
    gSchedStack[THREAD5_STACK / sizeof(u64) - 1]++;
    if (gSchedStack[THREAD5_STACK / sizeof(u64) - 1] != gSchedStack[0]) {
        puppyprint_assert("Thread 5 Stack overflow");
    }
#endif
}

static void __scHandleRSP(OSSched *sc) {
    OSScTask *t = sc->curRSPTask;
#ifdef PUPPYPRINT_DEBUG
    if (sc->curRSPTask) {
        if (sc->curRSPTask->list.t.type == M_GFXTASK) {
            sTimerChecks[1] = FALSE;
            osStopTimer(&sRSPGfxHangTimer);
            puppyprint_update_rsp(RSP_GFX_FINISHED);
        } else {
            sTimerChecks[0] = FALSE;
            osStopTimer(&sRSPAudHangTimer);
            puppyprint_update_rsp(RSP_AUDIO_FINISHED);
        }
    }
#endif
    sc->curRSPTask = NULL;

    if ((t->state & OS_SC_YIELD) && osSpTaskYielded(&t->list)) {
        t->state |= OS_SC_YIELDED;
        if ((t->flags & OS_SC_RCP_MASK) == OS_SC_XBUS) {
            sc->nextGfxTask2 = sc->nextGfxTask;
            sc->nextGfxTask = t;
            //puppyprint_update_rsp(RSP_GFX_RESUME);
        }
        //sc->curRDPTask = NULL;
    } else {
        t->state &= ~OS_SC_NEEDS_RSP;
        if ((t->state & OS_SC_RCP_MASK) == 0) {
            __scTaskComplete(sc, t);
        }
    }

    __scTryDispatch(sc);
}

static void __scHandleRDP(OSSched *sc) {
    OSScTask *t;
    if (sc->curRDPTask == NULL) {
        __scTryDispatch(sc);
        return;
    }

    t = sc->curRDPTask;
    sc->curRDPTask = NULL;

    t->state &= ~OS_SC_NEEDS_RDP;
#ifdef PUPPYPRINT_DEBUG
    update_rdp_profiling();
    sWroteRDP = 1;
#endif

    if ((t->state & OS_SC_RCP_MASK) == 0) {
#ifdef PUPPYPRINT_DEBUG
        sTimerChecks[2] = FALSE;
        osStopTimer(&sRDPHangTimer);
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
#ifdef PUPPYPRINT_DEBUG
        profiler_snapshot(THREAD5_START);
        if (sTimerChecks[3] == FALSE && gPlatform & CONSOLE) {
            osSetTimer(&sSchedHangTimer, OS_USEC_TO_CYCLES(20000), (OSTime) 0, &gCrashScreen.mesgQueue, (OSMesg) MESG_TASK_FAILED);
            sTimerChecks[3] = TRUE;
        }
#endif
        msg(sc);
#ifdef PUPPYPRINT_DEBUG
        sTimerChecks[3] = FALSE;
        osStopTimer(&sSchedHangTimer);
        profiler_snapshot(THREAD5_END);
#endif
    }
}

//------------------------------------------------------------------------------/
//-- Public functions ---------------------------------------------------------/
//----------------------------------------------------------------------------/

void osScSubmitTask(OSSched *sc, OSScTask *t) {
    OSPri prevpri = __osRunningThread->priority;
    osSetThreadPri(NULL, OS_SC_PRIORITY + 1);

    if (t->list.t.type == M_AUDTASK) {
        t->state = OS_SC_NEEDS_RSP;
        if (sc->nextAudTask == NULL) {
            sc->nextAudTask = t;
        }
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

    osSetThreadPri(NULL, prevpri);
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
#ifdef PUPPYPRINT_DEBUG
    sWroteRDP = 1;
#endif
}