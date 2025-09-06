#include "PRinternal/macros.h"
#include "PR/os_internal.h"
#include "PRinternal/siint.h"
#include "PRinternal/controller.h"

s32 osPfsInit(OSMesgQueue* queue, OSPfs* pfs, int channel) {
    s32 ret = 0;

    __osSiGetAccess();
    ret = __osPfsGetStatus(queue, channel);
    __osSiRelAccess();

    if (ret != 0) {
        return ret;
    }

    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;
    ERRCK(__osGetId(pfs));

    ret = osPfsChecker(pfs);
    pfs->status |= PFS_INITIALIZED;
    return ret;
}

#define ROUND_UP_DIVIDE(numerator, denominator) (((numerator) + (denominator)-1) / (denominator))

#if BUILD_VERSION >= VERSION_J

s32 osPfsAllocateFile(OSPfs* pfs, u16 company_code, u32 game_code, u8* game_name, u8* ext_name, int file_size_in_bytes,
                      s32* file_no) {
    int start_page;
    int decleared;
    int last_page;
    int old_last_page = 0;
    s32 ret = 0;
    int file_size_in_pages;
    __OSInode inode;
    __OSInode backup_inode;
    __OSDir dir;
    u8 bank;
    u8 old_bank = 0;
    int firsttime = 0;
    s32 bytes;
    __OSInodeUnit fpage;

    if (company_code == 0 || game_code == 0) {
        return PFS_ERR_INVALID;
    }

    file_size_in_pages = ROUND_UP_DIVIDE(file_size_in_bytes, BLOCKSIZE * PFS_ONE_PAGE);

    if (((ret = osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, file_no)) != 0) &&
        ret != PFS_ERR_INVALID) {
        return ret;
    }

    if (*file_no != -1) {
        return PFS_ERR_EXIST;
    }

    ret = osPfsFreeBlocks(pfs, &bytes);

    if (file_size_in_bytes > bytes) {
        return PFS_DATA_FULL;
    }

    if (file_size_in_pages == 0) {
        return PFS_ERR_INVALID;
    }

    if (((ret = osPfsFindFile(pfs, 0, 0, NULL, NULL, file_no)) != 0) && ret != PFS_ERR_INVALID) {
        return ret;
    }

    if (*file_no == -1) {
        return PFS_DIR_FULL;
    }

    for (bank = 0; bank < pfs->banks; bank++) {
        ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
        ERRCK(__osPfsDeclearPage(pfs, &inode, file_size_in_pages, &start_page, bank, &decleared, &last_page));

        if (start_page != -1) {
            if (firsttime == 0) {
                fpage.inode_t.page = start_page;
                fpage.inode_t.bank = bank;
            } else {
                backup_inode.inode_page[old_last_page].inode_t.bank = bank;
                backup_inode.inode_page[old_last_page].inode_t.page = start_page;
                ERRCK(__osPfsRWInode(pfs, &backup_inode, PFS_WRITE, old_bank));
            }

            if (file_size_in_pages > decleared) {
                wcopy(&inode, &backup_inode, sizeof(__OSInode));
                old_last_page = last_page;
                old_bank = bank;
                file_size_in_pages -= decleared;
                firsttime++;
            } else {
                file_size_in_pages = 0;
                ERRCK(__osPfsRWInode(pfs, &inode, PFS_WRITE, bank));
                break;
            }
        }
    }

    if (file_size_in_pages > 0 || start_page == -1) {
        return PFS_ERR_INCONSISTENT;
    }

    dir.start_page = fpage;
    dir.company_code = company_code;
    dir.game_code = game_code;
    dir.data_sum = 0;

    wcopy(game_name, dir.game_name, PFS_FILE_NAME_LEN);
    wcopy(ext_name, dir.ext_name, PFS_FILE_EXT_LEN);

    ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + *file_no, (u8*)&dir, FALSE);
    return ret;
}

s32 __osPfsDeclearPage(OSPfs* pfs, __OSInode* inode, int file_size_in_pages, int* first_page, u8 bank, int* decleared,
                       int* last_page) {
    int j;
    int spage;
    int old_page;
    s32 ret = 0;
    int offset = bank > 0 ? 1 : pfs->inode_start_page;

    for (j = offset; j < ARRLEN(inode->inode_page); j++) {
        if (inode->inode_page[j].ipage == 3) {
            break;
        }
    }

    if (j == ARRLEN(inode->inode_page)) {
        *first_page = -1;
        return ret;
    }

    spage = j;
    *decleared = 1;
    old_page = j;
    j++;

    while (file_size_in_pages > *decleared && j < ARRLEN(inode->inode_page)) {
        if (inode->inode_page[j].ipage == 3) {
            inode->inode_page[old_page].inode_t.bank = bank;
            inode->inode_page[old_page].inode_t.page = j;
            old_page = j;
            (*decleared)++;
        }
        j++;
    }

    *first_page = spage;

    if (j == ARRLEN(inode->inode_page) && file_size_in_pages > *decleared) {
        *last_page = old_page;
    } else {
        inode->inode_page[old_page].ipage = 1;
        *last_page = 0;
    }

    return ret;
}

