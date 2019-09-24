import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns
import math

def GetErrorData(path):
	print("Data File Path : {}".format(path))
	# read
	f = open(path+'/build/data_step3_localizationErrors.txt')
	lines = f.readlines()

	binCentors = []
	contents   = []

	for line in lines:
		line = line.strip().split()

		binCentors	.append(float(line[1]))
		contents	.append(float(line[2])*100.)

	return binCentors, contents

if __name__ == '__main__':
	print("hello")

	# read
	f = open('data_step1_observations.txt')
	lines = f.readlines()

	ancxs = []
	ancys = []
	for line in lines:
		line = line.strip().split()
		ancxs.append(float(line[4]))
		ancys.append(float(line[5]))

	# read
	#f = open('alice_step3_leastMeanSquares/build/data_step3_localizationResults.txt')
	#f = open('alice_step3_rssi-newtonsmethod-shadowing_2_bayesianAnchor/build/data_step3_localizationResults.txt')
	#f = open('alice_step3_rssi-newtonsmethod-shadowing_1_basic/build/data_step3_localizationResults.txt')
	#f = open('alice_step3_rssi-newtonsmethod-shadowing_3_bayesianObstacles/build/data_step3_localizationResults.txt')
	f = open('alice_step3_rssi-newtonsmethod-shadowing_5_trilaterationAndBayesian/build/data_step3_localizationResults.txt')
	lines = f.readlines()

	tarxs = []
	tarys = []
	gt_tarxs = [] # ground truth
	gt_tarys = []
	errorxs = []
	errorys = []
	sdtDevxs = []
	sdtDevys = []

	errors = []
	stdDevs = []

	for line in lines:
		line = line.strip().split()

		if float(line[0])<0 or float(line[0])>60: continue
		if float(line[1])<0 or float(line[1])>70: continue

		tarxs	.append(float(line[0]))
		tarys	.append(float(line[1]))
		gt_tarxs.append(float(line[2]))
		gt_tarys.append(float(line[3]))
		errorxs	.append(float(line[4]))
		sdtDevxs.append(float(line[5]))
		errorys	.append(float(line[6]))
		sdtDevys.append(float(line[7]))

		errors	.append(10.*math.log10((float(line[4]))**2+(float(line[6]))**2))
		stdDevs	.append(1.0*((float(line[5]))**2+(float(line[7]))**2))

	# read errors
	paths = []
	paths.append('alice_step3_leastMeanSquares')
	paths.append('alice_step3_rssi-newtonsmethod-shadowing_1_basic')
	paths.append('alice_step3_rssi-newtonsmethod-shadowing_2_bayesianAnchor')
	paths.append('alice_step3_rssi-newtonsmethod-shadowing_3_bayesianObstacles')
	paths.append('alice_step3_rssi-newtonsmethod-shadowing_4_bayesianAnchorAndObstacles')
	paths.append('alice_step3_rssi-newtonsmethod-shadowing_5_trilaterationAndBayesian')

	situations = []
	situations.append(GetErrorData(paths[0]))
	situations.append(GetErrorData(paths[5]))
	situations.append(GetErrorData(paths[1]))
	situations.append(GetErrorData(paths[2]))
	situations.append(GetErrorData(paths[3]))
	situations.append(GetErrorData(paths[4]))

	#
	# plot
	#
	color = ['tab:blue', 'tab:orange', 'tab:green']

	# 
	fig = plt.figure(figsize=(6,5))
	plt.plot(situations[0][0], situations[0][1],ls='-', linewidth=1.0,color='k', marker='s', mec='k', mfc='w', label='Multilateration')
	plt.plot(situations[1][0], situations[1][1],ls='-', linewidth=1.0,color='m', marker='o', mec='m', mfc='w', label='Multilateration-NewtonBayesian')
#	plt.plot(situations[2][0], situations[2][1],ls='-', linewidth=1.0,color='k', marker='.', mec='k', mfc='w', label='Newton')
#	plt.plot(situations[3][0], situations[3][1],ls='--',linewidth=1.0,color='k', marker='v', mec='k', mfc='w', label='NewtonBayesianForAcnhors')
#	plt.plot(situations[4][0], situations[3][1],ls='--',linewidth=1.0,color='k', marker='^', mec='k', mfc='w', label='NewtonBayesianForObstacles')
#	plt.plot(situations[5][0], situations[3][1],ls='-', linewidth=1.0,color='k', marker='*', mec='k', mfc='w', label='NewtonBayesian')
	plt.legend(frameon=True)
	plt.xlabel('Error / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("Weight / %",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.xlim(0,30)
	#plt.ylim(0,60)
	plt.title('Distribution of Errors')
	plt.savefig('figure-DistributionOfErrors.png',dpi=300)


	#
	fig = plt.figure(figsize=(6,5))
	plt.scatter(ancxs, ancys, marker='o',s=80,c=color[2],alpha=1.0,edgecolors='none',label='iBeacon')
	plt.scatter(gt_tarxs, gt_tarys, marker='s',s=40,c=color[1],alpha=0.8,edgecolors='none',label='Target-Ground Truth')
	plt.scatter(tarxs, tarys, marker='o',s=errors,c=color[0],alpha=0.4,edgecolors='none',label='Target')
	plt.scatter(tarxs, tarys, marker='o',s=stdDevs,c='r',alpha=0.8,edgecolors='none',label='Target')
	plt.legend(frameon=True)
	plt.xlabel('X / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("Y / m",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.axis('off')
	plt.xlim(0,70)
	plt.ylim(0,60)
	#plt.title('Localization Results')
	plt.savefig('figure-LocalizationResults-alpha.png',transparent=True,dpi=300)


	#
	fig = plt.figure(figsize=(6,5))
	plt.scatter(ancxs, ancys, marker='o',s=80,c=color[2],alpha=1.0,edgecolors='none',label='iBeacon')
	plt.scatter(gt_tarxs, gt_tarys, marker='s',s=40,c=color[1],alpha=0.8,edgecolors='none',label='Target-Ground Truth')
	plt.scatter(tarxs, tarys, marker='o',s=errors,c=color[0],alpha=0.4,edgecolors='none',label='Target')
	plt.legend(frameon=True)
	plt.xlabel('X / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("Y / m",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.xlim(0,70)
	plt.ylim(0,60)
	plt.title('Localization Results')
	plt.savefig('figure-LocalizationResults.png',dpi=300)

	plt.show()
