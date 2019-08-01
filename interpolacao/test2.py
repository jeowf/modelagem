import cv2 as cv
import numpy as np

img = cv.imread('trovs.JPG', 0)
h = img.shape[0]
w = img.shape[1]

def interpol(x,L):
	return (-4/L**2)*(x**2 - x*L)

def int_ch(c, x, y):
	a = interpol(y, (0,h-1), (colors[0][c], colors[2][c])) 
	b = interpol(y, (0,h-1), (colors[1][c], colors[3][c]))

	return interpol(x, (0,w-1), (a,b))

for y in range(h):
	for x in range(w):
		img[y][x] = interpol(x, w)*img[y][x]
		

cv.imwrite('out.png', img)