#else

static s32 __osClearPage(OSPfs *pfs, int page_num, u8 *data, u8 bank);

s32 osPfsAllocateFile(OSPfs* pfs, u16 company_code, u32 game_code, u8* game_name, u8* ext_name, int file_size_in_bytes,
                      s32* file_no) {
    int start_page;
    int decleared;
    int last_page;
    int old_last_page = 0;
    int j;
    s32 ret = 0;
    int file_size_in_pages;
    __OSInode inode;
    __OSInode backup_inode;
    __OSDir dir;
    u8 bank;
    u8 old_bank = 0;
    int firsttime = 0;
    s32 bytes;
    __OSInodeUnit fpage;

    if (company_code == 0 || game_code == 0) {
        return PFS_ERR_INVALID;
    }

    file_size_in_pages = ROUND_UP_DIVIDE(file_size_in_bytes, BLOCKSIZE * PFS_ONE_PAGE);

    if ((pfs->status & PFS_INITIALIZED) == FALSE) {
        return PFS_ERR_INVALID;
    }

    PFS_CHECK_ID();

    if (((ret = osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, file_no)) != 0) &&
        ret != PFS_ERR_INVALID) {
        return ret;
    }

    if (*file_no != -1) {
        return PFS_ERR_EXIST;
    }

    ret = osPfsFreeBlocks(pfs, &bytes);

    if (file_size_in_bytes > bytes) {
        return PFS_DATA_FULL;
    }

    if (file_size_in_pages != 0) {

        if (((ret = osPfsFindFile(pfs, 0, 0, NULL, NULL, file_no)) != 0) && ret != PFS_ERR_INVALID) {
            return ret;
        }

        if (*file_no == -1) {
            return PFS_DIR_FULL;
        }

        for (bank = 0; bank < pfs->banks; bank++) {
            ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
            ERRCK(__osPfsDeclearPage(pfs, &inode, file_size_in_pages, &start_page, bank, &decleared, &last_page));

            if (start_page != -1) {
                if (firsttime == 0) {
                    fpage.inode_t.page = start_page;
                    fpage.inode_t.bank = bank;
                } else {
                    backup_inode.inode_page[old_last_page].inode_t.bank = bank;
                    backup_inode.inode_page[old_last_page].inode_t.page = start_page;
                    ERRCK(__osPfsRWInode(pfs, &backup_inode, PFS_WRITE, old_bank));
                }

                for (j = 0; j < ARRLEN(inode.inode_page); j++) {
                    backup_inode.inode_page[j].ipage = inode.inode_page[j].ipage;
                }
                old_last_page = last_page;
                old_bank = bank;
                firsttime++;
                if (file_size_in_pages > decleared) {
                    file_size_in_pages = file_size_in_pages - decleared;
                } else {
                    file_size_in_pages = 0;
                    break;
                }
            }
        }

        if (file_size_in_pages > 0 || start_page == -1) {
            return PFS_ERR_INCONSISTENT;
        }
        
        backup_inode.inode_page[old_last_page].inode_t.bank = bank;
        backup_inode.inode_page[old_last_page].inode_t.page = start_page;
        ERRCK(__osPfsRWInode(pfs, &backup_inode, PFS_WRITE, old_bank));

        dir.start_page = fpage;
        dir.company_code = company_code;
        dir.game_code = game_code;
        dir.data_sum = 0;

        for (j = 0; j < ARRLEN(dir.game_name); j++)
            dir.game_name[j] = *game_name++;
        for (j = 0; j < ARRLEN(dir.ext_name); j++)
            dir.ext_name[j] = *ext_name++;

        ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + *file_no, (u8*)&dir, FALSE));
        return ret;
    } else {
        return PFS_ERR_INVALID;
    }
}

s32 __osPfsDeclearPage(OSPfs* pfs, __OSInode* inode, int file_size_in_pages, int* first_page, u8 bank, int* decleared,
                       int* last_page) {
    int j;
    int spage;
    int old_page;
    u8 tmp_data[BLOCKSIZE];
    int i;
    s32 ret = 0;
    int offset = bank > 0 ? 1 : pfs->inode_start_page;

    for (j = offset; j < ARRLEN(inode->inode_page); j++) {
        if (inode->inode_page[j].ipage == 3) {
            break;
        }
    }

    if (j == ARRLEN(inode->inode_page)) {
        *first_page = -1;
        return ret;
    }
    
    for (i = 0; i < ARRLEN(tmp_data); i++) {
        tmp_data[i] = 0;
    }

    spage = j;
    *decleared = 1;
    old_page = j;
    j++;

    while (file_size_in_pages > *decleared && j < ARRLEN(inode->inode_page)) {
        if (inode->inode_page[j].ipage == 3) {
            inode->inode_page[old_page].inode_t.bank = bank;
            inode->inode_page[old_page].inode_t.page = j;
            ERRCK(__osClearPage(pfs, old_page, (u8*)tmp_data, bank));
            old_page = j;
            (*decleared)++;
        }
        j++;
    }

    *first_page = spage;

    if (j == ARRLEN(inode->inode_page) && file_size_in_pages > *decleared) {
        *last_page = old_page;
        return ret;
    } else {
        inode->inode_page[old_page].ipage = 1;
        ret = __osClearPage(pfs, old_page, (u8*)tmp_data, bank);
        *last_page = 0;
        return ret;
    }
}

