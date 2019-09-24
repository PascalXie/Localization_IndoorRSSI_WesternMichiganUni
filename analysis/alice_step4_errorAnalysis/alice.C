#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "Histogram.hh"
#include "Histogram2D.hh"


using namespace std;

int SampleID_ = 0;

vector<double> rssis, dists;
vector<double> tarxs, tarys;
vector<double> ancxs, ancys;
vector<double> times;

vector<double> error_dists;

double Power_ref;
double PathLossExponent;
double d0;

bool PowerRSSI_residualTest();
bool PowerRSSI(double time_center, double time_halfWidth, int &ID_);
int RSSI();

int main()
{
	cout<<"Hello "<<endl;

	// step 0 : Data_PathLossExponent
	string filename0 = "../../alice_step2_fitting1_pathLossExponent_part1/build/data_step2_pathLossExponent.txt";
	ifstream file(filename0.c_str());

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename0<<" \""<<endl;
		return 0;
	}

	string temp;

	file>>temp>>Power_ref;
	file>>temp>>PathLossExponent;
	file>>temp>>d0;

	cout<<"Power_ref "<<Power_ref<<endl;
	cout<<"PathLossExponent "<<PathLossExponent<<endl;
	cout<<"d0 "<<d0<<endl;

	// step 0 : read observations
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

		if(read.eof()) break;

		if(rssi<-199) continue;

		rssis.push_back(rssi);
		dists.push_back(dist);
		tarxs.push_back(tarx);
		tarys.push_back(tary);
		ancxs.push_back(ancx);
		ancys.push_back(ancy);
		times.push_back(time);
	}
	read.close();

	//
	// localization
	//
	ofstream write("data_step3_localizationResults.txt");
	write.close();

	ofstream write3("data_step3_localizationDetails.txt");
	write3.close();

	double time_center_earliest = 12389843;
	double time_center_latest = 28379721;
	double time_halfWidth = 10;

	int ID = times.size()-1;
	while(ID>0)
	{
		double time = times[ID];
		//cout<<"ID "<<ID<<", time "<<time<<endl;

		bool isBuildGood = PowerRSSI(time,time_halfWidth, ID);

		ID --;
	}

	//
	// histogram
	//
	int bin = 50;
	double min = 0;
	double max = 50;
	Histogram *hDist = new Histogram("dist", bin, min, max);
	for(int i=0;i<error_dists.size();i++)
	{
		hDist->Fill(error_dists[i], 1);
	}

	ofstream write2("data_step3_localizationErrors.txt");
	for(int i=0;i<bin;i++)
	{
		int ID = i;
		double content = hDist->GetBinContent(ID);
		double binCenter = hDist->GetBinCenter(ID);
		write2<<ID<<" "<<binCenter<<" "<<content<<endl;
	}
	write2.close();

	return 1;
}

bool PowerRSSI_residualTest()
{
	int NumberNodes = 2;
	int observationsSize = 4+2*NumberNodes;
	int varialbeSize = 2*NumberNodes;
	int residualSize = 1;

	vector<double> obs;
	obs.push_back(-38.4748);
	obs.push_back(Power_ref);
	obs.push_back(-44.8991);
	obs.push_back(Power_ref);
	obs.push_back(PathLossExponent);
	obs.push_back(d0);
	obs.push_back(-45.3566);
	obs.push_back(84.0258);

	vector<double> variables;
	variables.push_back(-93.8696);
	variables.push_back(27.0384);
	variables.push_back(46.2762);
	variables.push_back(-43.2151);

	vector<double> residual;

	PolyResidualBlockFunction *poly = new PolyResidualBlockFunction("poly",obs,observationsSize,varialbeSize,residualSize);
	bool isPolyGood = poly->ResidualFunction(variables,residual);

	cout<<"Residual "<<residual[0]<<endl;

	return true;
}

