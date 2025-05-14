import struct
import argparse
import os

import json
import struct

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
            base_bytes = base.encode('ascii', errors='replace')[:32]
            base_bytes = base_bytes.ljust(32, b'\0')
            out.write(base_bytes)



MAP_SYMBOL_STRUCT_FORMAT = "<I32s"

troll = [4, 7]

def write_binary_file():
    with open("assets/map_symbols.bin", "wb") as binary_file:
        pass


def parse():
    write_binary_file()

parse()
process_asset_list(
    "assets/us_1.0/asset_textures_2d.meta.json",
    "assets/asset_textures_2d_list.bin"
)
process_asset_list(
    "assets/us_1.0/asset_textures_3d.meta.json",
    "assets/asset_textures_3d_list.bin"
)
process_asset_list(
    "assets/us_1.0/asset_sprites.meta.json",
    "assets/asset_sprites_list.bin"
)
process_asset_list(
    "assets/us_1.0/asset_object_animations.meta.json",
    "assets/asset_object_animations_list.bin"
)
process_asset_list(
    "assets/us_1.0/asset_object_models.meta.json",
    "assets/asset_object_models_list.bin"
)