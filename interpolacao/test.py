import cv2 as cv
import numpy as np

h = 200
w = 300

img = np.zeros((h,w,3), np.uint8)

colors = [(255,0,0), (0,0,0),
          (0,0,255),(0,255,0)]


def interpol(v,x,y):
	return y[0] + (y[1] - y[0])*((v-x[0])/(x[1] - x[0]))

def int_ch(c, x, y):
	a = interpol(y, (0,h-1), (colors[0][c], colors[2][c])) 
	b = interpol(y, (0,h-1), (colors[1][c], colors[3][c]))

	return interpol(x, (0,w-1), (a,b))

for y in range(h):
	for x in range(w):
		r = int_ch(0, x, y)
		g = int_ch(1, x, y)
		b = int_ch(2, x, y)
		img[y][x] = (b,g,r)
		

cv.imwrite('out.png', img)