static s32 __osClearPage(OSPfs *pfs, int page_no, u8 *data, u8 bank)
{
    int i;
    s32 ret;
    ret = 0;
    pfs->activebank = bank;
    ERRCK(__osPfsSelectBank(pfs));
    for (i = 0; i < PFS_ONE_PAGE; i++) {
        ret = __osContRamWrite(pfs->queue, pfs->channel, page_no * PFS_ONE_PAGE + i, data, FALSE);
        if (ret != 0) {
            break;
        }
    }
    pfs->activebank = 0;
    ret = __osPfsSelectBank(pfs);
    return ret;
}

#endif

#ifdef _DEBUG
s32 __osDumpInode(OSPfs* pfs) {
    int j;
    __OSInode inode;
    s32 ret = 0;
    __OSDir dir;
    u8 bank;
#if BUILD_VERSION >= VERSION_J
    u8 startbank = PFS_ID_BANK_256K;
#endif

    rmonPrintf("INODE:\n");

#if BUILD_VERSION >= VERSION_J
    for (bank = startbank; bank < pfs->banks; bank++) {
#else
    for (bank = 0; bank < pfs->banks; bank++) {
#endif
        rmonPrintf("\nBank %d:\n", bank);
        ret = __osPfsRWInode(pfs, &inode, PFS_READ, bank);
        for (j = 0; j < PFS_INODE_SIZE_PER_PAGE; j++) {
            rmonPrintf("%x ", inode.inode_page[j].ipage);
        }
    }

    rmonPrintf("dir_size %d %d\n", pfs->dir_size, pfs->inode_start_page);

    for (j = 0; j < pfs->dir_size; j++) {
        __osContRamRead(pfs->queue, pfs->channel, (u16)(pfs->dir_table + (int)j), (u8*)&dir);
        rmonPrintf("file %d game_code %d page %x c_code %d sum %d\n", j, dir.game_code, dir.start_page.ipage,
                   dir.company_code, dir.data_sum);
    }
    rmonPrintf("End of Dump\n");
    return ret;
}
#endif

s32 corrupted_init(OSPfs* pfs, __OSInodeCache* cache);
s32 corrupted(OSPfs* pfs, __OSInodeUnit fpage, __OSInodeCache* cache);

#define CHECK_IPAGE(p)                                                                                        \
    (((p).ipage >= pfs->inode_start_page) && ((p).inode_t.bank < pfs->banks) && ((p).inode_t.page >= 0x01) && \
     ((p).inode_t.page < 0x80))

s32 osPfsChecker(OSPfs* pfs) {
    int j;
    s32 ret;
    __OSInodeUnit next_page;
    __OSInode checked_inode;
    __OSInode tmp_inode;
    __OSDir tmp_dir;
    __OSInodeUnit file_next_node[16];
    __OSInodeCache cache;
    int fixed = 0;
    u8 bank;
#if BUILD_VERSION >= VERSION_J
    u8 oldbank = 254;
#endif
    s32 cc;
    s32 cl;
    int offset;

    ret = __osCheckId(pfs);

    if (ret == PFS_ERR_NEW_PACK) {
        ret = __osGetId(pfs);
    }

    if (ret != 0) {
        return ret;
    }

    ERRCK(corrupted_init(pfs, &cache));

    for (j = 0; j < pfs->dir_size; j++) {
        ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tmp_dir));

#if BUILD_VERSION >= VERSION_J
        if (tmp_dir.company_code != 0 || tmp_dir.game_code != 0) {
            if (tmp_dir.company_code == 0 || tmp_dir.game_code == 0) {
                cc = -1;
            } else {
                next_page = tmp_dir.start_page;
                cl = cc = 0;
                bank = 255;

                while (CHECK_IPAGE(next_page)) {
                    if (bank != next_page.inode_t.bank) {
                        bank = next_page.inode_t.bank;

                        if (oldbank != bank) {
                            ret = __osPfsRWInode(pfs, &tmp_inode, PFS_READ, bank);
                            oldbank = bank;
                        }

                        if (ret != 0 && ret != PFS_ERR_INCONSISTENT) {
                            return ret;
                        }
                    }

                    if ((cc = corrupted(pfs, next_page, &cache) - cl) != 0) {
                        break;
                    }

                    cl = 1;
                    next_page = tmp_inode.inode_page[next_page.inode_t.page];
                }
            }

            if (cc != 0 || next_page.ipage != PFS_EOF) {
                bzero(&tmp_dir, sizeof(__OSDir));

                SET_ACTIVEBANK_TO_ZERO();
                ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tmp_dir, FALSE));
                fixed++;
            }
        }
