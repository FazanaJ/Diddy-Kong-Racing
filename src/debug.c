#include "main.h"
#include "memory.h"
#include "video.h"
#include "joypad.h"
#include "thread0_epc.h"
#include "font.h"
#include "PR/os_internal_reg.h"
#include "PRinternal/piint.h"
#include "game.h"
#include "objects.h"
#include "tracks.h"

DebugData *gDebug;

void debug_init(void) {
    gDebug = (DebugData *) mempool_alloc(sizeof(DebugData), PP_RAM_DEBUG);
    bzero(gDebug, sizeof(DebugData));
    gDebug->enabled = FALSE;
    gDebug->iter = 0;
    debug_ram(K0_TO_PHYS((u32) &gMainMemoryPool) - (MAIN_POOL_SLOT_COUNT * sizeof(MemoryPoolSlot)), PP_RAM_CODE);
    debug_ram(MAIN_POOL_SLOT_COUNT * sizeof(MemoryPoolSlot), PP_RAM_SLOTS);
    debug_ram(sizeof(DebugData), PP_RAM_DEBUG);
}

typedef char *outfun(char *dst, const char *src, size_t count);

void debug_log(s32 logLevel, char *str, ...) {
}

void debug_dump_hex(u8 *var, s32 size, s32 lineWidth) {
    for (int i = 0; i < size; i++) {
        debug_printf("0x%02X, ", (u32) var[i]);

        if (i && ((i + 1) % lineWidth)== 0) {
            debug_printf("\n");
        }
    }
    debug_printf("\n");
}

