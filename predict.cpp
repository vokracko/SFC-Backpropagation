#include <iostream>
#include "params.h"
#include "network.h"
#include <vector>

void printHelp()
{

	std::cout << "Usage: ./predict [mode -n network_file -i input_file]\n"
	"\t mode\n"
	"\t\t -t\ttrain, require arguments e, m, l\n"
	"\t\t -p\tpredict\n"
	"\t-n network_file\n"
	"\t\t mode=train\tfile where trained network will be saved\n"
	"\t\t mode=predict\tfile where trained network is saved\n"
	"\t-i input_file\n"
	"\t\t mode=train\tfile with train data\n"
	"\t\t mode=predict\tfile with input data\n"
	"\t-e N\terror_rate, determine swhen network is precise enough\n"
	"\t-m N\tlearning_rate \n"
	"\t-l I N [N ...] O \tnumber of neurons in layer, I size of input, O size of output\n";
}

int main(int argc, char * argv[])
{
	Params params;

	try
	{
		params.parse(argc, argv);

		if(!params.isInteractive())
			params.dump();

		if(params.getMode() == TRAIN)
		{
			Network network(params.getLayers());
			network.setParams(params);
			network.loadTrainData(params.getInputFile());
			network.train();
			network.save(params.getNetworkFile());
		}
		else
		{
			Network network(params.getNetworkFile());
			network.predictFile(params.getInputFile());
		}
	} catch (std::invalid_argument &e)
	{
		std::cerr << e.what() << std::endl;
		printHelp();
		return 1;
	} catch (std::ios_base::failure &e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	} catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 3;
	}

	return 0;
}

