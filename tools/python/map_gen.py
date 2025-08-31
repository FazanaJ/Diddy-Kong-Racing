import struct
import argparse
import os
from collections import defaultdict

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
                if filepath.endswith(".o"):
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
        # First pass: write placeholder OverlayFile structs
        overlay_entries = []
        for filename, sections in overlays.items():
            textAddr, textSize = sections["text"]
            dataAddr, dataSize = sections["data"]
            rodataAddr, rodataSize = sections["rodata"]
            bssAddr, bssSize = sections["bss"]
            symOffset = 0  # placeholder for now

            # Convert all addresses to offsets relative to the overlay .text start
            textAddr_offset = 0  # .text always starts at 0
            dataAddr_offset = dataAddr - textAddr
            rodataAddr_offset = rodataAddr - textAddr
            bssAddr_offset = bssAddr - textAddr

            overlay_entries.append((
                filename,
                textAddr_offset, textSize,
                dataAddr_offset, dataSize,
                rodataAddr_offset, rodataSize,
                bssAddr_offset, bssSize,
                symOffset
            ))


        # Write them all with placeholder symOffset
        table_start = f.tell()
        for entry in overlay_entries:
            packed = struct.pack(
                OVERLAY_STRUCT_FORMAT,
                *[int.from_bytes(x.to_bytes(4, "big"), "little") if isinstance(x, int) and x else 0
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
                sym_offset = f.tell()  # file offset where its symbols begin
            else:
                sym_offset = 0

            for addr, name in symbols:
                offset_addr = addr - sections["text"][0]  # relative to overlay .text
                addr_swapped = int.from_bytes(offset_addr.to_bytes(4, "big"), "little")
                name_bytes = name.encode("ascii", errors="replace")[:32]
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
            name_bytes = name.encode("ascii")[:32]
            name_bytes = name_bytes.ljust(32, b'\x00')
            binary_file.write(struct.pack(MAP_SYMBOL_STRUCT_FORMAT, address, name_bytes))

def parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("map_file")
    parser.add_argument("main_output_file")
    parser.add_argument("overlay_output_file")
    args = parser.parse_args()
    if os.path.isdir(args.main_output_file) or os.path.isdir(args.overlay_output_file):
        return

    # Parse main segment symbols
    main_symbols = map_parse(args.map_file, [".main"])
    main_symbols.sort(key=lambda x: x[0])
    write_binary_file(main_symbols, args.main_output_file)

    # Parse overlay segment symbols (add more section names if needed)
    overlays = parse_overlays_and_symbols(args.map_file, section_name=".overlays")
    write_overlay_and_symbol_tables(overlays, args.overlay_output_file)

parse()