#else
        if (tmp_dir.company_code != 0 && tmp_dir.game_code != 0) {
            next_page = tmp_dir.start_page;
            cl = cc = 0;
            bank = 255;

            while (CHECK_IPAGE(next_page)) {
                if (bank != next_page.inode_t.bank) {
                    bank = next_page.inode_t.bank;
                    ret = __osPfsRWInode(pfs, &tmp_inode, PFS_READ, bank);
                    if (ret != 0 && ret != PFS_ERR_INCONSISTENT) {
                        return ret;
                    }
                }
                
                if ((cc = corrupted(pfs, next_page, &cache) - cl) != 0) {
                    break;
                }

                cl = 1;
                next_page = tmp_inode.inode_page[next_page.inode_t.page];
            }

            if (cc != 0 || next_page.ipage != PFS_EOF) {
                tmp_dir.company_code = 0;
                tmp_dir.game_code = 0;
                tmp_dir.start_page.ipage = 0;
                tmp_dir.status = DIR_STATUS_EMPTY;
                tmp_dir.data_sum = 0;

                SET_ACTIVEBANK_TO_ZERO();
                ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tmp_dir, FALSE));
                fixed++;
            }
        } else {
            if (tmp_dir.company_code != 0 || tmp_dir.game_code != 0) {
                tmp_dir.company_code = 0;
                tmp_dir.game_code = 0;
                tmp_dir.start_page.ipage = 0;
                tmp_dir.status = DIR_STATUS_EMPTY;
                tmp_dir.data_sum = 0;

                SET_ACTIVEBANK_TO_ZERO();
                ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tmp_dir, FALSE));
                fixed++;
            }
        }
#endif
    }
    for (j = 0; j < pfs->dir_size; j++) {
        ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&tmp_dir));

        if (tmp_dir.company_code != 0 && tmp_dir.game_code != 0 &&
            tmp_dir.start_page.ipage >= (u16)pfs->inode_start_page) {
            file_next_node[j].ipage = tmp_dir.start_page.ipage;
        } else {
            file_next_node[j].ipage = 0;
        }
    }

    for (bank = 0; bank < pfs->banks; bank++) {
        ret = __osPfsRWInode(pfs, &tmp_inode, PFS_READ, bank);

        if (ret != 0 && ret != PFS_ERR_INCONSISTENT) {
            return ret;
        }

        offset = (bank > 0) ? 1 : pfs->inode_start_page;

        for (j = 0; j < offset; j++) {
            checked_inode.inode_page[j].ipage = tmp_inode.inode_page[j].ipage;
        }

        for (; j < 128; j++) {
            checked_inode.inode_page[j].ipage = PFS_PAGE_NOT_USED;
        }

        for (j = 0; j < pfs->dir_size; j++) {
            while (file_next_node[j].inode_t.bank == bank && file_next_node[j].ipage >= (u16)pfs->inode_start_page) {
                u8 pp = file_next_node[j].inode_t.page;
                file_next_node[j] = checked_inode.inode_page[pp] = tmp_inode.inode_page[pp];
            }
        }
        ERRCK(__osPfsRWInode(pfs, &checked_inode, PFS_WRITE, bank));
    }

    if (fixed) {
        pfs->status |= PFS_CORRUPTED;
    } else {
        pfs->status &= ~PFS_CORRUPTED;
    }

    return 0;
}

s32 corrupted_init(OSPfs* pfs, __OSInodeCache* cache) {
    int i;
    int n;
    int offset;
    u8 bank;
    __OSInodeUnit tpage;
    __OSInode tmp_inode;
    s32 ret;

    for (i = 0; i < PFS_INODE_DIST_MAP; i++) {
        cache->map[i] = 0;
    }

    cache->bank = -1;
    for (bank = 0; bank < pfs->banks; bank++) {
        offset = bank > 0 ? 1 : pfs->inode_start_page;

        ret = __osPfsRWInode(pfs, &tmp_inode, PFS_READ, bank);

        if (ret != 0 && ret != PFS_ERR_INCONSISTENT) {
            return ret;
        }

        for (i = offset; i < ARRLEN(tmp_inode.inode_page); i++) {
            tpage = tmp_inode.inode_page[i];

            if (tpage.ipage >= pfs->inode_start_page && tpage.inode_t.bank != bank) {
#if BUILD_VERSION >= VERSION_J
                n = ((tpage.inode_t.page & 0x7F) / PFS_SECTOR_SIZE) +
                    ((tpage.inode_t.bank % PFS_BANK_LAPPED_BY) * BLOCKSIZE);
#else
                n = ((tpage.inode_t.page) / PFS_SECTOR_SIZE) +
                    ((tpage.inode_t.bank % PFS_BANK_LAPPED_BY) * BLOCKSIZE);
#endif
                cache->map[n] |= 1 << (bank % PFS_BANK_LAPPED_BY);
            }
        }
    }
    return 0;
}

