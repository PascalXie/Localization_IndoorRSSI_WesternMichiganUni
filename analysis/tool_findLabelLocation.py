import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns
import re

def GetLocationLabel(anchorName):
	if anchorName=='b3001': return 'F09'
	if anchorName=='b3002': return 'J04'
	if anchorName=='b3003': return 'N04'
	if anchorName=='b3004': return 'S04'
	if anchorName=='b3005': return 'J07'
	if anchorName=='b3006': return 'N07'
	if anchorName=='b3007': return 'S07'
	if anchorName=='b3008': return 'J10'
	if anchorName=='b3009': return 'D15'
	if anchorName=='b3010': return 'J15'
	if anchorName=='b3011': return 'N15'
	if anchorName=='b3012': return 'R15'
	if anchorName=='b3013': return 'W15'

	return 'NONE'

def GetLocation(label):
	Alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X']
	HorID = Alphabet.index(label[0])
	VerID = 18 - (int(label[1])*10 + int(label[2]))
	
	# x (Horrizontal) : binWidth 3.05 m, min = 0m
	# y (Vertical)    : binWidth 3.05 m, min = 0m
	# center of A18 being the origin of the coordinate system
	width = 3.048 # unit : m

	x = 0. + float(HorID)*width
	y = 0. + float(VerID)*width

	print("Label {} : ID {}, {}, Loc {}, {}".format(label, HorID, VerID,x,y))
	return x, y

if __name__ == '__main__':
	print("hello")
	GetLocation('N11')

	iBeacons = []
	iBeacons.append('b3001')
	iBeacons.append('b3002')
	iBeacons.append('b3003')
	iBeacons.append('b3004')
	iBeacons.append('b3005')
	iBeacons.append('b3006')
	iBeacons.append('b3007')
	iBeacons.append('b3008')
	iBeacons.append('b3009')
	iBeacons.append('b3010')
	iBeacons.append('b3011')
	iBeacons.append('b3012')
	iBeacons.append('b3013')

	# write file
	with open ('data_tool_label_location.txt','w')as g:
		for name in iBeacons:
			label = GetLocationLabel(name)
			x, y  = GetLocation(label)
			line = ''
			line = line + str(name) + ' '
			line = line + str(label) + ' '
			line = line + str(x) + ' '
			line = line + str(y) + '\n'
			g.write(line)
		g.close()

