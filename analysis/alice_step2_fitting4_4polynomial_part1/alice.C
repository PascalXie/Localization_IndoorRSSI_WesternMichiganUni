#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "UserCostFunction.hh"
#include "SteepestCostFunction.hh"
#include "NewtonsCostFunction.hh"

#include "UserResidualBlockFunction.hh"
#include "PolyResidualBlockFunction.hh"

#include "UserOptimizationManager.hh"
#include "SteepestOptimizationManager.hh"
#include "NewtonsOptimizationManager.hh"

#include "ToolNodesGenerator.hh"
#include "ToolMapGenerator.hh"
#include "ToolSignalPowerGenerator.hh"

using namespace std;

vector<double> rssis, dists;

void RaisdualTest();
void Analysis(double P0, double gamma, double min, double max);

int main()
{
	cout<<"Hello "<<endl;

	// step 0 : read
	string filename = "../../data_step1_observations.txt";
	ifstream read(filename);

	if(read.fail())
	{
		cout<<"file "<<filename<<" dose not exist"<<endl;
		return 1;
	}

	string locLabel;
	double tarx, tary;
	double rssi;
	double ancx, ancy;
	double dist;
	double time;

	while(!read.eof())
	{
		read>>locLabel>>tarx>>tary>>rssi>>ancx>>ancy>>dist>>time;
		cout<<"time "<<time<<endl;

		if(read.eof()) break;

		if(rssi<-100) continue;

		rssis.push_back(rssi);
		dists.push_back(dist);
	}
	read.close();



	// debug
	RaisdualTest();

	// step 1 : path loss exponent estimation
	//
	// Optimization 
	//
	int observationsSize = 2;
	int residualSize = 1;
	int varialbeSize = 4;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	variables.push_back(-0.);
	variables.push_back(0.);
	variables.push_back(0.);
	variables.push_back(0.);
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	//  observations
	for(int i=0;i<rssis.size();i++)
	//for(int i=0;i<150;i++)
	{
		int ID = i;

		if(dists[ID]<=0.1) continue;

		vector<double> observation_current;
		observation_current.push_back(rssis[ID]);
		observation_current.push_back(dists[ID]);
		costFunction->AddResidualBlock(observation_current);
	}

	//
	cout<<" "<<endl;
	cout<<"alice SetUserInitialization"<<endl;
	manager->SetUserInitialization(costFunction);

	//
	double UserReferencedLength = 70.;
	manager->SetUserReferencedLength(UserReferencedLength);

	// 
	double UserReferencedEpsilon = 1e-2;
	manager->SetUserEpsilonForTerminating(UserReferencedEpsilon);

	// initialize
	cout<<" "<<endl;
	cout<<"Initialize "<<endl;
	manager->Initialize();

	//
	vector<double> results;
	manager->GetVariables(results);

	cout<<"Fitting results : a0 "<<results[0]<<", a1 "<<results[1]<<", a2 "<<results[2]<<", a3 "<<results[3]<<endl;

	ofstream write("data_step2_polynomial.txt");
	write<<"a0 "<<results[0]<<endl;
	write<<"a1 "<<results[1]<<endl;
	write<<"a2 "<<results[2]<<endl;
	write<<"a3 "<<results[3]<<endl;
	write.close();

	// test
	for(int i=0;i<10;i++)
	{
		double dt = 0 + double(i)*3.;
		double rssit =  results[0] + results[1]*dt + results[2]*dt*dt + results[3]*dt*dt*dt;
		cout<<"ID "<<i<<": distance "<<dt<<", rssi "<<rssit<<endl;
	}


	return 1;
}

void RaisdualTest()
{
	int observationsSize = 2;
	int residualSize = 1;
	int varialbeSize = 2;

	vector<double> observation_current;
	observation_current.push_back(rssis[0]);
	observation_current.push_back(dists[0]);

	UserResidualBlockFunction *poly = new PolyResidualBlockFunction("rssi",observation_current,observationsSize,varialbeSize,residualSize);

	vector<double> variables, residual;
	variables.push_back(0);
	variables.push_back(0);

	poly->ResidualFunction(variables,residual);

}
