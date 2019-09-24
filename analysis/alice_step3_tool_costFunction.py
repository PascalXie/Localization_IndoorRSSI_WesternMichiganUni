import math
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm

import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def drawAPlot(ID, dataFilePath):
	dataFileName = dataFilePath + "data_step3_tool_costFunction_" + str(ID) + ".txt"
	print(dataFileName)

	data = open(dataFileName)
	lines = data.readlines()
	data.close()

	xs = []
	ys = []
	values = []
	values_log = []
	for line in lines:
		line = line.strip().split()
		xs.append(float(line[0]))
		ys.append(float(line[1]))

		value = float(line[2])
		values.append(value)

		value_log = -1.*math.log(value)
		values_log.append(value_log)


	df = pd.DataFrame({'y':ys,'x': xs,'CostFunctionValues':values, 'logValue':values_log})
	pt = df.pivot_table(index='y', columns='x', values='CostFunctionValues', aggfunc=np.sum)
	pt_log = df.pivot_table(index='y', columns='x', values='logValue', aggfunc=np.sum)

	f, (ax1) = plt.subplots(figsize = (9,8),nrows=1)
	cmap = sns.diverging_palette(200,20,sep=20,as_cmap=True)
	sns_plot = sns.heatmap(pt_log, linewidths = 0.00, ax = ax1, cmap="RdPu", xticklabels=8, yticklabels=8)
	sns_plot.tick_params(labelsize=10)

	ax1.invert_yaxis()
	ax1.set_title('Cost Function Distribution')
	ax1.set_xlabel('X')
	ax1.set_ylabel('Y')
	plt.savefig('figure-tool-DistributionOfCostFunction_'+str(ID)+'.png',dpi=300)

	return

if __name__ == '__main__':
	print("hello")

	for ID in range(20):
		#dataFilePath = "alice_step3_rssi-newtonsmethod-shadowing_2_bayesianAnchor/build/"
		dataFilePath = "alice_step3_rssi-newtonsmethod-shadowing_3_bayesianObstacles/build/"
		drawAPlot(ID, dataFilePath)


#	#
#	ID = 0
#	#dataFileName = "alice_step3_rssi-newtonsmethod-shadowing_1_basic/build/data_step3_tool_costFunction_" + str(ID) + ".txt"
#	#dataFileName = "alice_step3_rssi-newtonsmethod-shadowing_2_bayesianAnchor/build/data_step3_tool_costFunction_" + str(ID) + ".txt"
#	dataFileName = "alice_step3_rssi-newtonsmethod-shadowing_3_bayesianObstacles/build/data_step3_tool_costFunction_" + str(ID) + ".txt"
#	data = open(dataFileName)
#	lines = data.readlines()
#	data.close()
#
#	xs = []
#	ys = []
#	values = []
#	values_log = []
#	for line in lines:
#		line = line.strip().split()
#		xs.append(float(line[0]))
#		ys.append(float(line[1]))
#
#		value = float(line[2])
#		values.append(value)
#
#		value_log = -1.*math.log(value)
#		values_log.append(value_log)
#
#
#	df = pd.DataFrame({'y':ys,'x': xs,'CostFunctionValues':values, 'logValue':values_log})
#	pt = df.pivot_table(index='y', columns='x', values='CostFunctionValues', aggfunc=np.sum)
#	pt_log = df.pivot_table(index='y', columns='x', values='logValue', aggfunc=np.sum)
#
#	f, (ax1) = plt.subplots(figsize = (9,8),nrows=1)
#	cmap = sns.diverging_palette(200,20,sep=20,as_cmap=True)
#	sns_plot = sns.heatmap(pt_log, linewidths = 0.00, ax = ax1, cmap="RdPu", xticklabels=8, yticklabels=8)
#	sns_plot.tick_params(labelsize=10)
#
#	ax1.invert_yaxis()
#	ax1.set_title('Cost Function Distribution')
#	ax1.set_xlabel('X')
#	ax1.set_ylabel('Y')
#	plt.savefig('figure-tool-DistributionOfCostFunction.png',transparent=True,dpi=300)
#	
#	#plt.show()
