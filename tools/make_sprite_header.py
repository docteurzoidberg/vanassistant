import numpy as np
from PIL import Image
import os
import argparse

def convert_png_to_raw_data(png_file):
    img = Image.open(png_file)
    img = img.convert("RGBA")  # Ensure the image is in RGBA format
    data = np.array(img)
    height, width, _ = data.shape
    
    # Flatten the image data
    flat_data = data.flatten().tolist()
    
    return width, height, flat_data

def generate_header_file(png_file, width, height, raw_data, output_file):
    sprite_name = os.path.splitext(os.path.basename(png_file))[0].upper()
    header_guard = f"{sprite_name}_SPRITE_H"
    with open(output_file, 'w') as f:
        f.write(f"#ifndef {header_guard}\n")
        f.write(f"#define {header_guard}\n\n")
        f.write(f"// Generated sprite data for {sprite_name}\n")
        f.write(f"const int {sprite_name}_SPRITE_WIDTH = {width};\n")
        f.write(f"const int {sprite_name}_SPRITE_HEIGHT = {height};\n")
        f.write(f"const unsigned int {sprite_name}_SPRITE_DATA[] = {{\n")
        
        # Write the raw data in C++ array format
        for i in range(0, len(raw_data), 4):
            rgba = raw_data[i:i+4]
            f.write(f"    0x{rgba[0]:02X}{rgba[1]:02X}{rgba[2]:02X}{rgba[3]:02X},\n")
        
        f.write(f"}};\n\n")
        f.write(f"#endif // {header_guard}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert PNG to C++ header file.')
    parser.add_argument('png_file', type=str, help='Path to the PNG file')
    parser.add_argument('output_file', type=str, help='Path to the output header file')
    
    args = parser.parse_args()
    
    png_file = args.png_file
    output_file = args.output_file
    
    width, height, raw_data = convert_png_to_raw_data(png_file)
    generate_header_file(png_file, width, height, raw_data, output_file)
    print(f"Header file '{output_file}' generated successfully.")
