#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <fstream>
#include <string>
#include <ostream>
#include <iostream>

#include <algorithm>


constexpr int BENCHMARK_LOOP_COUNT = 1000000;
namespace Benchmark
{

	struct Timer
	{
		lua_State *state_;

		Timer() :state_(luaL_newstate())
		{
			luaL_openlibs(state_);
			lua_getglobal(state_, "os");
			lua_getfield(state_, -1, "clock");
		}

		~Timer()
		{
			lua_close(state_);
		}
		double clock()const
		{
			lua_pushvalue(state_, -1);
			lua_pcall(state_, 0, 1, 0);
			double t = lua_tonumber(state_, -1);
			lua_pop(state_, 1);
			return t;
		}
	};


	std::ofstream out;
	bool executed = false;
	bool title_only = false;

	template<typename F>
	void execute_benchmark(const std::string& benchmark_name, F function)
	{
		if (title_only)
		{
			Benchmark::out << "," << benchmark_name;
			std::cout << "," << benchmark_name;
			return;
		}
		Timer timer;
		double total_time = 0;
		double min_time = std::numeric_limits<double>::max();
		static const int N = 10;
		for (int i = 0; i < N; ++i)
		{
			executed = false;
			double start = timer.clock();
			function();
			double end = timer.clock();

			if (executed)
			{
				total_time += end - start;
				min_time = std::min(min_time, end - start);
			}
		}
		if (min_time == std::numeric_limits<double>::max())
		{
			min_time = 0;
		}
		Benchmark::out << "," << min_time * 1000;
		std::cout << "," << min_time * 1000;

	}

	template<typename T>
	void global_table(T& globaltable)
	{
		executed = true;
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			globaltable["value"] = i;
			int v = globaltable["value"];
			if (v != i) { throw std::logic_error(""); }
		}
	}
	template<typename T>
	void table_chain_access(T& table)
	{
		executed = true;
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			table["t1"]["value"] = i;
			int v = table["t1"]["value"];
			if (v != i) { throw std::logic_error(""); }
		}
	}

	int native_function(int arg)
	{
		executed = true;
		return arg;
	}
	const char* native_function_lua_code()
	{
		return 
			"local times = 1000000\n"
			"for i=1,times do\n"
			"local r = native_function(i)\n"
			"if(r ~= i)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}


	class SetGet
	{
	public:
		SetGet() :_mem(0.0) {}
		void set(double i)
		{
			_mem = i;
		}
		double get()const
		{
			return _mem;
		}
	private:
		double _mem;
	};
	const char* object_set_get_lua_code()
	{
		executed = true;
		return 
			"local times = 1000000\n"
			"for i=1,times do\n"
			"getset:set(i)\n"
			"if(getset:get() ~= i)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}
}

void binding_begin();
void binding_end();

const char* binding_name();

void binding_global_table();
void binding_table_chain();
void binding_native_function_call();
void binding_object_set_get();

int main(int argc, const char* argv[])
{
	Benchmark::out.open(std::string("result.csv"), std::ios::out | std::ios::app | std::ios::binary);
	binding_begin();
	Benchmark::out << binding_name();
	std::cout << binding_name();

	Benchmark::execute_benchmark("global table", &binding_global_table);
	Benchmark::execute_benchmark("table chain", &binding_table_chain);
	Benchmark::execute_benchmark("native function", &binding_native_function_call);
	Benchmark::execute_benchmark("object member set get", &binding_object_set_get);

	Benchmark::out << std::endl;
	std::cout << std::endl;


	binding_end();
}
