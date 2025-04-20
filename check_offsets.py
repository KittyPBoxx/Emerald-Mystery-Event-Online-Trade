import sys
import subprocess

# Dictionary of symbols and their expected addresses
EXPECTED_SYMBOLS = {
    "gBerryCrush_TextWindows_Tilemap": 0x08DE3FD4,
    "gStringVar1" : 0x02021cc4
    # Add more symbols here as needed
    # "symbol_name": 0xExpectedAddress
}

def get_symbol_addresses(elf_file):
    """Extracts addresses of all symbols from the ELF file."""
    symbol_addresses = {}
    try:
        result = subprocess.run(
            ["arm-none-eabi-nm", "-n", elf_file],
            capture_output=True, text=True, check=True
        )
        lines = result.stdout.splitlines()
        for line in lines:
            parts = line.split()
            if len(parts) >= 3:
                address = int(parts[0], 16)
                symbol = parts[2]
                symbol_addresses[symbol] = address
    except Exception as e:
        print(f"Error reading ELF file: {e}")
    return symbol_addresses

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python check_offsets.py <elf_file>")
        sys.exit(1)

    elf_file = sys.argv[1]
    actual_symbols = get_symbol_addresses(elf_file)

    # Check each expected symbol
    for symbol, expected_address in EXPECTED_SYMBOLS.items():
        actual_address = actual_symbols.get(symbol)

        if actual_address is None:
            print(f"Warning: Could not find symbol {symbol} in ELF file.")
        elif actual_address != expected_address:
            print(f"WARNING: The address of {symbol} has changed!")
            print(f"   Expected: {hex(expected_address)}, Found: {hex(actual_address)}")
        else:
            print(f"{symbol} is at the expected address {hex(expected_address)} for vanilla.")

    print("Offset check complete.")
