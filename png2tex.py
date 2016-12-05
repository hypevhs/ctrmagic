#!/usr/bin/env python
# converts png files into rgba8 format (though it's really just ABGR ABGR ABGR)
# that crazy morton encoding is only within 8x8 blocks of pixels, and then the
# order of *those* blocks is how you'd expect: reading order.
# WARNING! the png files must have 3 planes, no greyscale, and no alpha channel.
# example = {
#  'bitdepth': 8, 'interlace': 0, 'planes': 3,
#  'greyscale': False, 'alpha': False, 'gamma': 0.45455, 'size': (256, 256)
# }

import png
import struct

def tex2png():
    fi = open("data/kitten.bin", "rb")
    fo = open("kitten.png", "wb")
    w = 64
    h = 64
    writer = png.Writer(w, h)

    pic = [[0 for a in range(w*3)] for b in range(h)]

    for cellY in range(h / 8):
        for cellX in range(w / 8):
            pixIndexCell = 0 # the pixel index for this 8x8 cell
            for y in range(8):
                for x in range(8):
                    offsetXForCell = DecodeMorton2X(pixIndexCell)
                    offsetYForCell = DecodeMorton2Y(pixIndexCell)
                    pixIndexCell += 1

                    b = struct.unpack('B', fi.read(1))[0]
                    g = struct.unpack('B', fi.read(1))[0]
                    r = struct.unpack('B', fi.read(1))[0]

                    realX = cellX*8 + offsetXForCell
                    realY = cellY*8 + offsetYForCell

                    pic[realY][(realX*3)+0] = r
                    pic[realY][(realX*3)+1] = g
                    pic[realY][(realX*3)+2] = b

    writer.write(fo, pic)
    fo.close()

def png2tex():
    fi = open("ybody.png", "rb")
    fo = open("ybody.bin", "wb")
    reader = png.Reader(fi)
    pObj = reader.read()
    w = pObj[0]
    h = pObj[1]
    pngRows = list(pObj[2])
    for cellY in range(h / 8):
        for cellX in range(w / 8):
            # for each cell, print out values for the cell in morton-order
            for mortonIndex in range(8*8):
                realX = cellX*8 + DecodeMorton2X(mortonIndex)
                realY = cellY*8 + DecodeMorton2Y(mortonIndex)
                r = pngRows[realY][realX*3]
                g = pngRows[realY][realX*3+1]
                b = pngRows[realY][realX*3+2]
                a = 255
                ba = struct.pack('B', a)
                bb = struct.pack('B', b)
                bg = struct.pack('B', g)
                br = struct.pack('B', r)
                fo.write(ba)
                fo.write(bb)
                fo.write(bg)
                fo.write(br)
    fo.close()

def EncodeMorton2(x, y):
    return (Part1By1(y) << 1) + (Part1By1(x) << 0)

# "Insert" a 0 bit after each of the 16 low bits of x
def Part1By1(x):
    x &= 0x0000ffff                  # x = ---- ---- ---- ---- fedc ba98 7654 3210
    x = (x ^ (x <<  8)) & 0x00ff00ff # x = ---- ---- fedc ba98 ---- ---- 7654 3210
    x = (x ^ (x <<  4)) & 0x0f0f0f0f # x = ---- fedc ---- ba98 ---- 7654 ---- 3210
    x = (x ^ (x <<  2)) & 0x33333333 # x = --fe --dc --ba --98 --76 --54 --32 --10
    x = (x ^ (x <<  1)) & 0x55555555 # x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
    return x

# Inverse of Part1By1 - "delete" all odd-indexed bits
def Compact1By1(x):
    x &= 0x55555555                  # x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
    x = (x ^ (x >>  1)) & 0x33333333 # x = --fe --dc --ba --98 --76 --54 --32 --10
    x = (x ^ (x >>  2)) & 0x0f0f0f0f # x = ---- fedc ---- ba98 ---- 7654 ---- 3210
    x = (x ^ (x >>  4)) & 0x00ff00ff # x = ---- ---- fedc ba98 ---- ---- 7654 3210
    x = (x ^ (x >>  8)) & 0x0000ffff # x = ---- ---- ---- ---- fedc ba98 7654 3210
    return x

def DecodeMorton2X(code):
    return Compact1By1(code >> 0)

def DecodeMorton2Y(code):
    return Compact1By1(code >> 1)

if __name__ == "__main__":
    #tex2png()
    png2tex()
