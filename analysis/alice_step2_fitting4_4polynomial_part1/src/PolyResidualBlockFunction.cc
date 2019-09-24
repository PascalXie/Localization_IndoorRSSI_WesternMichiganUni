#include "PolyResidualBlockFunction.hh"

//-------------------------
// Constructor
//-------------------------
PolyResidualBlockFunction::PolyResidualBlockFunction(string name, vector<double> observations, int SizeObservations, int SizeVariables, int SizeResiduals)
:	UserResidualBlockFunction(name+"_User"),
	name_(name),
	SizeObservations_(SizeObservations),
	SizeVariables_(SizeVariables), SizeResiduals_(SizeResiduals)
{
	observations_.clear();
	observations_ = observations;

}

//-------------------------
// Destructor
//-------------------------
PolyResidualBlockFunction::~PolyResidualBlockFunction()
{}

//-------------------------
// Public
//-------------------------
bool PolyResidualBlockFunction::ResidualFunction(vector<double> variables, vector<double> &residuals)
{
	//
	// step 0
	//
	if(variables.size()!=SizeVariables_)
	{
		cout<<"An error happend in PolyResidualBlockFunction::ResidualFunction"<<endl;
		return false;
	}
	if(observations_.size()!=SizeObservations_)
	{
		cout<<"An error happend in PolyResidualBlockFunction::ResidualFunction"<<endl;
		return false;
	}

	// observations_[0] : P_ob, anchor observed power strength
	// observations_[1] : d, distance

	// varialbles[0] : a0, a0 for polynomial 
	// varialbles[1] : a1, 
	// varialbles[2] : a2, 
	// varialbles[3] : a3, 

	// residual = P_ob - (a0 + a1*d + a2*d*d + a3*d*d*d)

	double P_ob = observations_[0];
	double d    = observations_[1];

	double a0 = variables[0];
	double a1 = variables[1];
	double a2 = variables[2];
	double a3 = variables[3];

	double residual_0 = 0;
	residual_0 = P_ob - (a0 + a1*d + a2*d*d + a3*d*d*d);

	double weight = 1.;
	//if(d>1e-5) weight = 1./d;
	//else  weight = 1e5;

	residual_0 = weight*residual_0*residual_0;

	residuals.clear();
	residuals.push_back(residual_0);

	/*
	// debug
	cout<<"Debug class PolyResidualBlockFunction::ResidualFunction"<<endl;
	cout<<"Debug Residual : Observations, P_ob "<<P_ob<<", d "<<d<<endl;
	cout<<"Debug Residual : varialbles,   P_0  "<<P_0<<",gamma "<<gamma<<endl;
	cout<<"Debug Residual : residual_0 "<<residual_0<<endl;
	*/

	return true;
}

int PolyResidualBlockFunction::GetObervationsSize()
{
	return SizeObservations_;
}

void PolyResidualBlockFunction::ShowResidualFunction()
{
	// debug
	cout<<"debug Class PolyResidualBlockFunction : "<<name_<<endl;
	cout<<"Observations : "<<endl;
	for(int i=0; i<observations_.size(); i++)
	{
		cout<<"    ID "<<i<<"; Observation "<<observations_[i]<<endl;
	}
}
