#include "network.h"

Network::Network(std::vector<int> & rv_layers)
{
	std::srand(std::time(0));

	unsigned input_size = rv_layers.front();
	rv_layers.erase(rv_layers.begin());
	mv_network.resize(rv_layers.size());

	for(unsigned l = 0; l < rv_layers.size(); ++l)
	{
		mv_network[l].resize(rv_layers[l]);

		for(int n = 0; n < rv_layers[l]; ++n)
		{
			mv_network[l][n].resize(input_size + 1);

			for(unsigned j = 0; j <= input_size; ++j) // <= adding weight for bias
				mv_network[l][n][j] = std::rand()/(double) RAND_MAX - 0.5;
		}

		input_size = mv_network[l].size();
	}
}


Network::Network(std::string & r_name)
{
	std::ifstream file(r_name.c_str());

	if(!file.good())
		throw std::invalid_argument("Failed to open network file " + r_name);

	std::string line;
	char * next_char;
	double weight;
	unsigned neurons_in_prev_layer;

	while(!file.eof())
	{
		std::vector<double> v_weights;
		std::vector<std::vector<double>> v_neurons;

		std::getline(file, line);

		if(line.length() == 0)
			break;

		while(line.length())
		{
			weight = std::strtod(line.c_str(), &next_char);
			v_weights.push_back(weight);

			if(*next_char == '\0')
				break;
			else if(*next_char == '\t')
			{
				if(v_neurons.empty() && mv_network.empty()) // first layer & first neuron can have any number of inputs but every other neuron in same layers has t ohave same number
					neurons_in_prev_layer = v_weights.size();

				if(v_weights.size() != neurons_in_prev_layer) // every neuron in layers > 1 must have number of inputs same as number of neurons in prev layer
					throw std::invalid_argument("Network file in wrong format");

				v_neurons.push_back(v_weights);
				v_weights.clear();
			}
            else if(*next_char != ' ')
				throw std::ios_base::failure("Network file in invalid format");

			line = line.substr(next_char + 1 - line.c_str());
		}

		v_neurons.push_back(v_weights);
		mv_network.push_back(v_neurons);
		neurons_in_prev_layer = v_neurons.size();
	}

	if(mv_network.size() < 2)
		throw std::invalid_argument("Network file in wrong format");
}

void Network::setParams(Params & r_params)
{
	m_learning_rate = r_params.getLearningRate();
	m_error_rate = r_params.getErrorRate();
}

void Network::save(std::string & r_name)
{
	std::ofstream file(r_name.c_str());

	if(!file.good())
		throw std::invalid_argument("Failed to open network file " + r_name);

	for(auto it_layer = mv_network.begin(); it_layer != mv_network.end(); ++ it_layer)
	{
		for(auto it_neuron = it_layer->begin(); it_neuron != it_layer->end(); ++ it_neuron)
		{
			if(it_neuron != it_layer->begin())
				file << '\t';

			for(auto it_weight = it_neuron->begin(); it_weight != it_neuron->end(); ++ it_weight)
			{
				if(it_weight != it_neuron->begin())
					file << " ";

				file << *it_weight;
			}
		}

		file << std::endl;
	}
}

void Network::loadTrainData(std::string & r_file_name)
{
	std::ifstream file(r_file_name.c_str());

	if(!file.good())
		throw std::invalid_argument("Failed to open input file "+ r_file_name);


	while(!file.eof())
	{
		std::string line;
		std::vector<double> v_input, v_output;
		bool output = false;

		std::getline(file, line);

		if(line.length() == 0)
			break;

		while(line.length())
		{
			char * next_char;
			double value = std::strtod(line.c_str(), &next_char);

			if(output)
				v_output.push_back(value);
			else
				v_input.push_back(value);

			if(line.length() <= (unsigned) (next_char - line.c_str()))
				break;
			else if(*next_char == '\t')
				output = 1;
			else if(*next_char != ' ')
				throw std::ios_base::failure("Train file in invalid format");

			line = line.substr(next_char + 1 - line.c_str());
		}

		mv_train_data.push_back(std::make_pair(v_input, v_output));
	}

	unsigned n_input = mv_train_data.front().first.size();
	unsigned n_output = mv_train_data.front().second.size();


	for(auto it = mv_train_data.begin() + 1; it != mv_train_data.end(); ++it)
	{
		if(it->first.size() != n_input || it->second.size() != n_output)
			throw std::ios_base::failure("Train vectors must have same size");
	}
}

