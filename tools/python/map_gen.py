import struct
import argparse
import os

MAP_SYMBOL_STRUCT_FORMAT = "<I32s"

def map_parse(map_file_path):
    symbols = []
    in_text_section = False

    with open(map_file_path, "r") as map_file:
        for line in map_file:
            line = line.strip()

            if line.startswith(".text"):
                in_text_section = True
                continue

            if in_text_section and line.startswith("."):
                in_text_section = False

            if in_text_section:
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
    parser.add_argument("output_file")
    args = parser.parse_args()
    if os.path.isdir(args.output_file):
        return
    symbols = map_parse(args.map_file)
    write_binary_file(symbols, args.output_file)

parse()