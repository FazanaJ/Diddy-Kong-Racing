import json
import struct
import sys
import os

asset_list = ["asset_textures_2d", "asset_textures_3d", "asset_sprites", "asset_object_animations", "asset_object_models"]

def process_asset_list(json_path, out_bin):
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    order = data["files"]["order"]
    sections = data["files"]["sections"]
    with open(out_bin, 'wb') as out:
        for idx, asset_name in enumerate(order):
            section = sections.get(asset_name)
            if not section or "filename" not in section:
                continue
            filename = section["filename"]
            base = os.path.splitext(os.path.basename(filename))[0]
            base_bytes = base.encode('ascii', errors='replace')[:31]
            base_bytes = base_bytes.ljust(32, b'\0')
            out.write(base_bytes)

def process_overlay_list(src_dir, out_bin):
    with open(out_bin, 'wb') as out:
        for filename in sorted(os.listdir(src_dir)):
            if not filename.endswith(".c"):
                continue
            base = os.path.splitext(filename)[0]
            base_bytes = base.encode('ascii', errors='replace')[:31]
            base_bytes = base_bytes.ljust(32, b'\0')
            out.write(base_bytes)

region = sys.argv[1]
version = sys.argv[2]
for asset in asset_list:
    process_asset_list(
        "assets/" + region + "." + version + "/" + asset + ".meta.json",
        "assets/" + asset + "_list.bin"
    )
    process_overlay_list("src/overlays", "assets/overlay_list.bin")