void Network::dump()
{
	std::ostream_iterator<double> out_it(std::cerr, " ");

	// for(auto it = mv_train_data.begin(); it != mv_train_data.end(); ++it)
	// {
	// 	std::copy(it->first.begin(), it->first.end(), out_it);
	// 	std::cerr << "\t";
	// 	std::copy(it->second.begin(), it->second.end(), out_it);
	// 	std::cerr << std::endl;
	// }

	int l=0, n=0;
	for(auto it_layer = mv_network.begin(); it_layer != mv_network.end(); ++it_layer)
	{
		l++;
		std::cerr << "Layer " << l << std::endl;

		for(auto it_neuron = it_layer->begin(); it_neuron != it_layer->end(); ++it_neuron)
		{
			n++;
			std::cerr << "Neuron " << n << " -- ";
			for(auto it_weight = it_neuron->begin(); it_weight != it_neuron->end(); ++it_weight)
				std::cerr << *it_weight << " ";

			std::cerr << std::endl;
		}

		n = 0;
	}
}
void Network::train()
{

	double global_error = 0;
	double error = 0;
	unsigned epoch = 1;
	unsigned L = mv_network.size(); // number of layers
	std::vector<std::vector<double>> v_deltas;
	std::vector<std::vector<double>> v_inputs;
	std::vector<std::vector<double>> v_potentials;

	if(mv_train_data[0].first.size() != mv_network[0][0].size() - 1) // -1 bias 
		throw std::invalid_argument("Size of input vector does not match network");
	if(mv_train_data[0].second.size() != mv_network[L-1].size()) 
		throw std::invalid_argument("Size of output vector does not match network");

	v_deltas.resize(mv_network.size());
	v_inputs.resize(mv_network.size() + 1); // +1 for results
	v_potentials.resize(mv_network.size());

	// make deltas the right size
	for(unsigned l = 0; l < L; ++l)
	{
		v_deltas[l].resize(mv_network[l].size());
		v_inputs[l].resize(mv_network[l][0].size());
		v_potentials[l].resize(mv_network[l].size());
	}

	std::cout.precision(5);
	std::cout << std::fixed; 

	v_inputs[L].resize(mv_network[L - 1].size() +1); // number of outputs (+1 for bias, generic indexing)

	do
	{
		global_error = 0;

		for(unsigned p = 0; p < mv_train_data.size(); ++p) // p = index for train vector
		{
			v_inputs[0] = mv_train_data[p].first; // copy input data
			v_inputs[0].push_back(1); // apend "1" as bias

			for(unsigned l = 0; l < L; ++l) // for every layer
			{
				for(unsigned j = 0; j < mv_network[l].size(); ++j) // for every neuron in this layers
				{
					double u = 0;
					if(v_inputs[l].size() != mv_network[l][0].size())
					{
						std::cout << v_inputs[l].size() << std::endl;
						std::cout << mv_network[l][0].size() << std::endl;
					}

					assert(v_inputs[l].size() == mv_network[l][0].size());

					for(unsigned i = 0; i < mv_network[l][j].size(); ++i) // for every input for neuron in this layers (= number of neurons in previous layer)
					{
						u += mv_network[l][j][i] * v_inputs[l][i];

                    }
					v_inputs[l + 1][j] = sigmoid(u);
					// v_inputs[l + 1][j] = l + 1 == L ? u : sigmoid(u);
				} // every neuron in layer end

				v_inputs[l + 1].back() = 1; // bias

			}
			error = 0;

			for(unsigned n = 0; n < mv_network[L - 1].size(); ++n) // count deltas for last layer
			{
				double neuron_output = v_inputs[L][n];
				double data_output = mv_train_data[p].second[n];
				error += pow(data_output - neuron_output, 2);
				v_deltas[L - 1][n] = (data_output - neuron_output);// * LAMBDA * neuron_output * (1 - neuron_output);
				// std::cerr << mv_train_data[p].first[0]  << " " << mv_train_data[p].first[1] << " " << neuron_output << std::endl;
			}

			global_error += 0.5 * error;

			for(int l = L - 2; l >= 0; --l)
				for(unsigned j = 0; j < mv_network[l].size(); ++j)
				{
					double sum = 0;

					for(unsigned k = 0; k < mv_network[l+1].size(); ++k)
						sum += v_deltas[l+1][k] * mv_network[l+1][k][j];

					v_deltas[l][j] = sum * LAMBDA * v_inputs[l + 1][j] * (1 - v_inputs[l + 1][j]);
				}

			for(unsigned l = 0; l < L; ++l)
				for(unsigned j = 0; j < mv_network[l].size(); ++j)
					for(unsigned i = 0; i < mv_network[l][j].size(); ++i)
					{
						mv_network[l][j][i] += m_learning_rate * v_deltas[l][j] * v_inputs[l][i];
					}
		} // for every train vector
		std::cout << "\rEpoch: " << epoch++ << ", global error = " << global_error  << ", error per input = " << global_error / (double) mv_train_data.size();
	}
	while(global_error > m_error_rate);

	std::cerr << std::endl << "Done" << std::endl;

}

double Network::sigmoid(double x)
{
	return 1 / (1 + exp(-(x * LAMBDA)));
}

std::vector<double> Network::predict(std::vector<double> & rv_input)
{
	if(rv_input.size() != mv_network[0][0].size() - 1) // -1 for bias weight
		throw std::ios_base::failure("Input data does not corespond to given network");

	std::vector<double> v_layer_input(rv_input.begin(), rv_input.end());
	std::vector<double> v_layer_output;

	for(unsigned l = 0; l < mv_network.size(); ++l) // every layer
	{
		v_layer_input.push_back(1); // bias

		for(unsigned j = 0; j < mv_network[l].size(); ++j) // every neuron in layer
		{
			double u = 0;

			for(unsigned i = 0; i < mv_network[l][j].size(); ++i) // every input
				u += mv_network[l][j][i] * v_layer_input[i];

			v_layer_output.push_back(sigmoid(u));
		}

		v_layer_input = v_layer_output;
		v_layer_output.clear();
	}

	return v_layer_input; // because input/output vectors are swapped
}

void Network::predictFile(std::string & r_file_name)
{
	std::ifstream file(r_file_name.c_str());

	if(!file.good())
		throw std::invalid_argument("Failed to open input file " + r_file_name);

	std::string line;
	char * next_char;
	std::vector<double> v_data;
	double value;
	std::ostream_iterator<double> out_it(std::cout, "\n");

	while(!file.eof())
	{
		std::getline(file, line);

		while(line.length() > 0)
		{
			value = std::strtod(line.c_str(), &next_char);
			v_data.push_back(value);

			if(*next_char == '\0')
				break;
			else if(*next_char != ' ')
				throw std::ios_base::failure("Input file in invalid format");

			line = line.substr(next_char + 1 - line.c_str());
		}

        if(v_data.size())
		{
			std::vector<double> v_output = predict(v_data);
			std::copy(v_output.begin(), v_output.end(), out_it);
		}

		v_data.clear();
	}
}
