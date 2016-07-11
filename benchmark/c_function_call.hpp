#pragma once

#include "benchmark.hpp"

namespace benchmark
{
	namespace c_function_call
	{
		int call_count_=0;
		int native_function(int arg)
		{
			call_count_++;
			return arg;
		}


		const char* lua_code = "local times = " BENCHMARK_LOOP_COUNT_STR "\n"
#ifndef BENCHMARK_WITHOUT_TYPESAFE
			"if(pcall(native_function,'abc') ~= false)then error('no error checked') end\n"//type error detect check.
#endif
			"for i=1,times do\n"
			"local r = native_function(i)\n"
			"if(r ~= i)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}
}

#define C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("c function call",c_function_call_bench_function){ using namespace benchmark::c_function_call;call_count_=0;

#define C_FUNCTION_CALL_BENCHMARK_FUNCTION_END return call_count_ == BENCHMARK_LOOP_COUNT;}
