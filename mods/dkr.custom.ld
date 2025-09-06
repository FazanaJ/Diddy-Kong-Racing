SECTIONS
{
    __romPos = 0;
    header_ROM_START = __romPos;
    header_VRAM = ADDR(.header);
    .header : AT(header_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        header_DATA_START = .;
        build/asm/header.s.o(.data);
        header_DATA_END = .;
        header_DATA_SIZE = ABSOLUTE(header_DATA_END - header_DATA_START);
    }
    __romPos += SIZEOF(.header);
    header_ROM_END = __romPos;
    header_VRAM_END = .;

    boot_ROM_START = __romPos;
    boot_VRAM = ADDR(.boot);
    .boot : AT(boot_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        boot_DATA_START = .;
        build/assets/boot.bin.o(.data);
        boot_DATA_END = .;
        boot_DATA_SIZE = ABSOLUTE(boot_DATA_END - boot_DATA_START);
    }
    __romPos += SIZEOF(.boot);
    boot_ROM_END = __romPos;
    boot_VRAM_END = .;

    main_ROM_START = __romPos;
    main_VRAM = ADDR(.main);
    .main 0x80000400 : AT(main_ROM_START) SUBALIGN(16)
    {
        /* Text */
        FILL(0x00000000);
        main_TEXT_START = .;
        build/src/hasm/entrypoint.s.o(.text);
        build/src*.o(.text);
        build/libultra/src*.o(.text);
        build/asm/data/aspMain.s.o(.text);
        build/asm/data/rspboot.s.o(.text);
        build/asm/data/f3ddkr_dram.s.o(.text);
        build/asm/data/f3ddkr_fifo.s.o(.text);
        build/asm/data/f3ddkr_xbus.s.o(.text);
        /* Data */
        main_TEXT_END = .;
        main_TEXT_SIZE = ABSOLUTE(main_TEXT_END - main_TEXT_START);
        main_DATA_START = .;
        build/src*.o(.data*);
        build/libultra/src*.o(.data);
        build/asm/data/aspMain.s.o(.data);
        build/asm/data/f3ddkr_fifo.s.o(.data);
        build/asm/data/f3ddkr_xbus.s.o(.data);
        /* Rodata */
        main_DATA_END = .;
        main_DATA_SIZE = ABSOLUTE(main_DATA_END - main_DATA_START);
        main_RODATA_START = .;
        build/src*.o(.rodata*);
        build/libultra/src*.o(.rodata*);
        . = ALIGN(., 16);
        main_RODATA_END = .;
        main_RODATA_SIZE = ABSOLUTE(main_RODATA_END - main_RODATA_START);
    }
    main_bss_VRAM = ADDR(.main_bss);
    .main_bss (NOLOAD) : SUBALIGN(16)
    {
        FILL(0x00000000);
        main_BSS_START = .;

        build/src*.o(.*bss*);
        build/libultra/src*.o(.*bss*);
        . = ALIGN(., 16);
        main_BSS_END = .;
        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);
    }
    __romPos += SIZEOF(.main);
    __romPos = ALIGN(__romPos, 16);
    . = ALIGN(., 16);
    main_ROM_END = __romPos;
    main_VRAM_END = .;

    overlays_ROM_START = __romPos;
    overlays_VRAM = ADDR(.overlays);
    .overlays assets_VRAM_END : AT(overlays_ROM_START) SUBALIGN(16)
    {
        overlays_TEXT_START = .;
        build/overlays/**/*.o(.text);
        overlays_TEXT_END = .;
        overlays_DATA_START = .;
        build/overlays/**/*.o(.data*);
        overlays_DATA_END = .;
        overlays_RODATA_START = .;
        build/overlays/**/*.o(.rodata*);
        overlays_RODATA_END = .;
        overlays_BSS_START = .;
        build/overlays/**/*.o(.*bss*);
        overlays_BSS_END = .;
        overlays_TEXT_SIZE = ABSOLUTE(overlays_TEXT_END - overlays_TEXT_START);
        overlays_DATA_SIZE = ABSOLUTE(overlays_DATA_END - overlays_DATA_START);
        overlays_RODATA_SIZE = ABSOLUTE(overlays_RODATA_END - overlays_RODATA_START);
        overlays_BSS_SIZE = ABSOLUTE(overlays_BSS_END - overlays_BSS_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.overlays);
    overlays_ROM_END = __romPos;
    overlays_VRAM_END = .;

    assets_ROM_START = __romPos;
    assets_VRAM = ADDR(.assets);
    .assets main_VRAM_END : AT(assets_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        assets_DATA_START = .;
        build/assets/assets.bin.o(.data);
        assets_DATA_END = .;
        assets_DATA_SIZE = ABSOLUTE(assets_DATA_END - assets_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.assets);
    assets_ROM_END = __romPos;
    assets_VRAM_END = .;

    map_ROM_START = __romPos;
    map_VRAM = ADDR(.map);
    .map assets_VRAM_END : AT(map_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        map_DATA_START = .;
        build/assets/map_symbols.bin.o(.data);
        map_DATA_END = .;
        map_DATA_SIZE = ABSOLUTE(map_DATA_END - map_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.map);
    map_ROM_END = __romPos;
    map_VRAM_END = .;

    tex2d_ROM_START = __romPos;
    tex2d_VRAM = ADDR(.tex2d);
    .tex2d assets_VRAM_END : AT(tex2d_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        tex2d_DATA_START = .;
        build/assets/asset_textures_2d_list.bin.o(.data);
        tex2d_DATA_END = .;
        tex2d_DATA_SIZE = ABSOLUTE(tex2d_DATA_END - tex2d_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.tex2d);
    tex2d_ROM_END = __romPos;
    tex2d_VRAM_END = .;

    tex3d_ROM_START = __romPos;
    tex3d_VRAM = ADDR(.tex3d);
    .tex3d assets_VRAM_END : AT(tex3d_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        tex3d_DATA_START = .;
        build/assets/asset_textures_3d_list.bin.o(.data);
        tex3d_DATA_END = .;
        tex3d_DATA_SIZE = ABSOLUTE(tex3d_DATA_END - tex3d_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.tex3d);
    tex3d_ROM_END = __romPos;
    tex3d_VRAM_END = .;

    sprites_ROM_START = __romPos;
    sprites_VRAM = ADDR(.sprites);
    .sprites assets_VRAM_END : AT(sprites_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        sprites_DATA_START = .;
        build/assets/asset_sprites_list.bin.o(.data);
        sprites_DATA_END = .;
        sprites_DATA_SIZE = ABSOLUTE(sprites_DATA_END - sprites_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.sprites);
    sprites_ROM_END = __romPos;
    sprites_VRAM_END = .;

    objmdl_ROM_START = __romPos;
    objmdl_VRAM = ADDR(.objmdl);
    .objmdl assets_VRAM_END : AT(objmdl_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        objmdl_DATA_START = .;
        build/assets/asset_object_models_list.bin.o(.data);
        objmdl_DATA_END = .;
        objmdl_DATA_SIZE = ABSOLUTE(objmdl_DATA_END - objmdl_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.objmdl);
    objmdl_ROM_END = __romPos;
    objmdl_VRAM_END = .;

    objanim_ROM_START = __romPos;
    objanim_VRAM = ADDR(.objanim);
    .objanim assets_VRAM_END : AT(objanim_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        objanim_DATA_START = .;
        build/assets/asset_object_models_list.bin.o(.data);
        objanim_DATA_END = .;
        objanim_DATA_SIZE = ABSOLUTE(objanim_DATA_END - objanim_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.objanim);
    objanim_ROM_END = __romPos;
    objanim_VRAM_END = .;

    ovltable_ROM_START = __romPos;
    ovltable_VRAM = ADDR(.ovltable);
    .ovltable assets_VRAM_END : AT(ovltable_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        ovltable_DATA_START = .;
        build/assets/overlay_symbols.bin.o(.data);
        ovltable_DATA_END = .;
        ovltable_DATA_SIZE = ABSOLUTE(ovltable_DATA_END - ovltable_DATA_START);
        . = ALIGN(., 16);
    }
    __romPos += SIZEOF(.ovltable);
    ovltable_ROM_END = __romPos;
    ovltable_VRAM_END = .;
    
    ovlNames_ROM_START = __romPos;
    ovlNames_VRAM = ADDR(.ovlNames);
    .ovlNames assets_VRAM_END : AT(ovlNames_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        ovlNames_DATA_START = .;
        build/assets/overlay_list.bin.o(.data);
        ovlNames_DATA_END = .;
        ovlNames_DATA_SIZE = ABSOLUTE(ovlNames_DATA_END - ovlNames_DATA_START);
    }
    __romPos += SIZEOF(.ovlNames);
    ovlNames_ROM_END = __romPos;
    ovlNames_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