s32 corrupted(OSPfs* pfs, __OSInodeUnit fpage, __OSInodeCache* cache) {
    int j;
    int n;
    int hit;
    u8 bank;
    int offset;
    s32 ret;

    hit = 0;
    ret = 0;
    n = (fpage.inode_t.page / PFS_SECTOR_SIZE) + (fpage.inode_t.bank % PFS_BANK_LAPPED_BY) * BLOCKSIZE;

    for (bank = 0; bank < pfs->banks; bank++) {
        offset = bank > 0 ? 1 : pfs->inode_start_page;

        if (bank == fpage.inode_t.bank || cache->map[n] & (1 << (bank % PFS_BANK_LAPPED_BY))) {
            if (bank != cache->bank) {
                ret = __osPfsRWInode(pfs, &cache->inode, PFS_READ, bank);

                if (ret != 0 && ret != PFS_ERR_INCONSISTENT) {
                    return ret;
                }

                cache->bank = bank;
            }

            for (j = offset; hit < 2 && (j < ARRLEN(cache->inode.inode_page)); j++) {
                if (cache->inode.inode_page[j].ipage == fpage.ipage) {
                    hit++;
                }
            }

            if (hit >= 2) {
                return PFS_ERR_NEW_PACK;
            }
        }
    }
    return hit;
}

s32 osPfsDeleteFile(OSPfs* pfs, u16 company_code, u32 game_code, u8* game_name, u8* ext_name) {
    s32 file_no;
#if BUILD_VERSION < VERSION_J
    int k;
#endif
    s32 ret;
    __OSInode inode;
    __OSDir dir;
#if BUILD_VERSION < VERSION_J
    u16 sum = 0;
#endif
    __OSInodeUnit last_page;
    u8 startpage;
    u8 bank;

    if (company_code == 0 || game_code == 0) {
        return PFS_ERR_INVALID;
    }

#if BUILD_VERSION >= VERSION_J
    ERRCK(osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, &file_no));
    SET_ACTIVEBANK_TO_ZERO();
#else
    PFS_CHECK_STATUS();
    PFS_CHECK_ID();
    SET_ACTIVEBANK_TO_ZERO();
    ERRCK(osPfsFindFile(pfs, company_code, game_code, game_name, ext_name, &file_no));
    
    if (file_no == -1) {
        return PFS_ERR_INVALID;
    }
#endif
    ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir));

    startpage = dir.start_page.inode_t.page;

    for (bank = dir.start_page.inode_t.bank; bank < pfs->banks;) {
        ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
#if BUILD_VERSION >= VERSION_J
        ERRCK(__osPfsReleasePages(pfs, &inode, startpage, bank, &last_page));
#else
        ERRCK(__osPfsReleasePages(pfs, &inode, startpage, &sum, bank, &last_page, TRUE));
#endif
        ERRCK(__osPfsRWInode(pfs, &inode, PFS_WRITE, bank));

        if (last_page.ipage == PFS_EOF) {
            break;
        }

        bank = last_page.inode_t.bank;
        startpage = last_page.inode_t.page;
    }

    if (bank >= pfs->banks) {
        return PFS_ERR_INCONSISTENT;
    }

#if BUILD_VERSION >= VERSION_J
    bzero(&dir, sizeof(__OSDir));
#else
    dir.game_code = 0;
    dir.company_code = 0;
    dir.start_page.ipage = 0;
    dir.data_sum = 0;
    for (k = 0; k < ARRLEN(dir.game_name); k++)
    {
        dir.game_name[k] = 0;
    }
    for (k = 0; k < ARRLEN(dir.ext_name); k++)
    {
        dir.ext_name[k] = 0;
    }
    dir.status = DIR_STATUS_EMPTY;
#endif

    ret = __osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir, FALSE);

    return ret;
}

#if BUILD_VERSION >= VERSION_J

s32 __osPfsReleasePages(OSPfs* pfs, __OSInode* inode, u8 start_page, u8 bank, __OSInodeUnit* last_page) {
    __OSInodeUnit next_page;
    __OSInodeUnit old_page;
    s32 ret = 0;

    next_page.ipage = (bank << 8) + start_page;

    do {
        old_page = next_page;
        next_page = inode->inode_page[next_page.inode_t.page];
        inode->inode_page[old_page.inode_t.page].ipage = PFS_PAGE_NOT_USED;
    } while (next_page.ipage >= pfs->inode_start_page && next_page.inode_t.bank == bank);

    *last_page = next_page;

    return ret;
}

#else

