#ifndef OPTIMIZATIONLEASTMEANSQUARES_HH
#define OPTIMIZATIONLEASTMEANSQUARES_HH

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// Eigen
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class OptimizationLeastMeanSquares
{
    public:
		OptimizationLeastMeanSquares(string name);
		~OptimizationLeastMeanSquares();

    public:
		void AddObservation(vector<double> anchor, double distance);
		void Initiate();
		void GetOptimizationResult(vector<double> &targetNode);

    private:
		string name_;

		bool is2D_;

		vector<double> x_; // anchor location, x
		vector<double> y_; // anchor location, y
		vector<double> z_; // anchor location, z
		vector<double> d_; // distance


		// Least mean squares
		// A_*X = b
		MatrixXd Result_X_;
};
#endif
