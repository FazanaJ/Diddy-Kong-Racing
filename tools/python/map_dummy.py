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

def write_overlay_enum(header_file_path):
    overlay_dir = os.path.join("src", "overlays")
    overlays = []

    # Collect overlay source filenames (ignore non-C files)
    for fname in sorted(os.listdir(overlay_dir)):
        if fname.endswith(".c"):
            overlays.append(fname)

    # Build the new header content in memory
    lines = []
    lines.append("#ifndef OVERLAYS_H\n")
    lines.append("#define OVERLAYS_H\n\n")
    lines.append("typedef enum {\n")

    for fname in overlays:
        base = os.path.splitext(fname)[0]  # remove .c
        name = base.upper().replace("-", "_").replace(".", "_")
        lines.append(f"    OVERLAY_{name},\n")

    lines.append("    OVERLAY_COUNT\n")
    lines.append("} OverlayID;\n\n")
    lines.append("#endif // OVERLAYS_H\n")

    new_content = "".join(lines)

    # Check if file exists and content is unchanged
    if os.path.exists(header_file_path):
        with open(header_file_path, "r") as f:
            old_content = f.read()
        if old_content == new_content:
            return  # no change, skip writing

    # Write new content only if changed
    with open(header_file_path, "w") as h:
        h.write(new_content)

def parse():
    write_binary_file()
    header_path = os.path.join("include", "overlays.h")
    os.makedirs(os.path.dirname(header_path), exist_ok=True)
    write_overlay_enum(header_path)

parse()