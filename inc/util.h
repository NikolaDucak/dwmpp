#pragma once 

#include <iostream>
#include <string>

namespace util
{
	template< typename ... Args>
	inline void die(const std::string& str, Args... args){
		fprintf(stderr, str.c_str(), args ... );
		exit(-1);
	}
};
