#include <stdexcept>
#include <limits>
#include <cstdlib>
#include <iostream> 


double getDouble(const char * p_name, const char * p_src, double min, double max);
int getInt(const char * p_name, const char * p_src, int min, int max);

#ifndef _TPL_COMPARE_LIMITS
#define _TPL_COMPARE_LIMITS
template<class T>
void compare_limits(const char * p_name, T value, T min, T max)
{
	if(max == min) 
		return;
	else if(value >= max)
		throw std::logic_error(std::string(p_name) + " bigger than or equal to max (" + std::to_string(max) +")");

	else if(value <= min)
		throw std::logic_error(std::string(p_name) + " lesser than or equal to min ("+ std::to_string(min) + ")");
}
#endif
