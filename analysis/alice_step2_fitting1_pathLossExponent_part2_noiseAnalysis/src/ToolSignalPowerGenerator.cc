#include "ToolSignalPowerGenerator.hh"

//---------------------------
// Constructor
//---------------------------
ToolSignalPowerGenerator::ToolSignalPowerGenerator(string name)
:	name_(name),
	isMapGood_(false),
	binSize_(1),
	C_(1e-1),
	Distance_Referenced_(1.),
	SignalPowerStrength_Referenced_(0.)
{
	/*
	// debug
	cout<<"Name "<<name_<<endl;
	*/
	ReferencedSignalStrength_.clear();
}

//---------------------------
// Deconstructor
//---------------------------
ToolSignalPowerGenerator::~ToolSignalPowerGenerator()
{}

//---------------------------
// Public
//---------------------------
void ToolSignalPowerGenerator::SetToolMapGenerator(ToolMapGenerator *map)
{
	map_ = map;
	isMapGood_ = true;
}

//---------------------------
// Public : compute
//---------------------------
double ToolSignalPowerGenerator::GetSignalPower(double S_node, vector<double> A_anchor, vector<double> A_node)
{
	// S_node : Signal Power at the node
	// A_anchor : position of the anchor , i.e. Beacon
	// A_anchor : x, y, z; for 2D and 3D
	// A_node : position of the node
	// A_node : x, y, z; for 2D and 3D

	// for 2D : A_anchor[2] = A_node[2], i.e. A_anchor[2] = A_node[2] = 0

	//
	// step 0 : debug
	//
	if(A_anchor.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_anchor.size must be 3"<<endl;
		return 0;
	}

	if(A_node.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_node.size must be 3"<<endl;
		return 0;
	}

	//
	// step 1 : compute binWidth
	//
	vector<double> binWidth;
	for(int i=0;i<3;i++)
	{
		double width = (A_anchor[i]-A_node[i])/double(binSize_);
		binWidth.push_back(width);

		/*
		// debug
		cout<<"bin width ID "<<i<<", witdh "<<width<<endl;
		*/
	}


	//
	// step 2 : integral
	//
	// integral = \int -2*constant*delta
	double integral = 0;

	double delta = sqrt(binWidth[0]*binWidth[0]+binWidth[1]*binWidth[1]+binWidth[2]*binWidth[2]);

	for(int i=0;i<binSize_;i++)
	{
		double x = A_node[0] + (double(i)+0.5)*binWidth[0];
		double y = A_node[1] + (double(i)+0.5)*binWidth[1];
		double z = A_node[2] + (double(i)+0.5)*binWidth[2];

		double constant = map_->GetConstant_Attenuation(x,y,z);

		integral = integral + (-2.*constant*delta);

		/*
		// debug
		cout<<"loop ID "<<i<<": x "<<x<<", y "<<y<<", z "<<z<<"; constant "<<constant<<endl;
		*/
	}


	//
	// step 3 : compute the Factor
	//
	// 1/(4.*PI*distance^2)
	double distance2 =      (A_anchor[0]-A_node[0])*(A_anchor[0]-A_node[0]);
	distance2 = distance2 + (A_anchor[1]-A_node[1])*(A_anchor[1]-A_node[1]);
	distance2 = distance2 + (A_anchor[2]-A_node[2])*(A_anchor[2]-A_node[2]);

	if(distance2<=C_/(2.*PI)) {
		E_ = 0.5;
	}
	else {
		E_ = C_/(4.*PI*distance2);
	}

	//
	// step 4 : compute power at the anchor
	//
	//double Power_anchor = 
	double S_anchor = S_node * E_ * exp(integral);

	/*
	// debug
	cout<<"------------------------------------------------"<<endl;
	cout<<"ToolSignalPowerGenerator::GetSignalPower"<<endl;
	cout<<"delta "<<delta<<endl;
	cout<<"Factor "<<E_<<endl;
	cout<<"integral "<<integral<<endl;
	cout<<"Power Node : "<<S_node<<endl;
	cout<<"Power Anchor : "<<S_anchor<<endl;
	cout<<"------------------------------------------------"<<endl;
	*/

	return S_anchor;
}

