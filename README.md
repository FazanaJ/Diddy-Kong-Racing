# Diddy Kong Racing

This repo contains a work-in-progress decompilation of Diddy Kong Racing for the N64. You will need to have a original copy of the ROM to extract the game's assets.

Currently, only the US 1.0 version of the game is supported. US 1.1, EU 1.0, EU 1.1, and JP are not supported at this time.

---

## Dependencies

* `libcapstone-dev`
* `libssl-dev`
* `gcc`, Version 8.0 or higher
* `make`, Version 4.2 or higher
* `python3`
* `wget`

`sudo apt install build-essential pkg-config git python3 libssl-dev wget libcapstone-dev`

### binutils

You are not required to install a binutils package, but it does speed up the initial setup if you do have it installed.

* Ubuntu/Debian (x86): `sudo apt install binutils-mips-linux-gnu`

## Setup / Building
1. Install the dependencies
2. Place the ROM file within the `baseroms` directory.  
    **a.** The name of the ROM file does not matter, but it must end with one of these extensions: `.z64`, `.v64`, `.n64`, or `.rom`. It will be detected automatically from an md5 checksum.  
    **b.** If you use a byte-swapped or little-endian ROM, then it will automatically be converted to a big-endian (.z64) ROM file.  
3. Run `make` in the main directory.  
    **a.** Use the `-jN` argument to use `N` number of threads to speed up building. For example, if you have a system with 4 cores / 4 threads, you should do `make -j4`.

## Modding
If you are modifying the code in the repo, then you should add `NON_MATCHING=1` to the make command.  
Example: `make NON_MATCHING=1 -j4`  
  
The `NON_MATCHING` define will include the functions that don't exactly match one-to-one, but should be no different functionality-wise. If you do notice any bugs that occur in a `NON_MATCHING` build that are not in the vanilla game, then please file an issue describing the bug. It would be helpful if you can track down which function is causing the bug, but that is not required.

## Style Guide

The style guide for the project can be found here: https://docs.google.com/document/d/1euQf8nwynGcCZL1MfoMaLs-XRvx3ejjj8fIfykUZ-NQ

If you see variables/functions/structs/etc. that doesn't follow these rules, then file an issue and we'll try to fix them.

## Scripts

There are some useful scripts that should be kept in mind when working on this repo.

---

#### `./extract.sh <version>`

This script will extract all the assets from a DKR ROM and place them into `/assets/` folder according to the config file within the `/extract-ver/` folder. The version parameter should either be `us_1.0`, `us_1.1`, `eu_1.0`, `eu_1.1`, or `jp`.

Example: `./extract.sh us_1.0`

---

#### `./diff.sh <function_name>`

Compares the assembly output of a function from a built ROM to the base ROM. Useful for decompiling.

Example: `./diff.sh menu_init`

---

#### `./rename.sh <old_name> <new_name>`

This script will rename an existing symbol, file name, and/or folder name within the repo with a new one.

Example: `./rename.sh D_A4001000 SP_IMEM`

---

#### `./get_symbol.sh <ram_address>|<symbol>`

Given either a RAM address or symbol, returns its `(symbol, address)` pairing as defined in `/build/us_1.0/dkr.map`. If specified, the RAM address must be in base 16. The `0x` prefix is not required.

Example: 
```
./get_symbol.sh 0xA4001000
0xA4001000 = SP_IMEM

./get_symbol.sh osCicId
0x80000310 = osCicId
```

---

#### `./score.sh`

Prints out the current completion percentage of the decomp. You do need to have an `OK` build for this to work properly.

The scoring is split into 2 sections: 
1. Assembly to C Decompilation (Adventure One)
2. Cleanup and Documentation (Adventure Two)

Show scores for Adventure One & Adventure Two: `./score.sh`  
Show score for Adventure One only: `./score.sh -a 1`  
Show score for Adventure Two only: `./score.sh -a 2`  
Show top 10 files remaining: `./score.sh -t 10`  

To make progress in Adventure 2, a function must be properly named (must not start with `func_`) and also include a doxygen comment above it.

For example:
```c
/**
 * Returns 1 if Drumstick is avaliable to use, or 0 if not.
 */
s32 is_drumstick_unlocked(void) {
    return gActiveMagicCodes & 2;
}
```

As of October 3rd, 2022, this is our current score:
```
 ==========================================
   ADVENTURE ONE (ASM -> C Decompilation)
 ------------ 33.19% Complete -------------
        # Decompiled functions: 1292
        # GLOBAL_ASM remaining: 541
        # NON_MATCHING functions: 12
     # NON_EQUIVALENT WIP functions: 94
 -------------- Game Status ---------------
 Balloons: 17/47, Keys: 2/4, Trophies: 1/5
   T.T. Amulets: 1/4, Wizpig Amulets: 1/4
 ------------------------------------------
    We are racing the walrus boss Bluey.
 ==========================================
   ADVENTURE TWO (Cleanup & Documentation)
 ------------  6.13% Complete -------------
        # Documented functions: 210
       # Undocumented remaining: 1623
 -------------- Game Status ---------------
  Balloons: 4/47, Keys: 0/4, Trophies: 0/5
   T.T. Amulets: 0/4, Wizpig Amulets: 0/4
 ------------------------------------------
  We are racing in Ancient Lake. (Lap 1/3)
 ==========================================
```

---

## TODO list

### Major

What should be focused on.

* Decompiling the non-matching asm files into matching C functions.
* Documenting variables & functions

### Minor

What can be done, but not essential.

* Figuring out the formats for the assets in the /assets/ folder.
* Creating tools to modify assets

### Future

These features won't be complete anytime soon.

* Add support for the other 4 versions.
