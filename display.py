import sys, os
from PIL import Image, ImageDraw
import numpy as np

def main():
    if len(sys.argv) != 2:
        print(sys.argv)
        print("Wrong arg")
        sys.exit(-1)
    src = sys.argv[1]
    with open(src) as f:
        lines = f.readlines()
        h, w = [int(i) for i in lines[0].split()]
        array = np.zeros(shape=(h, w, 3))
        cur_line = 1
        for i in range(h):
            for j in range(w):
                array[i][j][:] = np.array([int(t) for t in lines[cur_line].split()])[:]
                cur_line += 1
    im = Image.fromarray(np.uint8(array))
    print("Picture displayed and saved.")
    im.show()
    im.save("test.JPEG")

if __name__ == "__main__":
    main()