double ToolSignalPowerGenerator::GetLogSignalPower(double S_node, vector<double> A_anchor, vector<double> A_node)
{
	// S_node : Signal Power at the node
	// A_anchor : position of the anchor , i.e. Beacon
	// A_anchor : x, y, z; for 2D and 3D
	// A_node : position of the node
	// A_node : x, y, z; for 2D and 3D

	// for 2D : A_anchor[2] = A_node[2], i.e. A_anchor[2] = A_node[2] = 0

	//
	// step 0 : debug
	//
	if(A_anchor.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_anchor.size must be 3"<<endl;
		return 0;
	}

	if(A_node.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_node.size must be 3"<<endl;
		return 0;
	}

	//
	// step 1 : compute binWidth
	//
	vector<double> binWidth;
	for(int i=0;i<3;i++)
	{
		double width = (A_anchor[i]-A_node[i])/double(binSize_);
		binWidth.push_back(width);

		/*
		// debug
		cout<<"bin width ID "<<i<<", witdh "<<width<<endl;
		*/
	}


	//
	// step 2 : integral
	//
	// integral = \int -2*constant*delta
	double integral = 0;

	double delta = sqrt(binWidth[0]*binWidth[0]+binWidth[1]*binWidth[1]+binWidth[2]*binWidth[2]);

	for(int i=0;i<binSize_;i++)
	{
		double x = A_node[0] + (double(i)+0.5)*binWidth[0];
		double y = A_node[1] + (double(i)+0.5)*binWidth[1];
		double z = A_node[2] + (double(i)+0.5)*binWidth[2];

		double constant = map_->GetConstant_Attenuation(x,y,z);

		integral = integral + (-2.*constant*delta);

		/*
		// debug
		cout<<"loop ID "<<i<<": x "<<x<<", y "<<y<<", z "<<z<<"; constant "<<constant<<endl;
		*/
	}


	//
	// step 3 : compute the Geometrical Factor
	//
	// 1/(4.*PI*distance^2)
	double distance2 =      (A_anchor[0]-A_node[0])*(A_anchor[0]-A_node[0]);
	distance2 = distance2 + (A_anchor[1]-A_node[1])*(A_anchor[1]-A_node[1]);
	distance2 = distance2 + (A_anchor[2]-A_node[2])*(A_anchor[2]-A_node[2]);

	if(distance2<=C_/(2.*PI)) {
		E_ = 0.5;
	}
	else {
		E_ = C_/(4.*PI*distance2);
	}

	//
	// step 4 : compute power at the anchor
	//
	//double Power_anchor = 
	double LnS_anchor = log(S_node) + log(E_) + integral;

	/*
	// debug
	cout<<"------------------------------------------------"<<endl;
	cout<<"ToolSignalPowerGenerator::GetSignalPower"<<endl;
	cout<<"delta "<<delta<<endl;
	cout<<"Factor "<<E_<<endl;
	cout<<"integral "<<integral<<endl;
	cout<<"Power Node : "<<S_node<<endl;
	cout<<"Ln Power Anchor : "<<LnS_anchor<<endl;
	cout<<"------------------------------------------------"<<endl;
	*/

	return LnS_anchor;
}

double ToolSignalPowerGenerator::GetSignalPowerStrength(double S_node, vector<double> A_anchor, vector<double> A_node)
{
	double SignalPowerStrength = 30. + 10.*GetLogSignalPower(S_node, A_anchor, A_node);

	/*
	// debug
	cout<<"ToolSignalPowerGenerator::GetSignalPowerStrength"<<endl;
	cout<<"SignalPowerStrength_Ref "<<SignalPowerStrength_Ref<<endl;
	cout<<"SignalPowerStrength "<<SignalPowerStrength<<endl;
	*/

	return SignalPowerStrength;
}

