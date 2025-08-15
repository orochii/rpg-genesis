import sys
from PIL import Image
from pathlib import Path

PALLETE = [
    [0,0,0],
    [255,0,52]
]
PALLETE_LEN = len(PALLETE)

def find_index(p):
    for i in range(PALLETE_LEN):
        c = PALLETE[i]
        if (c[0]==p[0] and c[1]==p[1] and c[2]==p[2]):
            return i
    return 0

n = len(sys.argv)
argv = sys.argv
if n > 1:
    print(argv)
    filename = argv[1]
    basename = Path(filename).stem
    im = Image.open(filename)
    pix = im.load()
    width_px, height_px = im.size
    w = int(width_px / 8)
    h = int(height_px / 8)
    coll_data = ""
    count = 0
    for y in range(h):
        for x in range(w):
            pixel = pix[x*8,y*8]
            v = find_index(pixel)
            coll_data += str(v) + ","
            count += 1
            if count >= w:
                coll_data += "\n"
                count = 0
    with open(basename+".h", "w") as text_file:
        uppername = basename.upper()
        size = w * h
        text_file.write("const u16 " + uppername + "[" + str(size) + "] =\n{\n")
        text_file.write(coll_data)
        if (count > 0):
            text_file.write("\n")
        text_file.write("};\n")
        text_file.write("const u16 " + uppername + "_W = " + str(w) + ";\n")
        text_file.write("const u16 " + uppername + "_H = " + str(h) + ";\n")