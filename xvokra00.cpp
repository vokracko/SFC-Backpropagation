#include "xvokra00.h"

double getDouble(const char * p_name, const char * p_src, double min, double max)
{
	char * p_end;
	double res = std::strtod(p_src, &p_end);
	compare_limits(p_name, res, min, max);

	return res;
}

int getInt(const char * p_name, const char * p_src, int min, int max)
{
	int res = std::stoi(p_src);
	compare_limits(p_name, res, min, max);

	return res;
}

