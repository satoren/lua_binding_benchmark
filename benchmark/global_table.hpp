#pragma once

#include "benchmark_runner.hpp"

namespace benchmark
{
	namespace global_table
	{
		bool executed_ = false;
		template<typename T>
		void benchmark_exec(T& globaltable)
		{
			executed_ = true;
			for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
			{
				globaltable["value"] = i;
				int v = benchmark_type_cast<int>(globaltable["value"]);
				if (v != i) { throw std::logic_error(""); }
			}
		}
	}

#define GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("global table",global_table_bench_function){ using namespace benchmark::global_table;executed_ = false;

#define GLOBAL_TABLE_BENCHMARK_FUNCTION_END return executed_;}
}
