# Screen Size = 320 x 480

import os

from PIL import Image

current_dir = os.path.dirname(os.path.abspath(__file__))
path_dir_icons = os.path.join(current_dir, "icons")
os.makedirs(path_dir_icons, exist_ok=True)
path_icon_header = os.path.join(current_dir, "Icons.h")

# --- To be customized ---
gif_width = 8
gif_height = 8
icon_name = "mario"
# ------------------------

def define_struct():
    struct_content = """typedef struct {
    uint16_t *frames;
    uint16_t width;
    uint16_t height;
    uint16_t count;
} IconSequence;"""
    return struct_content

def generate_icon_array(filename: str):
    path_icon = os.path.join(path_dir_icons, filename)
    
    if not os.path.exists(path_icon):
        raise Exception("Icon file does not exist!")
    
    bitmaps_content = f"static const uint16_t PROGMEM icon_{icon_name.replace('.','_')}_bitmaps[] = {{ "

    with Image.open(path_icon) as img:
        if not hasattr(img, 'n_frames'):
            raise Exception("Frame count not defined") 
        gif_frames = img.n_frames

    with open(path_icon, "rb") as f:
        gif = Image.open(f)
        
        for frame_number in range(gif.n_frames):
            if frame_number >= gif_frames:
                break
            gif.seek(frame_number)
            img = gif.convert("RGBA").resize((gif_width, gif_height), Image.NEAREST)
            # img.save(f"frame_{frame_number}.png")  # for debug
            w, h = img.size
            
            for y in range(h):
                for x in range(w):
                    r, g, b, a = img.getpixel((x, y))
                    
                    if a <= 0:
                        r, g, b = 0, 0, 0
                    
                    # Convert to rgb565
                    r_565 = (r >> 3) & 0x1F      # 5 bits
                    g_565 = (g >> 2) & 0x3F      # 6 bits
                    b_565 = (b >> 3) & 0x1F      # 5 bits
                    
                    rgb565 = (r_565 << 11) | (g_565 << 5) | b_565
                    
                    rgb565 = ((rgb565 & 0xFF) << 8) | ((rgb565 >> 8) & 0xFF)
                    
                    bitmaps_content += f"0x{rgb565:04X}, "
    print(f"Generated icon '{icon_name}' with {gif.n_frames} frames")
    total_pixels = gif_width * gif_height * gif.n_frames
    print(f"Expected total pixels: {total_pixels}")
    print(f"Actual total pixels: {len(bitmaps_content.split(',')) - 1}")
                    
        
    bitmaps_content += "};"
    sequence_content = f"static const IconSequence PROGMEM icon_{icon_name.replace('.','_')} = {{ (uint16_t *) icon_{icon_name.replace('.','_')}_bitmaps, {gif_width}, {gif_height}, {gif.n_frames} }};"
    struct_content = define_struct()

    # write to header file
    with open(path_icon_header, "w") as header_file:
        header_file.write("#include <cstdint>")
        header_file.write("\n" + struct_content + "\n")
        header_file.write("\n" + bitmaps_content + "\n")
        header_file.write(sequence_content + "\n")
        header_file.write("\n")
            
if __name__ == "__main__":
    generate_icon_array(f"{icon_name}.gif")
