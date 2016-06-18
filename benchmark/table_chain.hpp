#pragma once

#include "benchmark.hpp"

namespace benchmark
{
	namespace table_chain
	{
		bool executed_ = false;
		template<typename T>
		void benchmark_exec(T& table)
		{
			executed_ = true;
			for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
			{
				table["t1"]["value"] = i;
				int v = benchmark_type_cast<int>(table["t1"]["value"]);
				if (v != i) { throw std::logic_error(""); }
			}
		}
	}
}

#define TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("table chain",table_chain_bench_function){ using namespace benchmark::table_chain;executed_ = false;

#define TABLE_CHAIN_BENCHMARK_FUNCTION_END return executed_;}
