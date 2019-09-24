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
	Alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']
	HorID = Alphabet.index(label[0])
	VerID = 18 - (int(label[1])*10 + int(label[2]))
	
	# x (Horrizontal) : binWidth 3.05 m, min = 0m
	# y (Vertical)    : binWidth 3.05 m, min = 0m
	# center of A18 being the origin of the coordinate system
	width = 3.048 # unit : m

	x = 0. + float(HorID)*width
	y = 0. + float(VerID)*width

	#print("Label {} : ID {}, {}, Loc {}, {}".format(label, HorID, VerID,x,y))
	return x, y

if __name__ == '__main__':
	# generate obstacels, including rooms, walls
	print("hello")

	labels = []

	#
	# room 1 : A-H, 1-5
	#
	labels_room1 = []
	labels_room1_X = ['A','B','C','D','E','F','G','H']
	labels_room1_Y = ['01','02','03','04','05']
	for x in labels_room1_X:
		for y in labels_room1_Y:
			labels_room1.append(x+y)

	for label in labels_room1:
		print("room1 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room1

	#
	# room 2 : T-Z, 2-8
	#
	labels_room2 = []
	labels_room2.append('W02')
	labels_room2.append('V03')
	labels_room2.append('W03')
	labels_room2.append('X03')
	labels_room2.append('V04')
	labels_room2.append('W04')
	labels_room2.append('X04')
	labels_room2.append('V05')
	labels_room2.append('W05')
	labels_room2.append('X05')
	labels_room2.append('Y05')
	labels_room2.append('T06') # 6
	labels_room2.append('U06')
	labels_room2.append('V06')
	labels_room2.append('W06')
	labels_room2.append('X06')
	labels_room2.append('Y06')
	labels_room2.append('T07') # 7
	labels_room2.append('U07')
	labels_room2.append('V07')
	labels_room2.append('X07')
	labels_room2.append('Y07')
	labels_room2.append('Z07')
	labels_room2.append('T08') # 8
	labels_room2.append('U08')
	labels_room2.append('V08')
	labels_room2.append('X08')
	labels_room2.append('Y08')
	labels_room2.append('Z08')

	for label in labels_room2:
		print("room2 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room2

	#
	# room 3 : A-C, 11-12 
	#
	labels_room3 = []
	labels_room3.append('B11')
	labels_room3.append('C11')
	labels_room3.append('A12')
	labels_room3.append('B12')
	labels_room3.append('C12')

	for label in labels_room3:
		print("room3 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room3

	#
	# room 4 : F-H, 10-14 
	#
	labels_room4 = []
	labels_room4.append('G10')
	labels_room4.append('H10')
	labels_room4.append('F11') # 11
	labels_room4.append('G11')
	labels_room4.append('H11')
	labels_room4.append('F12') # 12
	labels_room4.append('G12')
	labels_room4.append('H12')
	labels_room4.append('F13') # 13
	labels_room4.append('G13')
	labels_room4.append('H13')
	labels_room4.append('F14') # 14
	labels_room4.append('G14')
	labels_room4.append('H14')

	for label in labels_room4:
		print("room4 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room4

	#
	# room 5 : N-O, 11
	#
	labels_room5 = []
	labels_room5.append('N11')
	labels_room5.append('O11')

	for label in labels_room5:
		print("room5 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room5

	#
	# room 6 : Q-U, 10-12
	#
	labels_room6 = []
	labels_room6_X = ['R','S','T','U']
	labels_room6_Y = ['10','11','12']

	for x in labels_room6_X:
		for y in labels_room6_Y:
			labels_room6.append(x+y)

	labels_room6.append('Q11')

	for label in labels_room6:
		print("room6 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room6

	#
	# room 7 : Y-Z, 10-15
	#
	labels_room7 = []
	labels_room7_X = ['X','Y','Z']
	labels_room7_Y = ['10','11','12','13','14','15']

	for x in labels_room7_X:
		for y in labels_room7_Y:
			labels_room7.append(x+y)

	for label in labels_room7:
		print("room7 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room7

	#
	# room 8 : A, 13-18
	#
	labels_room8 = []
	labels_room8.append('A13')
	labels_room8.append('A14')
	labels_room8.append('A15')
	labels_room8.append('A16')
	labels_room8.append('A17')
	labels_room8.append('A18')

	for label in labels_room8:
		print("room8 : label {}, location {}".format(label,GetLocation(label)))
	
	labels = labels + labels_room8

	print(labels)

	#
	# write
	#
	with open ('data_step0_obstacles.txt','w')as g:
		for label in labels:
			x, y = GetLocation(label)
			line = label + ' ' + str(x) + ' ' + str(y) + '\n'
			g.write(line)
		g.close()
