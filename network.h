#include <fstream>
#include <iostream> 
#include <string>
#include <iterator>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iomanip>
#include "params.h"

#define LAMBDA 1

class Network
{

	using t_train_data = std::vector<std::pair<std::vector<double>, std::vector<double>>>;
	std::vector<std::vector<std::vector<double>>> mv_network;
    t_train_data mv_train_data;
	double m_learning_rate;
	double m_error_rate;

public:
	Network(std::string & r_name);
	Network(std::vector<int> & rv_layers);
	void save(std::string & r_name);
	void loadTrainData(std::string & r_file_name);
	void dump();
	double sigmoid(double x);
	void train();
	void setParams(Params & r_params);
	std::vector<double> predict(std::vector<double> & rv_input);
	void predictFile(std::string & r_file_name);
};
