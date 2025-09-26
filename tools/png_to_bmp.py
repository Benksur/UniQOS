from PIL import Image
import sys

def png_to_c_bitmap(filename, array_name="bitmap", alpha_threshold=128, brightness_threshold=128):
    # Open image with Pillow
    img = Image.open(filename).convert("RGBA")
    width, height = img.size
    pixels = img.load()

    # Build C array string
    c_array = f"const unsigned char {array_name}[{height}][{width}] = {{\n"
    for y in range(height):
        c_array += "    { "
        row = []
        for x in range(width):
            r, g, b, a = pixels[x, y]
            brightness = (r + g + b) // 3
            if a < alpha_threshold or brightness < brightness_threshold:
                row.append("0")  # transparent or dim
            else:
                row.append("1")  # sufficiently opaque and bright
        c_array += ", ".join(row) + " },\n"
    c_array += "};\n"

    return c_array

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python png_to_c_bitmap.py input.png [array_name] [alpha_threshold] [brightness_threshold]")
        sys.exit(1)

    filename = sys.argv[1]
    array_name = sys.argv[2] if len(sys.argv) > 2 else "bitmap"
    alpha_threshold = int(sys.argv[3]) if len(sys.argv) > 3 else 128
    brightness_threshold = int(sys.argv[4]) if len(sys.argv) > 4 else 128

    result = png_to_c_bitmap(filename, array_name, alpha_threshold, brightness_threshold)
    print(result)
