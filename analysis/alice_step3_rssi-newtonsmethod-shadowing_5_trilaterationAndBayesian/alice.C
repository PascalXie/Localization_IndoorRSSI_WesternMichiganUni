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

#include "OptimizationLeastMeanSquares.hh"

#include "Histogram.hh"
#include "Histogram2D.hh"


using namespace std;

int SampleID_ = 0;

int Tool_SampleID_ = 0;

vector<double> rssis, dists;
vector<double> tarxs, tarys;
vector<double> ancxs, ancys;
vector<double> times;

vector<double> error_dists;

double Power_ref;
double PathLossExponent;
double d0;

// bayesian anchors
vector<double> unoverlapped_ancxs, unoverlapped_ancys;

// bayesian obstacles
vector<double> obstacle_obsxs, obstacle_obsys;


bool PowerRSSI_residualTest();
bool PowerRSSI(double time_center, double time_halfWidth, int &ID_);

bool Tool_DistributionCostFunction(double time_center, double time_halfWidth, int &ID_);

// bayesian anchors
void ComputeUnoverlappedAnchors();
int WhichAnchorInTheList(double x, double y);

// bayesian obstacles
bool ReadObstacleData();

int main()
{
	cout<<"Hello "<<endl;

	// step 0
	ReadObstacleData();

	/*
	// debug
	for(int i=0;i<obstacle_obsxs.size();i++)
	{
		cout<<"Obstacle ID "<<i<<", obstacle_obsxs "<<obstacle_obsxs[i]<<", obstacle_obsys "<<obstacle_obsys[i]<<endl;
	}
	*/

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

		if(rssi<-110) continue;

		rssis.push_back(rssi);
		dists.push_back(dist);
		tarxs.push_back(tarx);
		tarys.push_back(tary);
		ancxs.push_back(ancx);
		ancys.push_back(ancy);
		times.push_back(time);
	}
	read.close();

	//ComputeUnoverlappedAnchors 
	ComputeUnoverlappedAnchors();

	/*
	// debug
	int ListID = 10;
	int IDInUnOverlappedList = WhichAnchorInTheList(ancxs[ListID],ancys[ListID]);
	cout<<"IDInUnOverlappedList "<<IDInUnOverlappedList<<", x "<<ancxs[ListID]<<", y "<<ancys[ListID]<<endl;
	return 1;
	*/


	//
	// localization
	//
	ofstream write("data_step3_localizationResults.txt");
	write.close();

	ofstream write3("data_step3_localizationDetails.txt");
	write3.close();

	double time_halfWidth = 40;

	int ID = times.size()-1;
	while(ID>0)
	{
		double time = times[ID];
		cout<<"ID "<<ID<<", time "<<time<<", ancxs "<<ancxs[ID]<<", ancys "<<ancys[ID]<<endl;

		ID += 40;
		if(ID>=times.size()-1) ID = times.size()-1;
		bool isBuildGood = PowerRSSI(time,time_halfWidth, ID);

		ID --;

	}

	//
	// histogram
	//
	int bin = 12;
	double min = 0;
	double max = 30;
	Histogram *hDist = new Histogram("dist", bin, min, max);
	for(int i=0;i<error_dists.size();i++)
	{
		hDist->Fill(error_dists[i], 1);
	}

	hDist->Normalize();

	ofstream write2("data_step3_localizationErrors.txt");
	for(int i=0;i<bin;i++)
	{
		int ID = i;
		double content = hDist->GetBinContent(ID);
		double binCenter = hDist->GetBinCenter(ID);
		write2<<ID<<" "<<binCenter<<" "<<content<<endl;
	}
	write2.close();

	/*
	//
	// Tool  : distribution of cost function
	//
	double tool_time_halfWidth = 40;

	int tool_ID = times.size()-1;
	while(tool_ID>0)
	{
		double time = times[tool_ID];
		cout<<"tool_ID "<<tool_ID<<", time "<<time<<", ancxs "<<ancxs[tool_ID]<<", ancys "<<ancys[tool_ID]<<endl;

		tool_ID += 40;
		if(tool_ID>=times.size()-1) tool_ID = times.size()-1;
		bool isBuildGood = Tool_DistributionCostFunction(time,tool_time_halfWidth, tool_ID);

		tool_ID --;

	}
	*/

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
	// Optimization : trilateration
	OptimizationLeastMeanSquares * lms= new OptimizationLeastMeanSquares("LMS");

	//
	// Optimization : nonlinear optimization
	//
	int observationsSize = 6;
	int residualSize = 1;
	int varialbeSize = 2;

	UserOptimizationManager * manager = new NewtonsOptimizationManager("NewtonsMethod",observationsSize,varialbeSize,residualSize);

	//// set variables
	//vector<double> variables;
	//for(int i=0;i<varialbeSize;i++)
	//{
	//	variables.push_back(0.);
	//}
	//manager->SetUserInitialization(variables);

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	// bayesian anchors : initialize
	vector<bool> AnchorUsedIDs;
	for(int i=0;i<unoverlapped_ancxs.size();i++)
		AnchorUsedIDs.push_back(false);

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
			//
			// trilateration
			//
			// compute distance
			double part1 = Power_ref-rssis[ID];
			double part2 = 10.*PathLossExponent;
			double exponent = part1/part2;
			double d = d0 * pow(10.,exponent);

			// observation for trilateration
			vector<double> anchor_tri;
			anchor_tri.push_back(ancxs[ID]);
			anchor_tri.push_back(ancys[ID]);
			lms->AddObservation(anchor_tri, d);


			//
			// nonlinear optimization
			//
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

			// bayesian anchors
			int IDInUnOverlappedList = WhichAnchorInTheList(ancxs[ID],ancys[ID]);
			AnchorUsedIDs[IDInUnOverlappedList] = true;

			// debug
			//cout<<"ID "<<ID<<": rssis[ID] "<<rssis[ID]<<", Power_ref "<<Power_ref<<", PathLossExponent "<<PathLossExponent<<", d0 "<<d0<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<", tarxs[ID] "<<tarxs[ID]<<", tarys[ID] "<<tarys[ID]<<endl;
		}

		else if(time>=time_center+time_halfWidth)
			break;
	}

	//
	// trilateration
	//
	vector<double> used_ancxs, used_ancys;
	for(int i=0;i<AnchorUsedIDs.size();i++)
	{
		int ID = i;
		if(AnchorUsedIDs[i]==1) 
		{
			used_ancxs.push_back(unoverlapped_ancxs[ID]);
			used_ancys.push_back(unoverlapped_ancys[ID]);
		}
	}

	// compute tarLocation_tri
	vector<double> tarLocation_tri;
	tarLocation_tri.push_back(-100);
	tarLocation_tri.push_back(-100);

	if(used_ancxs.size()>=3)
	{
		lms->Initiate();
		lms->GetOptimizationResult(tarLocation_tri);
	}

	else if(used_ancxs.size()==2)
	{
		tarLocation_tri[0] = (used_ancxs[0]+used_ancxs[1])/2.;
		tarLocation_tri[1] = (used_ancys[0]+used_ancys[1])/2.;
	}
	else if(used_ancxs.size()==1)
	{
		tarLocation_tri[0] = used_ancxs[0];
		tarLocation_tri[1] = used_ancys[0];
	}
	else if(used_ancxs.size()<1)
	{
		tarLocation_tri[0] = -100;
		tarLocation_tri[1] = -100;
	}

	// check tarLocation_tri[0] == nan
	if(isnan(tarLocation_tri[0])||isnan(tarLocation_tri[1])) 
	{
		tarLocation_tri[0] = 0;
		tarLocation_tri[1] = 0;
		for(int i=0;i<used_ancxs.size();i++)
		{
			tarLocation_tri[0] += used_ancxs[i];
			tarLocation_tri[1] += used_ancys[i];
		}

		if(used_ancxs.size()==0)
		{
			tarLocation_tri[0] = 0.;
			tarLocation_tri[1] = 0.;
		}
		else
		{
			tarLocation_tri[0] = tarLocation_tri[0]/used_ancxs.size();
			tarLocation_tri[1] = tarLocation_tri[1]/used_ancys.size();
		}
	}

	// set variables
	vector<double> variables;
	for(int i=0;i<varialbeSize;i++)
	{
		variables.push_back(0.);
	}

	variables[0] = tarLocation_tri[0] + 0.5;
	variables[1] = tarLocation_tri[1] + 0.5;

	manager->SetUserInitialization(variables);

	//
	// nonlinear 
	//
	// bayesian anchors
	vector<double> nosignal_ancxs, nosignal_ancys;
	for(int i=0;i<AnchorUsedIDs.size();i++)
	{
		int ID = i;
		if(AnchorUsedIDs[i]==0)
		{
			nosignal_ancxs.push_back(unoverlapped_ancxs[ID]);
			nosignal_ancys.push_back(unoverlapped_ancys[ID]);
		}

		cout<<"ID "<<ID<<", AnchorUsedIDs "<<AnchorUsedIDs[i]<<", unoverlapped_ancxs "<<unoverlapped_ancxs[ID]<<", unoverlapped_ancys "<<unoverlapped_ancys[ID]<<endl;
	}

	double nosignal_sigma = 10;
	double nosignal_weight = 500;

	costFunction->SetBayesianAnchors(nosignal_ancxs, nosignal_ancys, nosignal_sigma, nosignal_weight);

	// bayesian obstacles
	double obstacle_sigma = 1.5;
	double obstacle_weight = 500;
	costFunction->SetBayesianObstacles(obstacle_obsxs, obstacle_obsys, obstacle_sigma, obstacle_weight);

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

	// stansard deviation : ground truth
	double tarx_stdDev = 0;
	double tary_stdDev = 0;
	for(int i=0;i<IDUsed.size();i++)
	{
		tarx_stdDev = tarx_stdDev + (tarxs[IDUsed[i]]-tarx_mean)*(tarxs[IDUsed[i]]-tarx_mean);
		tary_stdDev = tary_stdDev + (tarys[IDUsed[i]]-tary_mean)*(tarys[IDUsed[i]]-tary_mean);
	}
	tarx_stdDev = sqrt(tarx_stdDev/double(IDUsed.size()));
	tary_stdDev = sqrt(tary_stdDev/double(IDUsed.size()));

	double loc_gt_stdDev = sqrt(tarx_stdDev*tarx_stdDev + tary_stdDev*tary_stdDev);

	// debug
	//cout<<"Ground Truth : tarx_mean "<<tarx_mean<<", StdDev "<<tarx_stdDev<<", tary_mean "<<tary_mean<<", StdDev "<<tary_stdDev<<endl;

	// error 
	double error_x = tarLocation[0] - tarx_mean;
	double error_y = tarLocation[1] - tary_mean;

	double error_dist = sqrt(error_x*error_x + error_y*error_y);
	error_dists.push_back(error_dist);

	/*
	// debug
	cout<<"Error : error_x "<<error_x<<", StdDev "<<tarx_stdDev<<", error_y "<<error_y<<", StdDev "<<tary_stdDev<<endl;
	if(error_dist<10)
	{
		cout<<"Error < 1 : error_x "<<error_x<<", StdDev "<<tarx_stdDev<<", error_y "<<error_y<<", StdDev "<<tary_stdDev<<endl;
		for(int i=0;i<IDUsed.size();i++)
		{
			int ID = i;
			cout<<"ID : "<<ID<<", anchor loc "<<ancxs[ID]<<", "<<ancys[ID]<<endl;
		}
		cout<<endl;
	}
	*/

	// write
	ofstream write("data_step3_localizationResults.txt",ios::app);
	write<<tarLocation[0]<<" "<<tarLocation[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_x<<" "<<tarx_stdDev<<" "<<error_y<<" "<<tary_stdDev<<endl;
	write.close();


	// write
	ofstream write3("data_step3_localizationDetails.txt",ios::app);
	for(int i=0;i<IDUsed.size();i++)
	{
		int ID = IDUsed[i];
		write3<<SampleID_<<" "<<tarLocation[0]<<" "<<tarLocation[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_dist<<" "<<ancxs[ID]<<" "<<ancys[ID]<<endl;
	}
	write3.close();

	//
	SampleID_ ++;

	return true;
}

bool Tool_DistributionCostFunction(double time_center, double time_halfWidth, int &ID_)
{
	//
	// Optimization 
	//
	int observationsSize = 6;
	int residualSize = 1;
	int varialbeSize = 2;

	// set cost function
	UserCostFunction* costFunction = new NewtonsCostFunction("costFunction",observationsSize,varialbeSize,residualSize);

	// bayesian anchors : initialize
	vector<bool> AnchorUsedIDs;
	for(int i=0;i<unoverlapped_ancxs.size();i++)
		AnchorUsedIDs.push_back(false);

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

			// bayesian anchors
			int IDInUnOverlappedList = WhichAnchorInTheList(ancxs[ID],ancys[ID]);
			AnchorUsedIDs[IDInUnOverlappedList] = true;

			// debug
			cout<<"ID "<<ID<<": rssis[ID] "<<rssis[ID]<<", Power_ref "<<Power_ref<<", PathLossExponent "<<PathLossExponent<<", d0 "<<d0<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<", tarxs[ID] "<<tarxs[ID]<<", tarys[ID] "<<tarys[ID]<<endl;
		}

		else if(time>=time_center+time_halfWidth)
			break;
	}

	// bayesian anchors
	vector<double> nosignal_ancxs, nosignal_ancys;
	for(int i=0;i<AnchorUsedIDs.size();i++)
	{
		int ID = i;
		if(AnchorUsedIDs[i]==0)
		{
			nosignal_ancxs.push_back(unoverlapped_ancxs[ID]);
			nosignal_ancys.push_back(unoverlapped_ancys[ID]);
		}

		cout<<"ID "<<ID<<", AnchorUsedIDs "<<AnchorUsedIDs[i]<<", unoverlapped_ancxs "<<unoverlapped_ancxs[ID]<<", unoverlapped_ancys "<<unoverlapped_ancys[ID]<<endl;
	}

	double nosignal_sigma = 10;
	double nosignal_weight = 500;

	costFunction->SetBayesianAnchors(nosignal_ancxs, nosignal_ancys, nosignal_sigma, nosignal_weight);

	// bayesian obstacles
	double obstacle_sigma = 1.5;
	double obstacle_weight = 500;
	costFunction->SetBayesianObstacles(obstacle_obsxs, obstacle_obsys, obstacle_sigma, obstacle_weight);



	//
	// histogram of cost function
	//
	string filename = "data_step3_tool_costFunction_" + to_string(Tool_SampleID_) + ".txt";
	ofstream write(filename);

	int binSizes[2] = {150, 150};
	double  mins[2] = {0., 0.};
	double  maxs[2] = {70., 60.};
	double binWidths[2];

	binWidths[0] = (maxs[0]-mins[0])/double(binSizes[0]);
	binWidths[1] = (maxs[1]-mins[1])/double(binSizes[1]);

	for(int i=0;i<binSizes[0];i++)
	for(int j=0;j<binSizes[1];j++)
	{
		int xID = i;
		int yID = j;

		double x = mins[0] + binWidths[0]*(double(xID)+0.5);
		double y = mins[1] + binWidths[1]*(double(yID)+0.5);

		// debug
		//cout<<"ID_ "<<ID_<<", x "<<x<<", y "<<y<<endl;

		vector<double> variables, costFunctionValue;
		variables.push_back(x);
		variables.push_back(y);

		costFunction->CostFunction(variables, costFunctionValue);

		write<<x<<" "<<y<<" "<<costFunctionValue[0]<<endl;

	}

	write.close();

	Tool_SampleID_ ++;

	return true;
}