bool PowerRSSI(double time_center, double time_halfWidth, int &ID_)
{
	//
	// Optimization 
	//
	int observationsSize = 6;
	int residualSize = 1;
	int varialbeSize = 2;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	// set variables
	vector<double> variables;
	for(int i=0;i<varialbeSize;i++)
	{
		variables.push_back(0.);
	}
	manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	// get observations
	vector<double> IDUsed;
	int ResidualBlockSize = 0;
	for(int i=ID_;i>=0;i--)
	{
		int ID = i;

		// get time
		double time = times[ID];

		if(time>=time_center-time_halfWidth&&time<time_center+time_halfWidth)
		{
			vector<double> observation_current;
			observation_current.push_back(rssis[ID]);
			observation_current.push_back(Power_ref);
			observation_current.push_back(PathLossExponent);
			observation_current.push_back(d0);
			observation_current.push_back(ancxs[ID]);
			observation_current.push_back(ancys[ID]);
			costFunction->AddResidualBlock(observation_current);

			ResidualBlockSize ++;

			IDUsed.push_back(ID);

			ID_ = ID;

			// debug
			//cout<<"ID "<<ID<<": rssis[ID] "<<rssis[ID]<<", Power_ref "<<Power_ref<<", PathLossExponent "<<PathLossExponent<<", d0 "<<d0<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<", tarxs[ID] "<<tarxs[ID]<<", tarys[ID] "<<tarys[ID]<<endl;
		}

		else if(time>=time_center+time_halfWidth)
			break;
	}

	//
	//cout<<"alice SetUserInitialization"<<endl;
	manager->SetUserInitialization(costFunction);

	//
	double UserReferencedLength = 70.;
	manager->SetUserReferencedLength(UserReferencedLength);

	// 
	double UserReferencedEpsilon = 1e-7;
	manager->SetUserEpsilonForTerminating(UserReferencedEpsilon);

	// initialize
	//cout<<"Initialize "<<endl;
	manager->Initialize();

	// get results
	vector<double> tarLocation;
	manager->GetVariables(tarLocation);

	/*
	//debug
	if(ResidualBlockSize>0)
		cout<<"Results target location: x "<<tarLocation[0]<<", y "<<tarLocation[1]<<endl;
	else if(ResidualBlockSize==0)
		cout<<"Results target location : Nothing"<<endl;
	*/


	//
	// Analysis
	//
	double tarx_total = 0;
	double tary_total = 0;
	// mean, i.e. the expectation of location, computed with ground truth
	for(int i=0;i<IDUsed.size();i++)
	{
		tarx_total += tarxs[IDUsed[i]];
		tary_total += tarys[IDUsed[i]];
	}

	double tarx_mean = tarx_total/double(IDUsed.size());
	double tary_mean = tary_total/double(IDUsed.size());

	// stansard deviation
	double tarx_stdDev = 0;
	double tary_stdDev = 0;
	for(int i=0;i<IDUsed.size();i++)
	{
		tarx_stdDev = tarx_stdDev + (tarxs[IDUsed[i]]-tarx_mean)*(tarxs[IDUsed[i]]-tarx_mean);
		tary_stdDev = tary_stdDev + (tarys[IDUsed[i]]-tary_mean)*(tarys[IDUsed[i]]-tary_mean);
	}
	tarx_stdDev = sqrt(tarx_stdDev/double(IDUsed.size()));
	tary_stdDev = sqrt(tary_stdDev/double(IDUsed.size()));

	// debug
	//cout<<"Ground Truth : tarx_mean "<<tarx_mean<<", StdDev "<<tarx_stdDev<<", tary_mean "<<tary_mean<<", StdDev "<<tary_stdDev<<endl;

	// error 
	double error_x = tarLocation[0] - tarx_mean;
	double error_y = tarLocation[1] - tary_mean;

	double error_dist = sqrt(error_x*error_x + error_y*error_y);
	error_dists.push_back(error_dist);

	// debug
	//cout<<"Error : error_x "<<error_x<<", StdDev "<<tarx_stdDev<<", error_y "<<error_y<<", StdDev "<<tary_stdDev<<endl;
	if(error_dist<1)
	{
		cout<<"Error < 1 : error_x "<<error_x<<", StdDev "<<tarx_stdDev<<", error_y "<<error_y<<", StdDev "<<tary_stdDev<<endl;
		for(int i=0;i<IDUsed.size();i++)
		{
			int ID = i;
			cout<<"ID : "<<ID<<", anchor loc "<<ancxs[ID]<<", "<<ancys[ID]<<endl;
		}
		cout<<endl;
	}

	// write
	ofstream write("data_step3_localizationResults.txt",ios::app);
	write<<tarLocation[0]<<" "<<tarLocation[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_x<<" "<<tarx_stdDev<<" "<<error_y<<" "<<tary_stdDev<<endl;
	write.close();


	// write
	ofstream write3("data_step3_localizationDetails.txt",ios::app);
	for(int i=0;i<IDUsed.size();i++)
	{
		int ID = i;
		write3<<SampleID_<<" "<<tarLocation[0]<<" "<<tarLocation[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_dist<<" "<<ancxs[ID]<<", "<<ancys[ID]<<endl;
	}
	write3.close();

	//
	SampleID_ ++;

	return true;
}
