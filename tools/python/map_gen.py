import struct
import argparse
import os
from collections import defaultdict
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection

MAP_SYMBOL_STRUCT_FORMAT = "<I32s"
OVERLAY_STRUCT_FORMAT = "<IIIIIIIII"  # 9x u32 (symOffset added)

def parse_overlays_and_symbols(map_file_path, section_name=".overlays"):
    overlays = defaultdict(lambda: {
        "text": (0, 0),
        "data": (0, 0),
        "rodata": (0, 0),
        "bss": (0, 0),
        "symbols": []
    })

    in_overlays = False
    current_file = None

    with open(map_file_path, "r") as f:
        for rawline in f:
            line = rawline.strip()
            if not line:
                continue

            # Enter overlays section
            if line.startswith(section_name):
                in_overlays = True
                continue

            # Exit overlays if .main or another top-level starts
            if in_overlays and line.startswith(".main"):
                in_overlays = False

            if not in_overlays:
                continue

            parts = line.split()
            if len(parts) < 2:
                continue

            section = parts[0]

            # Section definitions (text/data/rodata/bss)
            if section.startswith((".text", ".data", ".rodata", ".bss")) and len(parts) >= 4:
                try:
                    addr = int(parts[1], 16)
                    size = int(parts[2], 16)
                except ValueError:
                    continue

                filepath = parts[3]
                if filepath.endswith(".c.o"):
                    filename = os.path.basename(filepath)
                    current_file = filename

                    # Save the section base for offset calculations
                    if section.startswith(".text"):
                        overlays[filename]["text"] = (addr, size)
                        overlays[filename]["text_base"] = addr  # <--- add this
                    elif section.startswith(".data"):
                        overlays[filename]["data"] = (addr, size)
                    elif section.startswith(".rodata"):
                        overlays[filename]["rodata"] = (addr, size)
                    elif section.startswith(".bss"):
                        overlays[filename]["bss"] = (addr, size)


            # Symbol lines (address + name, no file path)
            elif current_file and len(parts) == 2:
                try:
                    addr = int(parts[0], 16)
                except ValueError:
                    continue
                name = parts[1]
                overlays[current_file]["symbols"].append((addr, name))

    return overlays

def write_overlay_and_symbol_tables(overlays, output_file_path):
    with open(output_file_path, "wb") as f:
        # Compute total sizes first
        total_text_size = sum(sections["text"][1] for sections in overlays.values())
        total_data_size = sum(sections["data"][1] for sections in overlays.values())
        total_rodata_size = sum(sections["rodata"][1] for sections in overlays.values())
        total_bss_size = sum(sections["bss"][1] for sections in overlays.values())

        # Now cumulative offsets per section
        text_offset = 0
        data_offset = total_text_size     # data comes after all text
        rodata_offset = total_text_size + total_data_size
        bss_offset = total_text_size + total_data_size + total_rodata_size

        overlay_entries = []
        for filename, sections in overlays.items():
            textSize = sections["text"][1]
            dataSize = sections["data"][1]
            rodataSize = sections["rodata"][1]
            bssSize = sections["bss"][1]

            overlay_entries.append((
                filename,
                text_offset, textSize,
                data_offset, dataSize,
                rodata_offset, rodataSize,
                bss_offset, bssSize,
                0  # symOffset placeholder
            ))

            text_offset += textSize
            data_offset += dataSize
            rodata_offset += rodataSize
            bss_offset += bssSize

        # Write them all with placeholder symOffset
        table_start = f.tell()
        for entry in overlay_entries:
            filename, text_off, textSize, data_off, dataSize, rodata_off, rodataSize, bss_off, bssSize, symOff = entry
            totalSize = textSize + dataSize + rodataSize + bssSize
            print(f"{filename}: text={hex(textSize)}, data={hex(dataSize)}, rodata={hex(rodataSize)}, bss={hex(bssSize)}, total={hex(totalSize)}")

            packed = struct.pack(
                OVERLAY_STRUCT_FORMAT,
                *[int.from_bytes(x.to_bytes(4, "big", signed=True), "little", signed=False) if isinstance(x, int) and x else 0
                for x in entry[1:]]  # skip filename
            )
            f.write(packed)

        # Align to 32 bytes before writing symbol table
        #while f.tell() % 32 != 0:
        #    f.write(b"\x00")

        symbol_table_start = f.tell()

        # Second pass: write symbols, remember offsets
        new_overlay_entries = []
        for entry in overlay_entries:
            filename = entry[0]
            sections = overlays[filename]
            symbols = sections["symbols"]

            if symbols:
                sym_offset = f.tell()  # file offset where symbols begin
            else:
                sym_offset = 0

            text_cumulative_offset = entry[1]  # text_offset in binary
            for addr, name in symbols:
                if name.startswith("_binary"):
                    continue
                offset_addr = text_cumulative_offset + (addr - sections["text"][0])
                if offset_addr < 0:
                    offset_addr = 0

                addr_swapped = struct.unpack("<I", struct.pack(">I", offset_addr))[0]

                name_bytes = name.encode("ascii", errors="replace")[:31]
                name_bytes = name_bytes.ljust(32, b"\x00")
                f.write(struct.pack(MAP_SYMBOL_STRUCT_FORMAT, addr_swapped, name_bytes))

            new_overlay_entries.append((
                filename,
                entry[1], entry[2],
                entry[3], entry[4],
                entry[5], entry[6],
                entry[7], entry[8],
                sym_offset
            ))


        # Seek back and rewrite overlay table with real symOffsets
        f.seek(table_start)
        for entry in new_overlay_entries:
            packed = struct.pack(
                OVERLAY_STRUCT_FORMAT,
                *[int.from_bytes(x.to_bytes(4, "big"), "little") if isinstance(x, int) and x else 0
                  for x in entry[1:]]
            )
            f.write(packed)