void ComputeUnoverlappedAnchors()
{
	for(int i=0;i<ancxs.size();i++)
	{
		int ID = i;

		//debug
		//cout<<"ID "<<ID<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<endl;

		bool isAnchorNew = true;
		for(int j=0;j<unoverlapped_ancxs.size();j++)
		{
			if(ancxs[ID]==unoverlapped_ancxs[j]&&ancys[ID]==unoverlapped_ancys[j])
				isAnchorNew = false;
		}

		if(isAnchorNew==true)
		{
			unoverlapped_ancxs.push_back(ancxs[ID]);
			unoverlapped_ancys.push_back(ancys[ID]);
		}
	}

	//
	for(int i=0;i<unoverlapped_ancxs.size();i++)
	{
		cout<<"Unoverlapped anchors, ID "<<i<<", x "<<unoverlapped_ancxs[i]<<", y "<<unoverlapped_ancys[i]<<endl;
	}

}

int WhichAnchorInTheList(double x, double y)
{
	for(int i=0;i<unoverlapped_ancxs.size();i++)
	{
		if(x==unoverlapped_ancxs[i]&&y==unoverlapped_ancys[i])
			return i;
	}

	return -1;
}

bool ReadObstacleData()
{
	string filename0 = "../../data_step0_obstacles.txt";
	ifstream file(filename0.c_str());

	if(file.fail())
	{
		cout<<"Can not find the file \" "<<filename0<<" \""<<endl;
		return 0;
	}

	string label;
	double x, y;
	while(!file.eof())
	{
		file>>label>>x>>y;

		if(file.eof()) break;

		obstacle_obsxs.push_back(x);
		obstacle_obsys.push_back(y);
	}

	file.close();

	return 1;
}
