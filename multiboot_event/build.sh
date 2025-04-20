#!/bin/bash

set -e

# Navigate to the build folder, clean, and rebuild
cd gba || exit
make clean

# Create the generated_symbols.h file
python generate_header.py

# Create the symbols.h
if [ ! -f "source/symbols.h" ]; then
    echo "source/symbols.h not found. Running create_symbols_header.sh..."
    sh create_symbols_header.sh
else
    echo "source/symbols.h already exists."
fi

make || exit
cd .. || exit

pwd
# Remove the gba_mb.gba file from the data folder
rm -f data/gba_mb.gba
rm -f self_extracting_gba/gba_mb.gba
cp gba/gba_pkjb.gba self_extracting_gba/gba_mb.gba

# Create a self extracting version of the mb to reduce tranfer size
rm -f self_extracting_gba/self_extracting_gba_mb.gba
rm -f self_extracting_gba/data/gba_mb_functions.lz.bin
rm -f self_extracting_gba/gba_mb_functions.bin
cd self_extracting_gba
python extractRomFunctions.py
gbalzss e gba_mb_functions.bin data/gba_mb_functions.lz.bin
make clean
make
cd ..
cp self_extracting_gba/self_extracting_gba_mb.gba data/gba_mb.gba

echo "MB ROMS can be up to 128kb, but the larger the more likely it wont inject."
COMPRESSED_FILE="self_extracting_gba/self_extracting_gba_mb.gba"
ALLOCATED_COMPRESSED_SIZE_KB=8  # 8KB
COMPRESSED_FILE_SIZE_KB=$(($(stat --format=%s "$COMPRESSED_FILE") / 1024))
echo "Self extracting ROM size: $COMPRESSED_FILE_SIZE_KB KB"
echo "Current allocated space is: $ALLOCATED_COMPRESSED_SIZE_KB KB"
if [ "$COMPRESSED_FILE_SIZE_KB" -gt "$ALLOCATED_COMPRESSED_SIZE_KB" ]; then
  echo "ERROR: The self extracting rom will overflow into the decompression buffer. You need to allocate more space"
  echo "In the gba project increase the .fill space in pkjb_crt0.s and update the MB_PADDING_SIZE in main.c"	
  echo "For the extracted project update 0x20E0 in extractRomFunctions.py and main.c"
  exit 1
fi
echo "SUCCESS: Rom is a good size."

make -f make.gc clean
make -f make.wii clean
make -f make.gc || exit
make -f make.gc clean
make -f make.wii || exit

echo "Build Finished."