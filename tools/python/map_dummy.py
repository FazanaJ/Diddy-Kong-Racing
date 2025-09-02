import struct
import argparse
import os
import json
import struct

def write_binary_file():
    with open("assets/map_symbols.bin", "wb") as binary_file:
        pass
    with open("assets/overlay_symbols.bin", "wb") as binary_file2:
        pass

def write_overlay_and_object_enum(header_file_path):
    overlay_dir = os.path.join("src", "overlays")
    object_dir = os.path.join("src", "objects")

    entries = []

    if os.path.exists(overlay_dir):
        for fname in sorted(os.listdir(overlay_dir)):
            if fname.endswith(".c"):
                base = os.path.splitext(fname)[0]
                name = base.upper().replace("-", "_").replace(".", "_")
                entries.append(f"    OVERLAY_{name},")

    if os.path.exists(object_dir):
        for fname in sorted(os.listdir(object_dir)):
            if fname.endswith(".c"):
                base = os.path.splitext(fname)[0]
                name = base.upper().replace("-", "_").replace(".", "_")
                entries.append(f"    OBJOVL_{name},")

    lines = []
    lines.append("#ifndef OVERLAYS_H\n")
    lines.append("#define OVERLAYS_H\n\n")

    lines.append("typedef enum {\n")
    lines.extend(line + "\n" for line in entries)
    lines.append("    OVERLAY_COUNT\n")
    lines.append("} OverlayID;\n\n")

    lines.append("#endif // OVERLAYS_H\n")

    new_content = "".join(lines)

    if os.path.exists(header_file_path):
        with open(header_file_path, "r") as f:
            old_content = f.read()
        if old_content == new_content:
            return

    with open(header_file_path, "w") as h:
        h.write(new_content)



def parse():
    write_binary_file()
    header_path = os.path.join("include", "overlays.h")
    os.makedirs(os.path.dirname(header_path), exist_ok=True)
    write_overlay_and_object_enum(header_path)

parse()