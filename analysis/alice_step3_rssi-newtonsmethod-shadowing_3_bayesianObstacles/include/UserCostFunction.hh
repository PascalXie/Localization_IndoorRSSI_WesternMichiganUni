#ifndef USERCOSTFUNCTION_HH
#define USERCOSTFUNCTION_HH

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "UserResidualBlockFunction.hh"

using namespace std;

class UserCostFunction
{
    public:
		UserCostFunction(string name, int SizeObservations, int SizeVariables, int SizeResiduals);
		~UserCostFunction();

    public:
		// pure virtual
		virtual void AddResidualBlock(vector<double> observations) = 0;
		virtual bool CostFunction(vector<double> variables, vector<double> &CostFunctionValues)=0;
		virtual bool GetOneDerivative(int VarialbleID, vector<double> variables, double &theDerivativeValue) =0;
		virtual bool GetOneSecondOrderDerivative(int FirstPartialDerivativeVarialbleID, int SecondPartialDerivativeVarialbleID, vector<double> variables, double &theDerivativeValue) =0;

		virtual void SetStepLength(double delta) = 0;

		// no signal anchors for bayesian 
		virtual void SetBayesianAnchors(vector<double> ancxs, vector<double> ancys, double sigma, double weight) = 0;
		// obstacles for bayesian , rooms, walls
		virtual void SetBayesianObstacles(vector<double> obsxs, vector<double> obsys, double sigma, double weight) = 0;

    public:
		bool GradientFunction(vector<double> variables, vector<double> &theDerivatives);
		bool HessianMatrixFunction(vector<double> variables, vector<double> &HessianMatrix);

    public:
		virtual void Show() = 0;

	protected:
		vector<UserResidualBlockFunction*> ResidualBlockFunctions_;
		int SizeObservations_;
		int SizeVariables_;
		int SizeResiduals_;

		//// for derivative calculation
		//double delta_;

    private:
		string name_;
};

#endif
