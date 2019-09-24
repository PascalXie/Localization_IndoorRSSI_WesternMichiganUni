#include "OptimizationLeastMeanSquares.hh"

//----------------------------
// Constructor
//----------------------------
OptimizationLeastMeanSquares::OptimizationLeastMeanSquares(string name)
:	name_(name),
	is2D_(false)
{
}

//----------------------------
// Deconstructor
//----------------------------
OptimizationLeastMeanSquares::~OptimizationLeastMeanSquares() {}

//----------------------------
// Public
//----------------------------
void OptimizationLeastMeanSquares::AddObservation(vector<double> anchor, double distance)
{
	// debug
	if(anchor.size()!=2&&anchor.size()!=3)
	{
		cout<<"An error happend in OptimizationLeastMeanSquares::AddObservation; Node size wrong"<<endl;
	}

	if(anchor.size()==2)
	{
		anchor.push_back(0);
		is2D_=true;
	}

	// adding
	x_.push_back(anchor[0]);
	y_.push_back(anchor[1]);
	z_.push_back(anchor[2]);

	d_.push_back(distance);

	//cout<<"OptimizationLeastMeanSquares::AddObservation , position "<<anchor[0]<<" "<<anchor[1]<<" "<<anchor[2]<<", distance "<<distance<<endl;
}

void OptimizationLeastMeanSquares::Initiate()
{
	// step 1 : filling matrix
	vector<double> A_col1;
	vector<double> A_col2;
	vector<double> A_col3;
	vector<double> b_col1;

	for(int i=0;i<x_.size();i++)
	for(int j=i+1;j<x_.size();j++)
	{
		int anchor1ID = i;
		int anchor2ID = j;

		double ele1 = 2.*(x_[anchor1ID]-x_[anchor2ID]);
		double ele2 = 2.*(y_[anchor1ID]-y_[anchor2ID]);
		double ele3 = 2.*(z_[anchor1ID]-z_[anchor2ID]);
		double eleb = pow(x_[anchor1ID],2.) - pow(x_[anchor2ID],2.) + pow(y_[anchor1ID],2.) - pow(y_[anchor2ID],2.) +pow(z_[anchor1ID],2.) - pow(z_[anchor2ID],2.) - pow(d_[anchor1ID],2.) + pow(d_[anchor2ID],2.);

		A_col1.push_back(ele1);
		A_col2.push_back(ele2);
		A_col3.push_back(ele3);
		b_col1.push_back(eleb);

		/*
		// debug
		cout<<"OptimizationLeastMeanSquares::Initiate "<<"i "<<i<<", j "<<j<<endl;
		cout<<"ele1 "<<ele1<<"; x1 "<<x_[anchor1ID]<<", x2 "<<x_[anchor2ID]<<endl;
		*/
	}

	/*
	//debug
	cout<<"OptimizationLeastMeanSquares::Initiate "<<endl;
	for(int i=0;i<A_col1.size();i++)
	{
		cout<<"RowID "<<i<<", A_ : "<<A_col1[i]<<", "<<A_col2[i]<<", "<<A_col3[i]<<"; b_ "<<b_col1[i]<<endl;
	}
	*/

	int rowNum = A_col1.size();
	int colNum = 3;
	if(is2D_==true)
	{
		colNum = 2;
	}

	MatrixXd A_(rowNum,colNum);
	MatrixXd b_(rowNum,1);

	for(int i=0;i<rowNum;i++)
	{
		int rowID = i;
		A_(rowID,0) = A_col1[rowID];
		A_(rowID,1) = A_col2[rowID];

		// 3D space
		if(is2D_==false)
		{
			A_(rowID,2) = A_col3[rowID];
		}

		b_(rowID,0) = b_col1[rowID];
	}

	// location estimation
	MatrixXd A_T = A_.transpose();
	MatrixXd ATA_I = (A_T*A_).inverse();
	MatrixXd X = ATA_I*A_T*b_;
	Result_X_ = X;

	/*
	// debug
	cout<<"A \n"<<A_<<endl;
	cout<<"b \n"<<b_<<endl;
	cout<<"X \n"<<X.transpose()<<endl;
	*/
}

void OptimizationLeastMeanSquares::GetOptimizationResult(vector<double> &targetNode)
{
	targetNode.clear();

	targetNode.push_back(Result_X_(0,0));
	targetNode.push_back(Result_X_(1,0));

	if(is2D_==false)
	{
		targetNode.push_back(Result_X_(2,0));
	}
}
