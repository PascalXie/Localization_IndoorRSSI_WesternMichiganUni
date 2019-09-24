import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns
import math

if __name__ == '__main__':
	print("hello")
	f = open('data_step1_observations.txt')
	lines = f.readlines()

	rssi = []
	dist = []
	for line in lines:
		line = line.strip().split()
		rssi.append(float(line[3]))
		dist.append(float(line[6]))
		#print(line)

#	# shadowing model
#	size = 15
#	P_0 = -64.0432
#	gamma = 1.30937
#	rssi_m_list = []
#	dist_m_list = []
#	for ID in range(size):
#		width = (30.-1.)/size
#		dist_m = 1 + width*float(ID)
#		rssi_m = P_0 - 10.*gamma*(math.log(dist_m/1.,10.))
#		print("{} {}".format(dist_m,rssi_m))
#		rssi_m_list.append(rssi_m)
#		dist_m_list.append(dist_m)

	# read
	f = open('alice_step2_fitting3_polynomial_part2_noiseAnalysis/build/data_step2_Gaussians.txt')
	lines = f.readlines()
	rssi_m_list = []
	dist_m_list = []
	stdDev_list = []
	rssi_upper  = []
	rssi_lower  = []
	rssi_upper_2  = []
	rssi_lower_2  = []
	for line in lines:
		line = line.strip().split()
		rssi_m_list.append(float(line[0]))
		stdDev_list.append(float(line[1]))
		dist_m_list.append(float(line[2]))
		rssi_upper.append(float(line[0])+float(line[1])*1.)
		rssi_lower.append(float(line[0])-float(line[1])*1.)
		rssi_upper_2.append(float(line[0])+float(line[1])*2.)
		rssi_lower_2.append(float(line[0])-float(line[1])*2.)

	print("rssi_upper {}".format(rssi_upper))


	#
	# plot: rssi
	#
	color = ['tab:blue', 'tab:orange', 'tab:green']
	fig = plt.figure(figsize=(6,5))
	plt.scatter(dist, rssi, marker='s',s=40,c=color[0],alpha=0.4,edgecolors='none',label='Target')
	#plt.plot(dist_m_list, rssi_m_list,marker='o', c='brown', lw=2, mec='m', mfc='w',label='Shadowing Model')
	plt.plot(dist_m_list, rssi_m_list, c='brown', lw=2,label='Shadowing Model')
	plt.fill_between(dist_m_list,rssi_m_list,rssi_upper,  color=color[1], alpha=0.7,label='32%-68%')
	plt.fill_between(dist_m_list,rssi_m_list,rssi_lower,  color=color[1], alpha=0.7)
	plt.fill_between(dist_m_list,rssi_upper ,rssi_upper_2,color=color[1], alpha=0.45,label='5%-95%')
	plt.fill_between(dist_m_list,rssi_lower ,rssi_lower_2,color=color[1], alpha=0.45)
	plt.legend(frameon=True)
	plt.xlabel('Distance / m',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("RSSI",fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.xlim(-5,35)
	plt.ylim(-90,-50)
	plt.title('RSSI Distribution and Fitting Result')
	plt.savefig('figure-RSSI-Distance_fitting3.png',dpi=300)

	plt.show()