s32 __osPfsReleasePages(OSPfs *pfs, __OSInode *inode, u8 start_page, u16 *sum, u8 bank, __OSInodeUnit *last_page, int flag)
{
    __OSInodeUnit next_page;
    __OSInodeUnit old_page;
    s32 ret;
    int offset;
    ret = 0;
    next_page = inode->inode_page[start_page];

    if (next_page.ipage != 1) {
        offset = (next_page.inode_t.bank > 0) ? 1 : pfs->inode_start_page;
    } else {
        offset = (bank > 0) ? 1 : pfs->inode_start_page;
    }

    if (next_page.inode_t.page < offset && next_page.ipage != 1) {
        return PFS_ERR_INCONSISTENT;
    }

    *last_page = next_page;
    
    if (flag == TRUE) {
        inode->inode_page[start_page].ipage = 3;
    }

    ERRCK(__osBlockSum(pfs, start_page, sum, bank));
    
    if (next_page.ipage == 1) {
        return 0;
    }

    while (next_page.ipage >= pfs->inode_start_page) {
        old_page = next_page;
        next_page = inode->inode_page[next_page.inode_t.page];
        inode->inode_page[old_page.inode_t.page].ipage = 3;

        ERRCK(__osBlockSum(pfs, old_page.inode_t.page, sum, bank));
        
        if (next_page.inode_t.bank != bank) {
            break;
        }
    }

    if (next_page.ipage >= pfs->inode_start_page) {
        inode->inode_page[next_page.inode_t.page].ipage = 3;
    }

    *last_page = next_page;
    return 0;
}
s32 __osBlockSum(OSPfs *pfs, u8 page_no, u16 *sum, u8 bank)
{
    int i;
    s32 ret;
    u8 data[32];
    ret = 0;
    pfs->activebank = bank;
    ERRCK(__osPfsSelectBank(pfs));
    for (i = 0; i < PFS_ONE_PAGE; i++)
    {
        ret = __osContRamRead(pfs->queue, pfs->channel, page_no * PFS_ONE_PAGE + i, data);
        if (ret != 0)
        {
            pfs->activebank = 0;
            __osPfsSelectBank(pfs);
            return ret;
        }
        *sum = *sum + __osSumcalc(data, sizeof(data));
    }
    pfs->activebank = 0;
    ret = __osPfsSelectBank(pfs);
    return ret;
}

#endif

s32 osPfsFileState(OSPfs* pfs, s32 file_no, OSPfsState* state) {
    s32 ret;
    int pages;
    __OSInode inode;
    __OSDir dir;
    __OSInodeUnit next_page;
#if BUILD_VERSION < VERSION_J
    int j;
#endif
    u8 bank;
#if BUILD_VERSION < VERSION_J
    u8 start_page;
#endif

    if (file_no >= pfs->dir_size || file_no < 0) {
        return PFS_ERR_INVALID;
    }

    PFS_CHECK_STATUS();
#if BUILD_VERSION >= VERSION_J
    ERRCK(__osCheckId(pfs));
#else
    PFS_CHECK_ID();
#endif
    SET_ACTIVEBANK_TO_ZERO();

    ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir));

    if (dir.company_code == 0 || dir.game_code == 0) {
        return PFS_ERR_INVALID;
    }

#if BUILD_VERSION >= VERSION_J
    pages = 0;
    next_page = dir.start_page;
    bank = 0xFF;

    while (TRUE) {
        if (next_page.ipage < pfs->inode_start_page) {
            break;
        } else if (next_page.inode_t.bank != bank) {
            bank = next_page.inode_t.bank;
            ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
        }

        pages++;
        next_page = inode.inode_page[next_page.inode_t.page];
    }

    if (next_page.ipage != PFS_EOF) {
        return PFS_ERR_INCONSISTENT;
    }

    state->file_size = pages * (PFS_ONE_PAGE * BLOCKSIZE);
    state->company_code = dir.company_code;
    state->game_code = dir.game_code;
    wcopy(&dir.game_name, state->game_name, PFS_FILE_NAME_LEN);
    wcopy(&dir.ext_name, state->ext_name, PFS_FILE_EXT_LEN);

    ret = __osPfsGetStatus(pfs->queue, pfs->channel);
    return ret;
#else
    if (dir.start_page.ipage < pfs->inode_start_page) {
        return PFS_ERR_INCONSISTENT;
    }

    pages = 0;
    start_page = dir.start_page.inode_t.page;
    bank = dir.start_page.inode_t.bank;

    while (bank < pfs->banks) {
        ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
        next_page = inode.inode_page[start_page];
        pages++;

        while (next_page.ipage >= pfs->inode_start_page) {
            pages++;
            next_page = inode.inode_page[next_page.inode_t.page];
            if (next_page.inode_t.bank != bank) {
                bank = next_page.inode_t.bank;
                start_page = next_page.inode_t.page;
                break;
            }
        }

        if (next_page.ipage == PFS_EOF) {
            break;
        }
    }
    
    if (next_page.ipage != PFS_EOF) {
        return PFS_ERR_INCONSISTENT;
    }

    state->file_size = pages * (PFS_ONE_PAGE * BLOCKSIZE);
    state->company_code = dir.company_code;
    state->game_code = dir.game_code;

    for (j = 0; j < ARRLEN(state->game_name); j++) {
        state->game_name[j] = dir.game_name[j];
    }

    for (j = 0; j < ARRLEN(state->ext_name); j++) {
        state->ext_name[j] = dir.ext_name[j];
    }

    return 0;
#endif
}

