from elftools.elf.elffile import ELFFile
import sys

def extract_section_data(elf_path, section_name):
    with open(elf_path, "rb") as f:
        elffile = ELFFile(f)
        
        # Find the section by name
        section = None
        for sec in elffile.iter_sections():
            if sec.name == section_name:
                section = sec
                break
        
        if section is None:
            raise ValueError(f"Section '{section_name}' not found in ELF file.")

        # Get the section data
        section_data = section.data()
        return section_data

def format_as_c_array(data, array_name="section_data"):
    c_array = f"unsigned char {array_name}[] = {{\n    "
    for i, byte in enumerate(data):
        c_array += f"0x{byte:02X}, "
        if (i + 1) % 12 == 0:  # Wrap lines every 12 bytes
            c_array += "\n    "
    c_array = c_array.rstrip(", \n") + "\n};"
    return c_array

def write_to_bin_file(data, file_name="ram_script.bin"):
    with open(file_name, "wb") as f:
        f.write(data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python gen_ram_script.py <elf_file> <section_name>")
        print("E.g python gen_ram_script.py pokeemerald.elf .rom_function")
        sys.exit(1)
    
    elf_file = sys.argv[1]
    section_name = sys.argv[2]
    
    try:
        section_data = extract_section_data(elf_file, section_name)
        
        # Write to binary file
        write_to_bin_file(section_data)
        print(f"Binary file 'ram_script.bin' created successfully with {len(section_data)} bytes.")
        
        # Print C array
        c_array = format_as_c_array(section_data)
        print("C Array:")
        print(c_array)
    except ValueError as e:
        print(f"Error: {e}")
