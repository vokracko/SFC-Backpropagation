#include "params.h"

void Params::parse(int argc, char * argv[])
{
	m_interactive = argc == 1;
	std::string line;

	if(m_interactive)
	{
		while(true)
		{
			std::cout << "1) train neural network" << std::endl;
			std::cout << "2) predict" << std::endl;
			std::cout << "Select mode: ";
			std::getline(std::cin, line);

			if(line == "1")
				m_mode = TRAIN;
			else if(line == "2")
				m_mode = PREDICT;
			else
			{
				std::cerr << "Invalid selection" << std::endl;
				continue;
			}

			break;
		}

		return;
	}

	char c;

	while((c = getopt(argc, argv, "n:l:m:e:i:tph")) != -1)
	{
		switch(c)
		{
			case 'm':
				m_learning_rate = getDouble("Learing rate", optarg, 0, 1);
				break;
			case 'e':
				m_error_rate = getDouble("Error rate", optarg, 0, std::numeric_limits<double>::max());
				break;
			case 'l':
				--optind;
				loadLayers(argc, argv);
				break;
			case 'n':
				m_network_file = optarg;
				break;
			case 'i':
				m_input_file = optarg;
				break;
			case 't':
				m_mode = TRAIN;
				break;
			case 'p':
				m_mode = PREDICT;
				break;
			default:
				throw std::invalid_argument("Invalid argument");
				break;
		}

	}

	if(m_mode == UNDEFINED)
		throw std::invalid_argument("use -t for train or -p for predict mode");
	if(m_mode == PREDICT && (m_input_file.empty() || m_network_file.empty()))
		throw std::invalid_argument("Invalid combination of arguments");
	if(m_mode == TRAIN && (m_input_file.empty() || m_network_file.empty() || m_error_rate == UNDEFINED || m_learning_rate == UNDEFINED || mn_neurons.empty()))
		throw std::invalid_argument("Invalid combination of arguments");
}

double Params::getLearningRate()
{
	if(!m_interactive)
		return m_learning_rate;

	double min = 0;
	double max = 1;
	std::string line;

	while(!std::cin.eof())
	{
		try
		{
			std::cout << "Learning rate " << "(" << min << "," << max << "): ";
			std::getline(std::cin, line);
			m_learning_rate = getDouble("Error rate", line.c_str(), min, max);
		} catch(std::exception & e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}

		break;
	}

	return m_learning_rate;
}

double Params::getErrorRate()
{
	if(!m_interactive)
		return m_error_rate;

	double min = 0;
	double max = std::numeric_limits<double>::max();
	std::string line;

	while(!std::cin.eof())
	{
		try
		{
			std::cout << "Error rate " << "(" << min << "," << max << "): ";
			std::getline(std::cin, line);
			m_error_rate = getDouble("Error rate", line.c_str(), min, max);
		} catch(std::exception & e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}

		break;
	}

	return m_error_rate;
}

std::string & Params::getNetworkFile()
{
	if(!m_interactive)
		return m_network_file;

	while(!std::cin.eof())
	{
		std::cout << "Enter file name for network file: ";
		std::getline(std::cin, m_network_file);

		if(m_network_file.length() == 0)
			std::cerr << "Invalid input, try again" << std::endl;
		else
			break;
	}

	return m_network_file;
}

int Params::getMode()
{
	return m_mode;
}

std::vector<int> & Params::getLayers()
{
	if(!m_interactive)
		return mn_neurons;

	std::cout << "Enter number of inputs, number of neurons in hidden layers and number of outputs." << std::endl;
	std::cout << "Each number has to be on single line, to exit press enter on empty line." << std::endl;

	int value;
	std::string line;

	while(!std::cin.eof())
	{
		std::getline(std::cin, line);

		if(line.length() == 0)
			break;

		try
		{
			value = std::stoi(line.c_str());

			if(value < 1)
				throw std::exception();
		} catch(std::exception & e)
		{
			std::cerr << "Invalid input, insert positive number" << std::endl;
			continue;
		}

		mn_neurons.push_back(value);
	}


	if(mn_neurons.size() < 3)
		throw std::invalid_argument("Neural network has to have at least one hidden layer");

	return mn_neurons;
}

void Params::loadLayers(int argc, char * argv[])
{

	while(optind < argc && argv[optind][0] != '-')
	{
		int value = getInt("Neurons", argv[optind++], 0, std::numeric_limits<int>::max());
		mn_neurons.push_back(value);
	}


	if(mn_neurons.size() < 3)
		throw std::invalid_argument("Neural network has to have at least one hidden layer");
}

std::string & Params::getInputFile()
{
	if(!m_interactive)
		return m_input_file;

	while(!std::cin.eof())
	{
		std::cout << "Enter file name for input file: ";
		std::getline(std::cin, m_input_file);

		if(m_input_file.length() == 0)
			std::cerr << "Invalid input, try again" << std::endl;
		else
			break;
	}

	return m_input_file;
}

void Params::dump()
{
	std::string mode = m_mode == TRAIN ? "TRAIN" : "PREDICT";
	std::cout << "Mode: " << mode << std::endl;
	std::cout << "-n Network file: " << m_network_file << std::endl;
	std::cout << "-i Input file: " << m_input_file << std::endl;

	if(m_error_rate != UNDEFINED)
		std::cout << "-e Error rate: " << m_error_rate << std::endl;

	if(m_error_rate != UNDEFINED)
		std::cout << "-m Learning rate: " << m_learning_rate << std::endl;

	if(m_error_rate != UNDEFINED)
	{
		std::cout << "-l Layers: ";

		for(auto it = mn_neurons.begin(); it != mn_neurons.end(); ++it)
			 std::cout << *it << " ";

		std::cout << std::endl;
	}
}

bool Params::isInteractive()
{
	return m_interactive;
}