s32 osPfsFreeBlocks(OSPfs* pfs, s32* bytes_not_used) {
    int j;
    int pages = 0;
    __OSInode inode;
    s32 ret = 0;
    u8 bank;
    int offset;

    PFS_CHECK_STATUS();
#if BUILD_VERSION >= VERSION_J
    ERRCK(__osCheckId(pfs));
#else
    PFS_CHECK_ID();
#endif
    for (bank = 0; bank < pfs->banks; bank++) {
        ERRCK(__osPfsRWInode(pfs, &inode, PFS_READ, bank));
        offset = ((bank > 0) ? 1 : pfs->inode_start_page);

        for (j = offset; j < ARRLEN(inode.inode_page); j++) {
            if (inode.inode_page[j].ipage == PFS_PAGE_NOT_USED) {
                pages++;
            }
        }
    }

    *bytes_not_used = pages * PFS_ONE_PAGE * BLOCKSIZE;
    return 0;
}

s32 osPfsNumFiles(OSPfs* pfs, s32* max_files, s32* files_used) {
    int j;
    s32 ret;
    __OSDir dir;
    int files = 0;

    PFS_CHECK_STATUS();
#if BUILD_VERSION >= VERSION_J
    ERRCK(__osCheckId(pfs));
#else
    PFS_CHECK_ID();
#endif
    SET_ACTIVEBANK_TO_ZERO();

    for (j = 0; j < pfs->dir_size; j++) {
        ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&dir));

        if (dir.company_code != 0 && dir.game_code != 0) {
            files++;
        }
    }
    *files_used = files;
    *max_files = pfs->dir_size;

#if BUILD_VERSION >= VERSION_J
    ret = __osPfsGetStatus(pfs->queue, pfs->channel);
    return ret;
#else
    return 0;
#endif
}

#define CHECK_IPAGE(p)                                                                                        \
    (((p).ipage >= pfs->inode_start_page) && ((p).inode_t.bank < pfs->banks) && ((p).inode_t.page >= 0x01) && \
     ((p).inode_t.page < 0x80))

static s32 __osPfsGetNextPage(OSPfs* pfs, u8* bank, __OSInode* inode, __OSInodeUnit* page) {
    s32 ret;

    if (page->inode_t.bank != *bank) {
        *bank = page->inode_t.bank;
        ERRCK(__osPfsRWInode(pfs, inode, PFS_READ, *bank));
    }

    *page = inode->inode_page[page->inode_t.page];

    if (!CHECK_IPAGE(*page)) {
        if (page->ipage == PFS_EOF) {
            return PFS_ERR_INVALID;
        }

        return PFS_ERR_INCONSISTENT;
    }
    return 0;
}
s32 osPfsReadWriteFile(OSPfs* pfs, s32 file_no, u8 flag, int offset, int size_in_bytes, u8* data_buffer) {
    s32 ret;
    __OSDir dir;
    __OSInode inode;
    __OSInodeUnit cur_page;
    int cur_block;
    int siz_block;
    u8* buffer;
    u8 bank;
    u16 blockno;

    if ((file_no >= (s32)pfs->dir_size) || (file_no < 0)) {
        return PFS_ERR_INVALID;
    }

    if ((size_in_bytes <= 0) || ((size_in_bytes % BLOCKSIZE) != 0)) {
        return PFS_ERR_INVALID;
    }

    if ((offset < 0) || ((offset % BLOCKSIZE) != 0)) {
        return PFS_ERR_INVALID;
    }

    PFS_CHECK_STATUS();
    PFS_CHECK_ID();
    SET_ACTIVEBANK_TO_ZERO();
    ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir));

    if (!CHECK_IPAGE(dir.start_page)) {
        if ((dir.start_page.ipage == PFS_EOF)) {
            return PFS_ERR_INVALID;
        }

        return PFS_ERR_INCONSISTENT;
    }

    if (flag == PFS_READ && (dir.status & DIR_STATUS_OCCUPIED) == 0) {
        return PFS_ERR_BAD_DATA;
    }

    bank = -1;
    cur_block = offset / BLOCKSIZE;
    cur_page = dir.start_page;

    while (cur_block >= PFS_ONE_PAGE) {
        ERRCK(__osPfsGetNextPage(pfs, &bank, &inode, &cur_page));
        cur_block -= PFS_ONE_PAGE;
    }

    siz_block = size_in_bytes / BLOCKSIZE;
    buffer = data_buffer;

    while (siz_block > 0) {
        if (cur_block == PFS_ONE_PAGE) {
            ERRCK(__osPfsGetNextPage(pfs, &bank, &inode, &cur_page));
            cur_block = 0;
        }

        if (pfs->activebank != cur_page.inode_t.bank) {
            ERRCK(SELECT_BANK(pfs, cur_page.inode_t.bank));
        }

        blockno = cur_page.inode_t.page * PFS_ONE_PAGE + cur_block;

        if (flag == OS_READ) {
            ret = __osContRamRead(pfs->queue, pfs->channel, blockno, buffer);
        } else {
            ret = __osContRamWrite(pfs->queue, pfs->channel, blockno, buffer, FALSE);
        }

        if (ret != 0) {
            return ret;
        }
        buffer += BLOCKSIZE;
        cur_block++;
        siz_block--;
    }

    if (flag == PFS_WRITE && (dir.status & DIR_STATUS_OCCUPIED) == 0) {
        dir.status |= DIR_STATUS_OCCUPIED;
#if BUILD_VERSION >= VERSION_J
        SET_ACTIVEBANK_TO_ZERO();
#else
        ERRCK(SELECT_BANK(pfs, 0));
#endif
        ERRCK(__osContRamWrite(pfs->queue, pfs->channel, pfs->dir_table + file_no, (u8*)&dir, FALSE));
    }

