#ifndef NEWTONSCOSTFUNCTION_HH
#define NEWTONSCOSTFUNCTION_HH

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "UserCostFunction.hh"

#include "UserResidualBlockFunction.hh"
#include "PolyResidualBlockFunction.hh"

#include "Units.hh"

using namespace std;

class NewtonsCostFunction : virtual public UserCostFunction
{
    public:
		NewtonsCostFunction(string name, int SizeObservations, int SizeVariables, int SizeResiduals);
		~NewtonsCostFunction();

    public:
		void SetStepLength(double delta);
		void Show();

    public:
		// no signal anchors for bayesian 
		virtual void SetBayesianAnchors(vector<double> ancxs, vector<double> ancys, double sigma, double weight);

    public:
		virtual void AddResidualBlock(vector<double> observations);
		virtual bool CostFunction(vector<double> variables, vector<double> &CostFunctionValues);
		virtual bool GetOneDerivative(int VarialbleID, vector<double> variables, double &theDerivativeValue);
		virtual bool GetOneSecondOrderDerivative(int FirstPartialDerivativeVarialbleID, int SecondPartialDerivativeVarialbleID, vector<double> variables, double &theDerivativeValue);

	protected:
		bool GetSecondOrderDerivative_Part1(int ResidualBlockID, int FirstPartialDerivativeVarialbleID, int SecondPartialDerivativeVarialbleID, vector<double> variables, double &theDerivativeValue);
		bool GetSecondOrderDerivative_Part2(int ResidualBlockID, int FirstPartialDerivativeVarialbleID, int SecondPartialDerivativeVarialbleID, vector<double> variables, double &theDerivativeValue);

	protected:
		// for derivative calculation
		double delta_;

		// no signal anchors for bayesian 
		vector<double>	nosignal_ancxs_;
		vector<double> 	nosignal_ancys_;
		double			nosignal_sigma_;
		double			nosignal_weight_;

    private:
		string name_;
};
#endif
