#ifndef TOOLSIGNALPOWERGENERATOR_HH
#define TOOLSIGNALPOWERGENERATOR_HH

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath> 

#include "Units.hh"

#include "ToolMapGenerator.hh"

using namespace std;

class ToolSignalPowerGenerator
{
    public:
		ToolSignalPowerGenerator(string name);
		~ToolSignalPowerGenerator();

    public:
		//  ToolMapGenerator
		void SetToolMapGenerator(ToolMapGenerator *map);

		// compute with attenuation
		double GetSignalPower(double S_node, vector<double> A_anchor, vector<double> A_node);
		double GetLogSignalPower(double S_node, vector<double> A_anchor, vector<double> A_node);

		double GetSignalPowerStrength(double S_node, vector<double> A_anchor, vector<double> A_node);
		double GetSignalPowerStrength_Referenced(double S_node);


		// compute with path loss exponent
		double GetSignalPowerStrength_PathLossExponent(vector<double> A_anchor, vector<double> A_node);
		void SetReferencedSignalPowerStrength(vector<double> ReferencedSignalStrength);

		// parameters
		void SetFactor(double C);

    private:
		string name_;

		//  ToolMapGenerator
		ToolMapGenerator * map_;
		bool isMapGood_;

		// Integral
		int binSize_;

		// Factor
		double E_;
		double C_; // geometrical factor

		// parameters for referenced power
		double Distance_Referenced_;
		double SignalPowerStrength_Referenced_;

		// compute with path loss exponent
		vector<double> ReferencedSignalStrength_;
};
#endif