void debug_fillrect(Gfx **gfx, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
    s32 alpha = (colour) & 0xFF;
    if (alpha != 255) {
        gDPSetPrimColor((*gfx)++, 0, 0, (colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, alpha);
    }
    gDPPipeSync((*gfx)++);
    gDPSetCombineMode((*gfx)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x2 > SCREEN_WIDTH) {
        x2 = SCREEN_WIDTH;
    }
    if (y2 > SCREEN_HEIGHT) {
        y2 = SCREEN_HEIGHT;
    }
    if (alpha == 255) {
        gDPSetRenderMode((*gfx)++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetCycleType((*gfx)++, G_CYC_FILL);
        gDPSetFillColor((*gfx)++, GPACK_RGBA5551((colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, 1) | 
                       (GPACK_RGBA5551((colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, 1) << 16));
    } else {
        gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
    }
    gDPFillRectangle((*gfx)++, x1, y1, x2, y2);
}

#define RDP_TO_USEC(x) ((x * 10) / 625)

void debug_timer_update(DebugData *d, s32 field, u32 time) {
    s32 i;
    s32 it;
    
    if (d == NULL) {
        return;
    }

    if (time > OS_USEC_TO_CYCLES(99999)) {
        time = OS_USEC_TO_CYCLES(99999);
    }
    //for (i = 0; i < ABS(d->iter - d->prevIter); i++) {
    //    it = d->prevIter + i;
        it = d->iter;
        if (it >= NUM_PERF_ITERATIONS) {
            it -= NUM_PERF_ITERATIONS;
        }
        d->timers[field][it] += time;
        d->timers[field][PERF_AGGREGATE] += d->timers[field][it];
        d->timers[field][PERF_TOTAL] = d->timers[field][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    //}
}

void debug_rdp(void) {
    DebugData *d = gDebug;

    if (d == NULL) {
        return;
    }

    debug_timer_update(d, PP_RDP_CLK, RDP_TO_USEC(IO_READ(DPC_CLOCK_REG)));
    debug_timer_update(d, PP_RDP_BUF, RDP_TO_USEC(IO_READ(DPC_BUFBUSY_REG)));
    debug_timer_update(d, PP_RDP_BUS, RDP_TO_USEC(IO_READ(DPC_PIPEBUSY_REG)));
    debug_timer_update(d, PP_RDP_TMM, RDP_TO_USEC(IO_READ(DPC_TMEM_REG)));
    
    d->rdpTotal = d->timers[PP_RDP_CLK][PERF_TOTAL];
}

void debug_rsp(s32 context) {
    DebugData *d = gDebug;
    u32 time = osGetCount();
    
    if (d == NULL) {
        return;
    }

    switch (context) {
        case RSP_GFX_START:
            d->rspTimers[context][d->rspGfxIter] = time;
            break;
        case RSP_GFX_END:
            d->rspTimers[context][d->rspGfxIter++] = time;
            if (d->rspGfxIter > 3) {
                d->rspGfxIter = 3;
            }
            break;
        case RSP_AUD_START:
            d->rspTimers[context][d->rspAudIter] = time;
            break;
        case RSP_AUD_END:
            d->rspTimers[context][d->rspAudIter++] = time;
            if (d->rspAudIter > 3) {
                d->rspAudIter = 3;
            }
            break;
    }
}

s32 debug_tag_index(s32 colourTag) {
    switch (colourTag) {
        case COLOUR_TAG_RED:
            return PP_RAM_RED;
        case COLOUR_TAG_GREEN:
            return PP_RAM_GREEN;
        case COLOUR_TAG_BLUE:
            return PP_RAM_BLUE;
        case COLOUR_TAG_YELLOW:
            return PP_RAM_YELLOW;
        case COLOUR_TAG_MAGENTA:
            return PP_RAM_MAGENTA;
        case COLOUR_TAG_CYAN:
            return PP_RAM_CYAN;
        case COLOUR_TAG_WHITE:
            return PP_RAM_WHITE;
        case COLOUR_TAG_GREY:
            return PP_RAM_GREY;
        case COLOUR_TAG_SEMITRANS_GREY:
            return PP_RAM_GREY_XLU;
        case COLOUR_TAG_ORANGE:
            return PP_RAM_ORANGE;
        case COLOUR_TAG_BLACK:
            return PP_RAM_BLACK;
        case COLOUR_TAG_LIGHT_ORANGE:
            return PP_RAM_LIGHT_ORANGE;
        case COLOUR_TAG_LIME:
            return PP_RAM_LIME;
        default:
            if (colourTag >= PP_RAM_TOTAL) {
                return PP_RAM_UNKNOWN;
            } else {
                return colourTag % PP_RAM_TOTAL;
            }
    }
}

void debug_ram(s32 size, s32 tag) {
    if (gDebug == NULL) {
        return;
    }
    if (tag != PP_RAM_SUBPOOLS) {
        gDebug->ramTotal += size;
    }
    gDebug->ramSegments[debug_tag_index(tag)] += size;
}

const char *sMinimalText[] = {
    "CPU",
    "RSP",
    "RDP",
    "Frametime"
};

typedef struct ProfilerGraphEntry {
    u32 *ptr;
    u32 colour;
} ProfilerGraphEntry;

typedef struct ProfilerGraph {
    char *name;
    u8 indexCount;
    u8 countType;
    ProfilerGraphEntry entry[4];
} ProfilerGraph;

void debug_graph(DebugData *d, Gfx **dList, s32 x, u32 *timer0, u32 *timer1, u32 *timer2, u32 *timer3, s32 divType, u32 colour0, u32 colour1, u32 colour2, u32 colour3, s32 nameIdx) {
    s32 i;
    f32 divisor = 1.0f;
    s32 iterCount;
    s32 k;
    s32 origin;
    s32 origin2;
    u32 *ref;
    u32 prevColour;
    u32 *idx[4];
    u32 colours[4];
    const s32 num = MIN(NUM_PERF_ITERATIONS, 60);

    idx[0] = timer0;
    idx[1] = timer1;
    idx[2] = timer2;
    idx[3] = timer3;

    colours[0] = colour0;
    colours[1] = colour1;
    colours[2] = colour2;
    colours[3] = colour3;

    iterCount = 0;
    for (i = 0; i < 4; i++) {
        if (idx[i] != NULL) {
            iterCount++;
        }
    }

    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);
    gDPSetRenderMode((*dList)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    origin = x + (((num * 1)) / 2);
    origin2 = x;
    gDPSetCombineMode((*dList)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    // bg
    gDPSetPrimColor((*dList)++, 0, 0, 0, 0, 0, 127);
    gDPFillRectangle((*dList)++, x - 1, SCREEN_HEIGHT - 16 - 54, x + (num * 1) + 1, SCREEN_HEIGHT - 15);
    gDPPipeSync((*dList)++);
    gDPSetRenderMode((*dList)++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    prevColour = 0;
    for (i = 0; i < num; i++) {
        s32 iter = d->iter + i;
        u32 count;
        s32 yT = 0;
        s32 y = 0;
        u32 colour;
        if (iter >= num) {
            iter -= num;
        }

        for (k = 0; k < iterCount; k++) {
            count = *(idx[k] + iter);
            yT += (count / 1536) * divisor;
            if (d->pageViewMode == 2) {
                gDPSetPrimColorRGBA((*dList)++, colours[k]);
                gDPFillRectangle((*dList)++, x, SCREEN_HEIGHT - 16 - (yT), x + 1, SCREEN_HEIGHT - 16 - y);
                y = yT;
            }
        }
        if (d->pageViewMode == 1) {
            if (yT >= 34) {
                colour = 0xFF4040FF;
            } else if (yT >= 23) {
                colour = 0xFFFF40FF;
            } else if (yT >= 12) {
                colour = 0x40FF40FF;
            } else {
                colour = 0x40FFFFFF;
            }
            if (colour != prevColour) {
                gDPSetPrimColorRGBA((*dList)++, colour);
                prevColour = colour;
            }
            gDPFillRectangle((*dList)++, x, SCREEN_HEIGHT - 16 - (yT), x + 1, SCREEN_HEIGHT - 16);
        }
        x += 1;
    }
    gDPSetRenderMode((*dList)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    // 60 line
    gDPSetPrimColor((*dList)++, 0, 0, 64, 255, 255, 160);
    gDPFillRectangle((*dList)++, origin2, SCREEN_HEIGHT - 16 - 11, origin2 + (num * 1), SCREEN_HEIGHT - 16 - 10);
    // 30 line
    gDPSetPrimColor((*dList)++, 0, 0, 64, 255, 64, 160);
    gDPFillRectangle((*dList)++, origin2, SCREEN_HEIGHT - 16 - 22, origin2 + (num * 1), SCREEN_HEIGHT - 16 - 21);
    // bruh line
    gDPSetPrimColor((*dList)++, 0, 0, 192, 192, 192, 112);
    gDPFillRectangle((*dList)++, origin2, SCREEN_HEIGHT - 16 - 44, origin2 + (num * 1), SCREEN_HEIGHT - 16 - 43);
    gDPPipeSync((*dList)++);
    draw_text(dList, origin, SCREEN_HEIGHT - 69, (char *) sMinimalText[nameIdx], ALIGN_TOP_CENTER);
}

void debug_render_minimal(DebugData *d, Gfx **dList, s32 updateRate) {
    char textBytes[32];
    s32 i;
    s32 y;
    u32 *time;
    f32 ram;
    char *ramStr[] = {"KB", "MB"};
    s32 lol;
    u32 ramCount;

    if (d->pageViewMode == 0) {
        debug_fillrect(dList, 8, 8, 112, 60 + 2, 0x0000007F);
        set_text_font(ASSET_FONTS_SMALLFONT);
        set_text_colour(255, 255, 255, 255, 255);
        set_text_background_colour(0, 0, 0, 0);
        set_kerning(FALSE);
        sprintf(textBytes, "FPS: %2.2f", (f64) gFPS);
        draw_text(dList, 10, 10, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "(%2.1fms)", (f64) (1000.0f / gFPS));
        draw_text(dList, 112 - 4, 10, textBytes, ALIGN_TOP_RIGHT);
        y = 20;

        time = &d->cpuTotal;
        for (i = 0; i < 3; i++) {
            sprintf(textBytes, "%s: %d", sMinimalText[i], (u32) *time);
            draw_text(dList, 10, y, textBytes, ALIGN_TOP_LEFT);
            sprintf(textBytes, "(%d%%)", *time / 333);
            draw_text(dList, 112 - 4, y, textBytes, ALIGN_TOP_RIGHT);
            time++;
            y += 10;
        }
        lol = 0;
        if (gUseExpansionMemory) {
            ramCount = 0x800000;
        } else {
            ramCount = 0x400000;
        }
        ram = ((u32) ramCount - gDebug->ramTotal) / 1024.0f;
        if (ram < 0.0f) {
            ram = 0.0f;
        }
        if (ram > 1024.0f) {
            ram /= 1024.0f;
            lol = 1;
        }
        sprintf(textBytes, "RAM: %2.3f%s", (f32) ram, ramStr[lol]);
        draw_text(dList, 10, y, textBytes, ALIGN_TOP_LEFT);
        
    } else {
        debug_graph(d, dList, 16, d->fpsGraph, NULL, NULL, NULL, 0, 0xFF4040FF, 0, 0, 0, 3);
        debug_graph(d, dList, 16 + 76, d->timers[PP_THREAD5], d->timers[PP_THREAD3], d->timers[PP_THREAD4], NULL, 0, 0x40FFFFFF, 0xFF4040FF, 0xFFFF40FF, 0, 0);
        //debug_graph(d, dList, 16 + 76 + 76, d->timers[PP_RSP_GFX], d->timers[PP_RSP_AUD], NULL, NULL, 0, 0xFF4040FF, 0xFFFF40FF, 0, 0, 1);
        debug_graph(d, dList, 16 + 76 + 76 + 76, d->timers[PP_RDP_CLK], d->timers[PP_RDP_TMM], NULL, NULL, 0, 0xFF4040FF, 0x40FFFFFF, 0, 0, 2);
    }
}

extern const char *sMemLabels[];

void debug_render_memory(DebugData *d, Gfx **dList, s32 updateRate) {
    char textBytes[32];
    u32 totalMem;
    s32 memTag;
    s32 y;
    s32 i;

    if (gUseExpansionMemory) {
        totalMem = 0x800000;
    } else {
        totalMem = 0x400000;
    }

    crash_reorder_ram(d);

    debug_fillrect(dList, SCREEN_WIDTH - 136, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000009F);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    sprintf(textBytes, "Total: %2.3f%s", memsize_float(totalMem, &memTag), sMemLabels[memTag]);
    draw_text(dList, SCREEN_WIDTH - 68, 6, textBytes, ALIGN_TOP_CENTER);
    sprintf(textBytes, "Used: %2.3f%s", memsize_float(gDebug->ramTotal, &memTag), sMemLabels[memTag]);
    draw_text(dList, SCREEN_WIDTH - 68, 16, textBytes, ALIGN_TOP_CENTER);
    sprintf(textBytes, "Free: %2.3f%s", memsize_float(totalMem - gDebug->ramTotal, &memTag), sMemLabels[memTag]);
    draw_text(dList, SCREEN_WIDTH - 68, 26, textBytes, ALIGN_TOP_CENTER);
    sprintf(textBytes, "Slots: %d/%d", gMemoryPools[POOL_MAIN].curNumSlots, gMemoryPools[POOL_MAIN].maxNumSlots);
    draw_text(dList, SCREEN_WIDTH - 68, 36, textBytes, ALIGN_TOP_CENTER);

    y = 50 - d->pageScroll;
    gDPSetScissor((*dList)++, G_SC_NON_INTERLACE, SCREEN_WIDTH - 136, 50, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (i = 0; i < PP_RAM_TOTAL; i++) {
        if (y > SCREEN_HEIGHT) {
            break;
        }
        if (y > 50 - 10) {
            sprintf(textBytes, "%s", sPuppyprintMemColours[gCrashMemPrintOrder[i]]);
            draw_text(dList, SCREEN_WIDTH - 136 + 4, y, textBytes, ALIGN_TOP_LEFT);
            sprintf(textBytes, "%2.3f%s", memsize_float(gDebug->ramSegments[gCrashMemPrintOrder[i]], &memTag), sMemLabels[memTag]);
            draw_text(dList, SCREEN_WIDTH - 56, y, textBytes, ALIGN_TOP_LEFT);
        }
        y += 10;
    }
    gDPSetScissor((*dList)++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void debug_render_misc(DebugData *d, Gfx **dList, s32 updateRate) {
    char textBytes[32];
    Object *obj;
    Object_Racer *racer;
    s32 showRacer;

    if (get_game_mode() == GAMEMODE_INGAME) {
        showRacer = TRUE;
        obj = get_racer_object(PLAYER_ONE);
        if (obj == NULL) {
            showRacer = FALSE;
        } else {
            racer = (Object_Racer *) obj->unk64;
        }
    } else {
        showRacer = FALSE;
    }

    if (showRacer == FALSE || d->pageViewMode) {
        debug_fillrect(dList, SCREEN_WIDTH - 96, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000009F);
    }

    if (showRacer) {
        debug_fillrect(dList, 8, 8, 192, 8 + 20 + 4, 0x0000007F);
    }

    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    if (showRacer == FALSE || d->pageViewMode) {
        sprintf(textBytes, "Draw BG: %d", d->misc.drawBG);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 4, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Inverse BG: %d", d->misc.invertBG);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 14, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "AA mode: %d", gAntiAliasing + 1);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 24, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Tex Loads: %d", d->misc.texLoads);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 34, textBytes, ALIGN_TOP_LEFT);

        
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 150, "Loading", ALIGN_TOP_LEFT);
        sprintf(textBytes, "Total: %2.3fs", d->loading.total);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 160, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "DMA: %2.3fs", d->loading.dma);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 170, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Unzip: %2.3fs", d->loading.decompress);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 180, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Malloc: %2.3fs", d->loading.malloc);
        draw_text(dList, SCREEN_WIDTH - 96 + 4, 190, textBytes, ALIGN_TOP_LEFT);
    }

    if (showRacer) {
        sprintf(textBytes, "X: %2.2f", obj->segment.trans.x_position);
        draw_text(dList, 10, 10, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Y: %2.2f", obj->segment.trans.y_position);
        draw_text(dList, 10 + 60, 10, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Z: %2.2f", obj->segment.trans.z_position);
        draw_text(dList, 10 + 120, 10, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "Y: 0x%04X", (u16) obj->segment.trans.rotation.s[0]);
        draw_text(dList, 10, 20, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "P: 0x%04X", (u16) obj->segment.trans.rotation.s[1]);
        draw_text(dList, 10 + 60, 20, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "R: 0x%04X", (u16) obj->segment.trans.rotation.s[2]);
        draw_text(dList, 10 + 120, 20, textBytes, ALIGN_TOP_LEFT);
    }

}

extern s32 *gTextureCache;
extern s32 gNumberOfLoadedTextures;
extern s32 *gSpriteCache;
extern s32 gSpriteCacheCount;
extern s32 *gModelCache;
extern s32 gModelCacheCount;
extern u8 *tex2d_ROM_START[];
extern u8 *tex2d_ROM_END[];
extern u8 *tex3d_ROM_START[];
extern u8 *tex3d_ROM_END[];
extern u8 *sprites_ROM_START[];
extern u8 *sprites_ROM_END[];
extern u8 *objmdl_ROM_START[];
extern u8 *objmdl_ROM_END[];
extern u8 *objanim_ROM_START[];
extern u8 *objanim_ROM_END[];

char sDebugAssetName[32];

char *assettable_name(s32 assetType, s32 assetID) {
    u32 searchAddr;
    char *typeStr;
    s32 dmaCount;

    switch (assetType) {
        case ASSET_TEXTURES_2D:
            searchAddr = (u32) tex2d_ROM_START;
            break;
        case ASSET_TEXTURES_3D:
            searchAddr = (u32) tex3d_ROM_START;
            break;
        case ASSET_SPRITES:
            searchAddr = (u32) sprites_ROM_START;
            break;
        case ASSET_OBJECT_MODELS:
            searchAddr = (u32) objmdl_ROM_START;
            break;
        case ASSET_OBJECT_ANIMATIONS:
            searchAddr = (u32) objanim_ROM_START;
            break;
    }

    searchAddr += (assetID * 32);

    dmacopy(searchAddr, (u32) sDebugAssetName, 32);

    return sDebugAssetName;
}

void debug_render_assets(DebugData *d, Gfx **dList, s32 updateRate) {
    char textBytes[32];
    s32 *table;
    s32 count;
    s32 i;
    s32 y;
    s32 assetID;
    s32 name;
    char *tableName;

    debug_fillrect(dList, SCREEN_WIDTH - 136, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000009F);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    switch (d->pageViewMode) {
        case 0:
            table = gTextureCache;
            count = gNumberOfLoadedTextures;
            name = ASSET_TEXTURES_2D;
            tableName = "Tex 2D";
            break;
        case 1:
            table = gTextureCache;
            count = gNumberOfLoadedTextures;
            name = ASSET_TEXTURES_3D;
            tableName = "Tex 3D";
            break;
        case 2:
            table = gSpriteCache;
            count = gSpriteCacheCount;
            name = ASSET_SPRITES;
            tableName = "Sprites";
            break;
        case 3:
            table = gModelCache;
            count = gModelCacheCount;
            name = ASSET_OBJECT_MODELS;
            tableName = "Models";
            break;
    }

    draw_text(dList, SCREEN_WIDTH - 136 + 4, 5, tableName, ALIGN_TOP_LEFT);
    y = 0;
    for (i = 0; i < count; i++) {
        assetID = table[ASSETCACHE_ID(i)];
        if (assetID != -1) {
            if (d->pageViewMode == 0) {
                if ((assetID & 0x8000) == 0) {
                    y++;
                }
            } else {
                y++;
            }
        }
    }
    sprintf(textBytes, "Loaded: %d", y);
    draw_text(dList, SCREEN_WIDTH - 136 + 4, 15, textBytes, ALIGN_TOP_LEFT);
    y = 30 - d->pageScroll;
    d->pageScrollMax = 0;
    gDPSetScissor((*dList)++, G_SC_NON_INTERLACE, SCREEN_WIDTH - 136, 30, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (i = 0; i < count; i++) {
        assetID = table[ASSETCACHE_ID(i)];
        if (assetID == -1 || (d->pageViewMode == 0 && assetID & 0x8000)) {
            continue;
        }
        assetID &= 0x7FFF;
        d->pageScrollMax += 10;
        if (y > SCREEN_HEIGHT) {
            break;
        }
        if (y > 30 - 10) {
            draw_text(dList, SCREEN_WIDTH - 136 + 4, y, assettable_name(name, assetID), ALIGN_TOP_LEFT);
        }
        y += 10;
    }
    gDPSetScissor((*dList)++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void debug_page_minimal(DebugData *d) {
    
}

void debug_page_memory(DebugData *d) {
}

DebugPage gDebugPages[] = {
    {"Minimal", PAGE_MINIMAL, debug_page_minimal, debug_render_minimal},
    {"Memory", PAGE_MEMORY, debug_page_memory, debug_render_memory},
    {"Misc", PAGE_MISC, debug_page_memory, debug_render_misc},
    {"Assets", PAGE_ASSETS, debug_page_memory, debug_render_assets},
};

void debug_render_page_menu(Gfx **dList, s32 updateRate) {
    DebugData *d = gDebug;
    s32 i;
    s32 y;
    char text[32];
    char *textPtr;

    if (d->pageMenuOpen == FALSE) {
        return;
    }

    debug_fillrect(dList, 10, 68, 112, 68 + 64, 0x0000007F);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    y = 72;
    
    for (i = 0; i < ARRAY_COUNT(gDebugPages); i++) {
        if (i == d->pageSelected) {
            textPtr = " <";
        } else {
            textPtr = " ";
        }
        sprintf(text, "%s%s", gDebugPages[i].name, textPtr);
        draw_text(dList, 14, y, text, ALIGN_TOP_LEFT);
        y += 10;
    }
}

void debug_render(Gfx **dList, s32 updateRate) {
    DebugData *d = gDebug;

    if (d == NULL || d->enabled == FALSE) {
        return;
    }

    gDebugPages[d->pageCurrent].renderFunc(d, dList, updateRate);

    if (d->pageMenuOpen) {
        debug_render_page_menu(dList, updateRate);
    }
}

void debug_thread(s32 field, s32 offset) {
    DebugData *d = gDebug;

    if (d == NULL) {
        return;
    }

    s32 count = field >> 1;
    d->threadTimers[field][d->threadIter[count]] = osGetCount() - offset;
    if (field % 2) {
        d->threadIter[count]++;
        if (d->threadIter[count] > NUM_THREAD_ITERATIONS) {
            d->threadIter[count] = NUM_THREAD_ITERATIONS;
        }
    }
}

s32 debug_thread_compare(DebugData *d, s32 threadID, u32 lowTime, u32 highTime) {
    s32 i;
    s32 j;
    u32 ret;
    s32 count;
    s32 t;
    
    if (threadID <= 0) {
        return 0;
    }

    count = threadID >> 1;


    ret = 0;
    for (i = 0; i < count; i++) {
        t = i << 1;
        for (j = 0; j < d->threadIter[i]; j++) {
            if (lowTime < d->threadTimers[t][j] && highTime > d->threadTimers[t + 1][j]) {
                ret += d->threadTimers[t + 1][j] - d->threadTimers[t][j];
            }
        }
    }

    return ret;
}

void debug_newframe(s32 updateRate) {
    s32 i;
    s32 j;
    s32 it;
    DebugData *d = gDebug;

    //for (j = 0; j < ABS(d->iter - d->prevIter); j++) {
        //it = d->prevIter + j;
        it = d->iter;
        if (it >= NUM_PERF_ITERATIONS) {
            it -= NUM_PERF_ITERATIONS;
        }
        for (i = 0; i < PP_TOTAL; i++) {
            d->timers[i][PERF_AGGREGATE] -= d->timers[i][it];
            d->timers[i][it] = 0;
        }
    //}
    bzero(&d->misc, sizeof(d->misc));
}

char *sMemDumpStrings[] = {
    "",
    "Allocated",
    "Fixed\t",
    "FixedAlloc",
    "4\t",
    "4 Allocated",
    "4 Fixed\t",
    "4 FixedAlloc"
};

char *sPuppyprintMemColours[] = {
    MEMSTRINGS
};

char *debug_asset_lookup(MemoryPoolSlot *slot) {
    s32 i;
    s32 texID;
    s32 tag = slot->colourTag;
    char *str;
    TextureHeader *texHeader;
    Sprite *sprite;
    ObjectHeader *objHeader;
    Object *obj;
    ObjectModel *objModel;
    ModelInstance *objGfx;
    ObjectModel_44 *objAnim;

    str = " ";

    switch (tag) {
        case PP_RAM_OBJHEADERS:
            objHeader = (ObjectHeader *) slot->data;
            return objHeader->internalName;
        case PP_RAM_OBJECTS:
            obj = (Object *) slot->data;
            return obj->segment.header->internalName;
        default:
            return str;
        case PP_RAM_OBJMDL:
            // Try object models
            objModel = (ObjectModel *) slot->data;
            for (i = 0; i < gModelCacheCount; i++) {
                if ((ObjectModel *) gModelCache[ASSETCACHE_PTR(i)] == objModel) {
                    texID = gModelCache[ASSETCACHE_ID(i)];
                }
            }
            if (texID != -200) {
                return assettable_name(ASSET_OBJECT_MODELS, texID);
            }
            return str;
        case PP_RAM_LIME:
            // Okay, lets try for a sprite?
            sprite = (Sprite *) slot->data;
            for (i = 0; i < gSpriteCacheCount; i++) {
                if ((Sprite *) gSpriteCache[ASSETCACHE_PTR(i)] == sprite) {
                    texID = gSpriteCache[ASSETCACHE_ID(i)];
                }
            }
            if (texID != -200) {
                return assettable_name(ASSET_SPRITES, texID);
            }
            return str;
        case PP_RAM_OBJTEX:
        case PP_RAM_LEVELTEX:
        case PP_RAM_MAGENTA:
            texHeader = (TextureHeader *) slot->data;
            texID = -200;
            // First see if it's a texture
            for (i = 0; i < gNumberOfLoadedTextures; i++) {
                if ((TextureHeader *) gTextureCache[ASSETCACHE_PTR(i)] == texHeader) {
                    texID = gTextureCache[ASSETCACHE_ID(i)];
                }
            }
            if (texID != -200) {
                if (texID & 0x8000) {
                    return assettable_name(ASSET_TEXTURES_3D, texID & 0x7FFF);
                } else {
                    return assettable_name(ASSET_TEXTURES_2D, texID);
                }
            }
            // Okay, lets try for a sprite?
            sprite = (Sprite *) slot->data;
            for (i = 0; i < gSpriteCacheCount; i++) {
                if ((Sprite *)  gSpriteCache[ASSETCACHE_PTR(i)] == sprite) {
                    texID = gSpriteCache[ASSETCACHE_ID(i)];
                }
            }
            if (texID != -200) {
                return assettable_name(ASSET_SPRITES, texID);
            }
            return str;
    }
}

void debug_ram_dump(void) {
    int flags;
    int nextIndex;
    int i;
    s32 colourTag;
    MemoryPoolSlot *slot;
    u32 ramTotal = osGetMemSize();

    for (i = 0; i <= gNumberOfMemoryPools; i++) {
        debug_printf("------------- Pool: %d\t Size: %X (%2.3fKiB)\t %2.2f%%\t Slots: %d/%d -------------\n", i, 
        gMemoryPools[i].size, (double) gMemoryPools[i].size / 1024.0, 
        (double) ((f32) gMemoryPools[i].size / (f32) ramTotal) * 100.0, gMemoryPools[i].curNumSlots, gMemoryPools[i].maxNumSlots);
        slot = &gMemoryPools[i].slots[0];
        
        do {
            flags = slot->flags;
            nextIndex = slot->nextIndex;

            colourTag = debug_tag_index(slot->colourTag);

            if (flags == SLOT_FREE) {
                debug_printf("Pool: %d Idx: %d   \t Free Slot\t\t\t\t\t Size: 0x%X\t (%2.3fKiB) \t %2.2f%%\t Addr: %X\n", i, slot->index, slot->size, (double) slot->size / 1024.0, 
                (double) ((f32) slot->size / (f32) ramTotal) * 100.0, slot->data);
            } else {
                debug_printf("Pool: %d Idx: %d   \t %s\t Tag: %s \t\t Size: 0x%X\t (%2.3fKiB) \t %2.2f%% \t Addr: %X\t Name: %s\n", i, slot->index, sMemDumpStrings[flags], 
                sPuppyprintMemColours[colourTag], slot->size, (double) slot->size / 1024.0, (double) ((f32) slot->size / (f32) ramTotal) * 100.0, slot->data, debug_asset_lookup(slot));
            }

            skip:
            if (nextIndex == -1) {
                continue;
            } else {
                slot = &gMemoryPools[i].slots[slot->nextIndex];
            }
        } while (nextIndex != -1);
    }
}

void debug_pause(DebugData *d) {
    switch (d->pageCurrent) {
        case PAGE_MINIMAL:
        case PAGE_BREAKDOWN:
        case PAGE_GENERAL:
        case PAGE_VISCVG:
        case PAGE_AUDIO:
        case PAGE_MISC:
            d->pauseGame = FALSE;
            break;
    }
}

void debug_update(s32 updateRate) {
    s32 i;
    s32 j;
    u32 highTime;
    DebugData *d = gDebug;
    s32 inputPressed;
    s32 inputHeld;
    s32 count;
    s32 offset;

    
    if (d == NULL) {
        return;
    }

    inputPressed = 0;
    inputHeld = 0;
    for (i = 0; i < 4; i++) {
        inputPressed |= input_pressed(i);
        inputHeld |= input_held(i);
    }

    if (inputHeld & U_JPAD && inputPressed & L_TRIG) {
        d->enabled ^= 1;
    } else if (inputPressed & L_TRIG) {
        d->pageMenuOpen ^= 1;
        d->pageCurrent = d->pageSelected;
    }
    if (d->pageMenuOpen == FALSE) {
        switch (d->pageCurrent) {
            case PAGE_MINIMAL:
                if (inputPressed & R_JPAD) {
                    d->pageViewMode++;
                    if (d->pageViewMode == 3) {
                        d->pageViewMode = 0;
                    }
                } else if (inputPressed & L_JPAD) {
                    d->pageViewMode--;
                    if (d->pageViewMode == 255) {
                        d->pageViewMode = 2;
                    }
                }
                break;
            case PAGE_MISC:
                if (inputPressed & R_JPAD || inputPressed & L_JPAD) {
                    d->pageViewMode ^= 1;
                }
                break;
            case PAGE_ASSETS:
                if (inputPressed & R_JPAD) {
                    d->pageViewMode++;
                    d->pageScroll = 0;
                    if (d->pageViewMode == 4) {
                        d->pageViewMode = 0;
                    }
                } else if (inputPressed & L_JPAD) {
                    d->pageViewMode--;
                    d->pageScroll = 0;
                    if (d->pageViewMode == 255) {
                        d->pageViewMode = 3;
                    }
                }
                if (inputHeld & U_JPAD) {
                    d->pageScroll -= 4 * updateRate;
                } else if (inputHeld & D_JPAD) {
                    d->pageScroll += 4 * updateRate;
                }
                if (d->pageScroll > (d->pageScrollMax) - (SCREEN_HEIGHT - 30) + 4) {
                    d->pageScroll = (d->pageScrollMax) - (SCREEN_HEIGHT - 30) + 4;
                }
                if (d->pageScroll < 0) {
                    d->pageScroll = 0;
                }
                break;
            case PAGE_MEMORY:
                if (inputHeld & U_JPAD) {
                    d->pageScroll -= 2 * updateRate;
                    if (d->pageScroll < 0) {
                        d->pageScroll = 0;
                    }
                } else if (inputHeld & D_JPAD) {
                    d->pageScroll += 2 * updateRate;
                }
                if (inputPressed & R_JPAD) {
                    debug_ram_dump();
                }
                if (d->pageScroll > (PP_RAM_TOTAL * 10) - (SCREEN_HEIGHT - 50) + 4) {
                    d->pageScroll = (PP_RAM_TOTAL * 10) - (SCREEN_HEIGHT - 50) + 4;
                }
                break;
        }
    } else {
        if (inputPressed & U_JPAD) {
            d->pageSelected--;
            if (d->pageSelected == 255) {
                d->pageSelected = ARRAY_COUNT(gDebugPages) - 1;
            }
        } else if (inputPressed & D_JPAD) {
            d->pageSelected++;
            if (d->pageSelected >= ARRAY_COUNT(gDebugPages)) {
                d->pageSelected = 0;
            }
        }
        if (d->pageCurrent != d->pagePrev) {
            d->pagePrev = d->pageCurrent;
            d->pageViewMode = 0;
            d->pageScroll = 0;
            debug_pause(d);
        }
    }

    d->cpuTotal = 0;
    for (i = 0; i < THREAD_CONTEXT_COUNT; i += 2) {
        highTime = 0;
        count = i >> 1;
        for (j = 0; j < d->threadIter[count]; j++) {
            highTime += OS_CYCLES_TO_USEC(d->threadTimers[i + 1][j] - d->threadTimers[i][j]);
            offset = OS_CYCLES_TO_USEC(debug_thread_compare(d, i, d->threadTimers[i][j], d->threadTimers[i + 1][j]));
            highTime -= offset;
            if (i == THREAD3_START) {
                debug_timer_update(d, PP_YIELD3, offset);
            }
        }
        debug_timer_update(d, PP_THREAD5 + count, highTime);
        d->timers[PP_THREAD5 + count][PERF_TOTAL] = d->timers[PP_THREAD5 + count][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
        d->cpuTotal += d->timers[PP_THREAD5 + count][PERF_TOTAL];
    }
    
    bzero(&d->threadIter, THREAD_CONTEXT_COUNT);
    d->timers[PP_YIELD3][PERF_TOTAL] = d->timers[PP_YIELD3][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;

    for (i = 0; i < d->rspGfxIter; i++) {
        debug_timer_update(d, PP_RSP_GFX, OS_CYCLES_TO_USEC(d->rspTimers[RSP_GFX_END][i] - d->rspTimers[RSP_GFX_START][i]));
    }
    for (i = 0; i < d->rspAudIter; i++) {
        debug_timer_update(d, PP_RSP_AUD, OS_CYCLES_TO_USEC(d->rspTimers[RSP_AUD_END][i] - d->rspTimers[RSP_AUD_START][i]));
    }
    d->rspGfxIter = 0;
    d->rspAudIter = 0;
    d->timers[PP_RSP_GFX][PERF_TOTAL] = d->timers[PP_RSP_GFX][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    // Temp measure until I figure out why aud profiles are messing up
    d->timers[PP_RSP_AUD][PERF_AGGREGATE] = 0;
    for (i = 0; i < NUM_PERF_ITERATIONS; i++) {
        d->timers[PP_RSP_AUD][PERF_AGGREGATE] += d->timers[PP_RSP_AUD][i];
    }
    d->timers[PP_RSP_AUD][PERF_TOTAL] = d->timers[PP_RSP_AUD][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    d->rspTotal = d->timers[PP_RSP_GFX][PERF_TOTAL];// + d->timers[PP_RSP_AUD][PERF_TOTAL];

    d->prevIter = d->iter;
    //d->iter += updateRate;
    d->iter++;

    if (d->iter >= NUM_PERF_ITERATIONS) {
        d->iter -= NUM_PERF_ITERATIONS;
    }
    debug_newframe(updateRate);
}

#include "usb/dkr_usb.c"
#include "usb/usb.c"
#include "usb/reset.c"
