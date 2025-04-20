import os

GBA_FILE = "gba_mb.gba"
OUTPUT_FILE = "gba_mb_functions.bin"
SKIP_OFFSET = 0x20E0  # Skip the multiboot header and padding

def extract_gba_function_data(gba_file, output_file, skip_offset):
    if not os.path.exists(gba_file):
        print(f"Error: {gba_file} not found.")
        return

    with open(gba_file, "rb") as f:
        f.seek(0, os.SEEK_END)
        gba_size = f.tell()
        if gba_size <= skip_offset:
            print("Error: GBA file is too small to contain content after padding.")
            return

        f.seek(skip_offset)
        content = f.read()

    if os.path.dirname(output_file):
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, "wb") as out:
        out.write(content)

    print(f"Extracted {len(content)} bytes to {output_file}.")

if __name__ == "__main__":
    extract_gba_function_data(GBA_FILE, OUTPUT_FILE, SKIP_OFFSET)
