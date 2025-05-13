if [ "$(uname)" = "Darwin" ]; then
python3 ./tools/python/map_dummy.py; gmake assets -j; python3 ./tools/python/map_gen.py ./build/dkr.us.v77.map ./assets/map_symbols.bin; gmake assets -j
else
python3 ./tools/python/map_dummy.py; make assets -j; python3 ./tools/python/map_gen.py ./build/dkr.us.v77.map ./assets/map_symbols.bin; make assets -j
fi