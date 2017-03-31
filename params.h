#include <fstream>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <limits>
#include <string>
#include "xvokra00.h"

#ifndef H_PARAMS
#define H_PARAMS
#define TRAIN 0
#define PREDICT 1
#define UNDEFINED -1

class Params
{
	double m_learning_rate = UNDEFINED;
	double m_error_rate  = UNDEFINED;;
	std::string m_network_file;
	std::string m_input_file;
	bool m_interactive;
	std::vector<int> mn_neurons;
	int m_mode = UNDEFINED;

	void loadLayers(int argc, char * argv[]);
public:

	void parse(int argc, char * argv[]);
	double getLearningRate();
	double getErrorRate();
	std::string & getNetworkFile();
	std::vector<int> & getLayers();
	std::string & getInputFile();
	void dump();
	int getMode();
	bool isInteractive();
};

#endif
