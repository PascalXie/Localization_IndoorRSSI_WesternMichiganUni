import numpy as np
import matplotlib.pyplot as plt
import csv
import string
import seaborn as sns
import math

if __name__ == '__main__':
	print("hello")

	# read
	f = open('alice_step3_rssi-newtonsmethod-shadowing/build/data_step3_localizationDetails.txt')
	lines = f.readlines()

	samples		= {}
	sample_errors	= {}

	IDs			= []
	tarxs		= []
	tarys		= []
	mean_tarxs	= []
	mean_tarys	= []
	error_dists = []
	ancxs		= []
	ancys		= []
	for line in lines:
		line = line.strip().split()
		IDs			.append(int  (line[0]))
		tarxs		.append(float(line[1]))
		tarys		.append(float(line[2]))
		mean_tarxs	.append(float(line[3]))
		mean_tarys	.append(float(line[4]))
		error_dists .append(float(line[5]))
		ancxs		.append(float(line[6]))
		ancys		.append(float(line[7]))

		samples[int(line[0])] = [float(line[5])]
		sample_errors[int(line[0])]	= [float(line[5])]

		#print(line)


	#
	for i in range(len(IDs)):
		anchorID = ancxs[i]*1e6 + ancys[i]

		sampleID = IDs[i]
		if not anchorID in samples[sampleID]:
			samples[sampleID].append(anchorID)

	#print(samples)
	#print(sample_errors)

	#
	AnchorNumbers	= [len(samples[key])-1 for key in samples]
	Errors			= [samples[key][0] for key in samples]

	# debug
	#for ele in AnchorNumbers:
	#	print(ele)

	#for ele in Errors:
	#	print(ele)

	#
	# plot
	#
	color = ['tab:blue', 'tab:orange', 'tab:green']
	fig = plt.figure(figsize=(6,5))
	plt.scatter(AnchorNumbers, Errors, marker='o',s=80,c=color[1],alpha=0.1,edgecolors='none',label='')
	plt.xlabel('Anchor Number',fontdict={'family' : 'Times New Roman', 'size': 12})
	plt.ylabel("Error / m",fontdict={'family' : 'Times New Roman', 'size': 12})
	#plt.xlim(0,70)
	plt.ylim(0,50)
	#plt.title('Localization Results')
	plt.savefig('figure-anchorNumber-Error.png',transparent=True,dpi=300)
	plt.show()


