import struct
import argparse
import os

import json
import struct

MAP_SYMBOL_STRUCT_FORMAT = "<I32s"

troll = [4, 7]

def write_binary_file():
    with open("assets/map_symbols.bin", "wb") as binary_file:
        pass
    with open("assets/overlay_symbols.bin", "wb") as binary_file2:
        pass


def parse():
    write_binary_file()

parse()