double ToolSignalPowerGenerator::GetSignalPowerStrength_Referenced(double S_node)
{
	vector<double> A_anchor, A_node;
	A_anchor.push_back(0.);
	A_anchor.push_back(0.);
	A_anchor.push_back(0.);

	A_node.push_back(A_anchor[0]+Distance_Referenced_);
	A_node.push_back(A_anchor[1]);
	A_node.push_back(A_anchor[2]);

	double SignalPowerStrength_Ref = GetSignalPowerStrength(S_node, A_anchor, A_node);

	return SignalPowerStrength_Ref;
}

void ToolSignalPowerGenerator::SetFactor(double C)
{
	C_ = C;
}


//---------------------------------------
// compute with path loss exponent
//---------------------------------------
double ToolSignalPowerGenerator::GetSignalPowerStrength_PathLossExponent(vector<double> A_anchor, vector<double> A_node)
{
	// A_anchor : position of the anchor , i.e. Beacon
	// A_anchor : x, y, z; for 2D and 3D
	// A_node : position of the node
	// A_node : x, y, z; for 2D and 3D

	// for 2D : A_anchor[2] = A_node[2], i.e. A_anchor[2] = A_node[2] = 0

	//
	// step 0 : debug
	//
	if(A_anchor.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_anchor.size must be 3"<<endl;
		return 0;
	}

	if(A_node.size()!=3)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, A_node.size must be 3"<<endl;
		return 0;
	}

	if(ReferencedSignalStrength_.size()==0)
	{
		cout<<"An error happend in ToolSignalPowerGenerator::GetSignalPower, ReferencedSignalStrength_ is empty"<<endl;
		return 0;
	}

	//
	// step 1 : compute binWidth
	//
	vector<double> binWidth;
	for(int i=0;i<3;i++)
	{
		double width = (A_anchor[i]-A_node[i])/double(binSize_);
		binWidth.push_back(width);

		/*
		// debug
		cout<<"bin width ID "<<i<<", witdh "<<width<<endl;
		*/
	}

	//
	// step 2 : integral
	//
	// integral = P_i,j^0 - (\int gamma)*log(delta/distance_ref)
	// path loss exponent : gamma

	double integral = ReferencedSignalStrength_[0];
	double delta = sqrt(binWidth[0]*binWidth[0]+binWidth[1]*binWidth[1]+binWidth[2]*binWidth[2]);
	double d_0 = 1;

	for(int i=0;i<binSize_;i++)
	{
		double x = A_node[0] + (double(i)+0.5)*binWidth[0];
		double y = A_node[1] + (double(i)+0.5)*binWidth[1];
		double z = A_node[2] + (double(i)+0.5)*binWidth[2];

		double PathLossExponent = map_->GetConstant_PathLossExponent(x,y,z);
		double distance = double(i+1) * delta;
		double ln10 = log(10.)/log(2.72);
		double part1 = 1./(distance/d_0*ln10)*(1./d_0);

		integral = integral - 10.*PathLossExponent*log(delta/d_0);

		// debug
		cout<<"loop ID "<<i<<": x "<<x<<", y "<<y<<", z "<<z<<"; PathLossExponent "<<PathLossExponent<<", ReferencedSignalStrength_[0] "<<ReferencedSignalStrength_[0]<<", integral "<<integral<<", delta "<<delta<<endl;
	}

	//
	// step 3 : compute power at the anchor
	//
	double S_anchor = integral;

	return S_anchor;
}

void ToolSignalPowerGenerator::SetReferencedSignalPowerStrength(vector<double> ReferencedSignalStrength)
{
	ReferencedSignalStrength_ = ReferencedSignalStrength;
}
