#!/bin/bash
set -e
touch src/mystery_event_script_custom.c # Ensure a the rom uses the new flags
make events
python gen_ram_script.py pokeemerald.elf .ram_function
python gen_event_script.py pokeemerald.elf .mystery_event_script
python generate_header.py
mv event_script.bin ram_script.bin multiboot_event/gba/data/
touch src/mystery_event_script.c # Ensure a the rom uses the new flags
make
cp pokeemerald.elf multiboot_event/gba/

python check_offsets.py pokeemerald.elf
cd multiboot_event
./build.sh