#if BUILD_VERSION >= VERSION_J
    ret = __osPfsGetStatus(pfs->queue, pfs->channel);
    return ret;
#else
    return 0;
#endif
}

s32 osPfsReFormat(OSPfs* pfs, OSMesgQueue* queue, int channel) {
    int j;
    int i;
    __OSInode inode;
    u8 tmp_data[32];
    u8* ptr;
    s32 ret;
    __osSiGetAccess();
    ret = __osPfsGetStatus(queue, channel);

    if (ret != 0) {
        return ret;
    }

    pfs->status ^= PFS_INITIALIZED;
    pfs->queue = queue;
    pfs->channel = channel;
    __osSiRelAccess();
    ERRCK(__osGetId(pfs));

    SET_ACTIVEBANK_TO_ZERO();

    for (j = 0; j < ARRLEN(tmp_data); j++) {
        tmp_data[j] = 0;
    }

    for (j = 0; j < pfs->dir_size; j++) {
        ERRCK(__osContRamWrite(queue, channel, pfs->dir_table + j, tmp_data, FALSE));
    }

    for (j = 0; j < pfs->inode_start_page; j++) {
        inode.inode_page[j].ipage = 0;
    }

    for (j = pfs->inode_start_page; j < ARRLEN(inode.inode_page); j++) {
        inode.inode_page[j].ipage = 3;
    }

    inode.inode_page[0].ipage = __osSumcalc((u8*)(inode.inode_page + pfs->inode_start_page),
                                            (ARRLEN(inode.inode_page) - pfs->inode_start_page) * sizeof(__OSInodeUnit));
    ptr = (u8*)&inode;

    for (j = 0; j < PFS_ONE_PAGE; j++) {
        ERRCK(__osContRamWrite(queue, channel, pfs->inode_table + j, ptr + j * BLOCKSIZE, FALSE));
        ERRCK(__osContRamWrite(queue, channel, pfs->minode_table + j, ptr + j * BLOCKSIZE, FALSE));
    }

    for (i = 1; i < pfs->banks; i++) {
        for (j = 1; j < ARRLEN(inode.inode_page); j++) {
            inode.inode_page[j].ipage = 3;
        }

        inode.inode_page[0].ipage =
            __osSumcalc((u8*)(inode.inode_page + 1), (ARRLEN(inode.inode_page) - 1) * sizeof(__OSInodeUnit));
        ptr = (u8*)&inode;

        for (j = 0; j < PFS_ONE_PAGE; j++) {
            ERRCK(
                __osContRamWrite(queue, channel, pfs->inode_table + i * PFS_ONE_PAGE + j, ptr + j * BLOCKSIZE, FALSE));
            ERRCK(
                __osContRamWrite(queue, channel, pfs->minode_table + i * PFS_ONE_PAGE + j, ptr + j * BLOCKSIZE, FALSE));
        }
    }

    return 0;
}

s32 osPfsFindFile(OSPfs* pfs, u16 company_code, u32 game_code, u8* game_name, u8* ext_name, s32* file_no) {
    s32 j;
    int i;
    __OSDir dir;
    s32 ret = 0;
    int fail;

#if BUILD_VERSION >= VERSION_J
    if (!(pfs->status & PFS_INITIALIZED)) {
        return PFS_ERR_INVALID;
    }
    ERRCK(__osCheckId(pfs));
#else
    PFS_CHECK_ID();
#endif

    for (j = 0; j < pfs->dir_size; j++) {
        ERRCK(__osContRamRead(pfs->queue, pfs->channel, pfs->dir_table + j, (u8*)&dir));
#if BUILD_VERSION >= VERSION_J
        ERRCK(__osPfsGetStatus(pfs->queue, pfs->channel));
#endif

        if ((dir.company_code == company_code) && dir.game_code == game_code) {
            fail = FALSE;

            if (game_name != NULL) {
                for (i = 0; i < ARRLEN(dir.game_name); i++) {
                    if (dir.game_name[i] != game_name[i]) {
                        fail = TRUE;
                        break;
                    }
                }
            }

            if (ext_name != NULL && !fail) {
                for (i = 0; i < ARRLEN(dir.ext_name); i++) {
                    if (dir.ext_name[i] != ext_name[i]) {
                        fail = TRUE;
                        break;
                    }
                }
            }

            if (!fail) {
                *file_no = j;
                return ret;
            }
        }
    }

    *file_no = -1;
    return PFS_ERR_INVALID;
}
