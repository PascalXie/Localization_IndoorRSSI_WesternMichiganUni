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

#include "Histogram.hh"

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

		if(rssi<-150) continue;

		rssis.push_back(rssi);
		dists.push_back(dist);
	}
	read.close();


	// step 0 : path loss exponent 
	string filename2 = "../../alice_step2_fitting1_pathLossExponent_part1/build/data_step2_pathLossExponent.txt";
	ifstream read2(filename2);

	if(read2.fail())
	{
		cout<<"file "<<filename2<<" dose not exist"<<endl;
		return 1;
	}

	string name;
	double P0;
	double gamma;

	read2>>name>>P0;
	read2>>name>>gamma;
	cout<<"Fitting Results : P0 "<<P0<<", Gamma "<<gamma<<endl;


	//
	ofstream write("data_step2_Gaussians.txt");

	// 
	int binSize = 10;
	double min = -1.4;
	double max = 33.6;
	double binWidth = (max-min)/double(binSize);

	for(int i=0;i<binSize;i++)
	{
		int ID = i;
		double current_min = min + (double(ID))*binWidth;
		double current_max = min + (double(ID)+1.)*binWidth;
		cout<<"ID "<<ID<<", current_min "<<current_min<<", current_max "<<current_max<<endl;
		Analysis(P0, gamma, current_min, current_max);
	}

	return 1;
}

void Analysis(double P0, double gamma, double min, double max)
{
	// step 1 : compute P_, P_ = P0 - 10* gamma * log10(d/d0) 
	vector<int> IDUsed;
	double mean_dist = (min+max)/2.;
	double mean_rssi = P0 - 10.*log10(mean_dist/1.);

	for(int i=0;i<rssis.size();i++)
	{
		int ID = i;
		if(dists[ID]>=min&&dists[ID]<max)
		{
			IDUsed.push_back(ID);
		}
	}

	// step 2 : histogram
	int binSize = 20;
	double hmin = -100;
	double hmax = -50;
	double hbinWidth = (hmax-hmin)/double(binSize);
	Histogram * h = new Histogram("rssi",binSize,hmin,hmax);

	for(int i=0;i<IDUsed.size();i++)
	{
		int ID = i;
		h->Fill(rssis[ID],1);
	}

	// normalization
	h->Normalize();

	//
	// Optimization 
	//
	int observationsSize = 3;
	int residualSize = 1;
	int varialbeSize = 1;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	variables.push_back(5.);
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	//  observations
	for(int i=0;i<binSize;i++)
	{
		int ID = i;

		double content = h->GetBinContent(ID)/hbinWidth; // pdf
		double rssi = h->GetBinCenter(ID);

		if(content<=0) continue;

		vector<double> observation_current;
		observation_current.push_back(mean_rssi);
		observation_current.push_back(content);
		observation_current.push_back(rssi);
		costFunction->AddResidualBlock(observation_current);

		cout<<"ID "<<ID<<", rssi "<<rssi<<", content "<<content<<endl;
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

	cout<<"Fitting results : sigma "<<results[0]<<", P_mean "<<mean_rssi<<endl;

	// write
	ofstream write("data_step2_Gaussians.txt", ios::app);
	double dist = (min+max)/2.; 
	double sigma = results[0];
	write<<mean_rssi<<" "<<sigma<<" "<<dist<<endl;
	write.close();

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
