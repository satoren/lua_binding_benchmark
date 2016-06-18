#pragma once

#include "benchmark_runner.hpp"

namespace benchmark
{
	namespace lua_function_call
	{
		int call_count_ = 0;
		const char* register_lua_function_code = "lua_function=function(i)return i..i;end";
		const char* lua_function_name = "lua_function";

		template<typename T>
		void benchmark_exec(T f)
		{
			std::string teststring = "testtext";
			for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
			{
				call_count_++;
				std::string result = f(teststring);
				if (result != "testtexttesttext") { throw std::logic_error(""); }
			}
		}
	}
}

#define LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("lua function call",lua_function_call_bench_function){ using namespace benchmark::lua_function_call;call_count_=0;

#define LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END return call_count_ == BENCHMARK_LOOP_COUNT;}
