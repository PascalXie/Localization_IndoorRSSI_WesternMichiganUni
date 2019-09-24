#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "OptimizationLeastMeanSquares.hh"

#include "Histogram.hh"
#include "Histogram2D.hh"

using namespace std;

int SampleID_ = 0;

// step 0 : Data_PathLossExponent
double Power_ref;
double PathLossExponent;
double d0;

// step 0 : read observations
vector<double> rssis, dists;
vector<double> tarxs, tarys;
vector<double> ancxs, ancys;
vector<double> times;

vector<double> error_dists;

// localization
bool PowerRSSI(double time_center, double time_halfWidth, int &ID_);

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

	return 1;
}

bool PowerRSSI(double time_center, double time_halfWidth, int &ID_)
{
	// Optimization : trilateration
	OptimizationLeastMeanSquares * lms= new OptimizationLeastMeanSquares("LMS");

	// get observations for trilateration
	vector<double> used_ancxs, used_ancys;
	vector<double> IDUsed;
	int ResidualBlockSize = 0;
	for(int i=ID_;i>=0;i--)
	{
		int ID = i;

		// get time
		double time = times[ID];

		if(time>=time_center-time_halfWidth&&time<time_center+time_halfWidth)
		{
			// compute distance
			double part1 = Power_ref-rssis[ID];
			double part2 = 10.*PathLossExponent;
			double exponent = part1/part2;
			double d = d0 * pow(10.,exponent);

			// observation for trilateration
			vector<double> anchor;
			anchor.push_back(ancxs[ID]);
			anchor.push_back(ancys[ID]);

			lms->AddObservation(anchor, d);


			// used
			bool isANewAnchor = true;
			for(int j=0;j<used_ancxs.size();j++)
			{
				if(used_ancxs[j]==ancxs[ID]&&used_ancys[j]==ancys[ID])
					isANewAnchor = false;
			}
			if(isANewAnchor == true)
			{
				used_ancxs.push_back(ancxs[ID]);
				used_ancys.push_back(ancys[ID]);
			}

			// settings
			ResidualBlockSize ++;

			IDUsed.push_back(ID);
			ID_ = ID;

			// debug
			//cout<<"ID "<<ID<<": rssis[ID] "<<rssis[ID]<<", Power_ref "<<Power_ref<<", PathLossExponent "<<PathLossExponent<<", d0 "<<d0<<", ancxs[ID] "<<ancxs[ID]<<", ancys[ID] "<<ancys[ID]<<", tarxs[ID] "<<tarxs[ID]<<", tarys[ID] "<<tarys[ID]<<endl;
		}

		else if(time>=time_center+time_halfWidth)
			break;
	}

	//debug
	for(int i=0;i<used_ancxs.size();i++)
	{
		cout<<"Anchor Used : ID "<<i<<", used_ancxs "<<used_ancxs[i]<<", used_ancys "<<used_ancys[i]<<endl;
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

	cout<<"Result Node: "<<tarLocation_tri[0]<<", "<<tarLocation_tri[1]<<endl;

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

	// error 
	double error_x = tarLocation_tri[0] - tarx_mean;
	double error_y = tarLocation_tri[1] - tary_mean;

	double error_dist = sqrt(error_x*error_x + error_y*error_y);
	error_dists.push_back(error_dist);

	// write
	ofstream write("data_step3_localizationResults.txt",ios::app);
	write<<tarLocation_tri[0]<<" "<<tarLocation_tri[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_x<<" "<<tarx_stdDev<<" "<<error_y<<" "<<tary_stdDev<<endl;
	write.close();

	// write
	ofstream write3("data_step3_localizationDetails.txt",ios::app);
	for(int i=0;i<IDUsed.size();i++)
	{
		int ID = IDUsed[i];
		write3<<SampleID_<<" "<<tarLocation_tri[0]<<" "<<tarLocation_tri[1]<<" "<<tarx_mean<<" "<<tary_mean<<" "<<error_dist<<" "<<ancxs[ID]<<" "<<ancys[ID]<<endl;
	}
	write3.close();

	//
	SampleID_ ++;
	return 1;
}
