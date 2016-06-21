#pragma once
#include <random>
#include "benchmark.hpp"

namespace benchmark
{
	namespace std_random_bind
	{
		const char* lua_code = "local times = " BENCHMARK_LOOP_COUNT_STR "\n"
			"local rengine=random.mt19937.new(0)\n"
			"local udist=random.uniform_int_distribution.new(1,6)\n"
			"if(rengine() ~= 2357136044) then error('mt19937?') end\n"
			"if(pcall(udist.__call,udist,udist) ~= false)then error('no error checked') end\n"//ouch! argument miss. error check
			"if(pcall(udist.__call,rengine,rengine) ~= false)then error('no error checked') end\n"
			"for i=1,times do\n"
			"local value = udist(rengine)\n"
			"if( value < 1 or value > 6)then\n"
			"error('error')\n"
			"end\n"
			"end\n";

		const char* call_constructor_version_lua_code = "local times = " BENCHMARK_LOOP_COUNT_STR "\n"
			"local rengine=random.mt19937(0)\n"
			"local udist=random.uniform_int_distribution(1,6)\n"
			"if(rengine:gen() ~= 2357136044) then error('mt19937?') end\n"
			"if(pcall(udist.gen,udist,udist) ~= false)then error('no error checked') end\n"//ouch! argument miss. error check
			"if(pcall(udist.gen,rengine,rengine) ~= false)then error('no error checked') end\n"
			"for i=1,times do\n"
			"local value = udist:gen(rengine)\n"
			"if( value < 1 or value > 6)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}
}

#define STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("std random(C++11) bind",std_random_bind_bench_function){ using namespace benchmark::std_random_bind;

#define STD_RANDOM_BIND_BENCHMARK_FUNCTION_END return true;}