def map_parse(map_file_path, target_sections):
    symbols = []
    in_target_section = False
    in_text = False

    stop_sections = (".data", ".bss", ".rodata")

    with open(map_file_path, "r") as map_file:
        for line in map_file:
            line = line.strip()
            if not line:
                continue

            # Entering a target section (.main, .overlays, etc.)
            if any(line.startswith(section) for section in target_sections):
                in_target_section = True
                in_text = False
                continue

            # Leaving target section if we hit stop sections
            if in_target_section and any(line.startswith(s) for s in stop_sections):
                in_target_section = False
                in_text = False
                continue

            if in_target_section:
                # Detect start of a .text block
                if line.startswith(".text"):
                    in_text = True
                    continue

                # If another subsection starts, stop collecting
                if line.startswith(".") and not line.startswith(".text"):
                    in_text = False
                    continue

                if in_text:
                    parts = line.split()
                    if len(parts) >= 2:
                        try:
                            address = int(parts[0], 16)
                            name = parts[-1]
                            symbols.append((address, name))
                        except ValueError:
                            continue

    return symbols


def write_binary_file(symbols, output_file_path):
    with open(output_file_path, "wb") as binary_file:
        for address, name in symbols:
            address = struct.unpack("<I", struct.pack(">I", address))[0]
            name_bytes = name.encode("ascii")[:31]
            name_bytes = name_bytes.ljust(32, b'\x00')
            binary_file.write(struct.pack(MAP_SYMBOL_STRUCT_FORMAT, address, name_bytes))

def parse_combined_overlays(map_file_path, elf_path, section_name=".overlays"):
    # First get overlay structure from .map file
    overlays = parse_overlays_and_symbols(map_file_path, section_name)
    
    # Then augment with ELF data
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        symtab = elf.get_section_by_name('.symtab')
        
        if symtab:
            for sym in symtab.iter_symbols():
                try:
                    # Skip special sections and linker-generated symbols
                    if (isinstance(sym.entry.st_shndx, str) or 
                        sym.entry.st_shndx == 0 or
                        sym.name.startswith(('overlays_', '__', '.'))):
                        continue
                        
                    sym_section = elf.get_section(sym.entry.st_shndx)
                    if sym_section and sym_section.name.startswith('.overlays'):
                        addr = sym.entry.st_value
                        name = sym.name
                        size = sym.entry.st_size
                        
                        # Find which overlay file this belongs to by address range
                        for filename, sections in overlays.items():
                            text_start, text_size = sections["text"]
                            data_start, data_size = sections["data"]
                            rodata_start, rodata_size = sections["rodata"]
                            bss_start, bss_size = sections["bss"]
                            
                            # Check if symbol falls within this file's sections
                            in_text = text_start <= addr < text_start + text_size
                            in_data = data_start <= addr < data_start + data_size
                            in_rodata = rodata_start <= addr < rodata_start + rodata_size
                            in_bss = bss_start <= addr < bss_start + bss_size
                            
                            if in_text or in_data or in_rodata or in_bss:
                                # Add symbol if it's not already present
                                if not any(sym_addr == addr for sym_addr, _ in sections["symbols"]):
                                    sections["symbols"].append((addr, name))
                                break
                                
                except (TypeError, AttributeError):
                    continue
    
    return overlays

def parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("map_file")
    parser.add_argument("elf_file") 
    parser.add_argument("main_output_file")
    parser.add_argument("overlay_output_file")
    args = parser.parse_args()
    
    if os.path.isdir(args.main_output_file) or os.path.isdir(args.overlay_output_file):
        return

    # Parse main segment symbols
    main_symbols = map_parse(args.map_file, [".main"])
    main_symbols.sort(key=lambda x: x[0])
    write_binary_file(main_symbols, args.main_output_file)

    # Parse overlays using combined approach
    overlays = parse_combined_overlays(args.map_file, args.elf_file)
    write_overlay_and_symbol_tables(overlays, args.overlay_output_file)

parse()
