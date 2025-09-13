from PIL import Image
import sys

def png_to_c_bitmap(filename, array_name="bitmap"):
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
            if a == 0:
                row.append("0")  # transparent
            else:
                row.append("1")  # has colour
        c_array += ", ".join(row) + " },\n"
    c_array += "};\n"

    return c_array

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python png_to_c_bitmap.py input.png [array_name]")
        sys.exit(1)

    filename = sys.argv[1]
    array_name = sys.argv[2] if len(sys.argv) > 2 else "bitmap"

    result = png_to_c_bitmap(filename, array_name)
    print(result)
