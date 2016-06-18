#pragma once

#include "benchmark_runner.hpp"

namespace benchmark
{
	namespace object_member_call
	{
		int call_count_ = 0;
		class TestClass
		{
		public:
			TestClass() :_mem(0.0) {}
			void set(double i)
			{
				call_count_++;
				_mem = i;
			}
			double get()const
			{
				return _mem;
			}
		private:
			double _mem;
		};


		const char* lua_code = "local times = " BENCHMARK_LOOP_COUNT_STR "\n"
			"for i=1,times do\n"
			"getset:set(i)\n"
			"if(getset:get() ~= i)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}
}

#define OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("C++ object member call",object_member_call_bench_function){ using namespace benchmark::object_member_call;call_count_=0;

#define OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END return call_count_ == BENCHMARK_LOOP_COUNT;}
