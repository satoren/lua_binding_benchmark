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
#include <stdexcept>
#include <random>
#include <thread>


constexpr int BENCHMARK_LOOP_COUNT = 5000000;
#define BENCHMARK_LOOP_COUNT_STR "5000000"
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
			std::this_thread::yield();
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

	template<typename T,typename O>T benchmark_type_cast(O && obj)
	{
		return std::forward<O>(obj);
	}

	template<typename T>
	void global_table(T& globaltable)
	{
		executed = true;
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			globaltable["value"] = i;
			int v = benchmark_type_cast<int>(globaltable["value"]);
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
			int v = benchmark_type_cast<int>(table["t1"]["value"]);
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
			"local times = " BENCHMARK_LOOP_COUNT_STR "\n"
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
			"local times = " BENCHMARK_LOOP_COUNT_STR "\n"
			"for i=1,times do\n"
			"getset:set(i)\n"
			"if(getset:get() ~= i)then\n"
			"error('error')\n"
			"end\n"
			"end\n";
	}

	const char* register_lua_function_lua_code()
	{
		return "lua_function=function(i)return i;end";
	}
	const char* lua_function_name()
	{
		return "lua_function";
	}

	template<typename F>
	void lua_function_call(F& f)
	{
		executed = true;
		std::string teststring = "testtext";
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			std::string result = f(teststring);
			if (result != teststring) { throw std::logic_error(""); }
		}
	}


	namespace returning_class_object
	{
		class ReturnObject
		{
		public:
			ReturnObject() :_mem(0), _random(0) {
			}
			ReturnObject(int seed) :_mem(0), _random(0) {
				_random = seed;
				_data.resize(10, _random);
			}

			ReturnObject(const ReturnObject&) = default;
			ReturnObject& operator=(const ReturnObject&) = default;
			ReturnObject(ReturnObject&&) = default;
			ReturnObject& operator=(ReturnObject&& other) = default;
			~ReturnObject(){}
			bool operator==(const ReturnObject& other)
			{
				return _data == other._data;
			}
			bool operator!=(const ReturnObject& other)
			{
				return _data != other._data;
			}

			void set(double i)
			{
				_mem = i;
			}
			double get()const
			{
				return _mem;
			}
			int random()const { return _random; }
		private:
			std::vector<int> _data;
			double _mem;
			int _random;
		};


		int returned_object_random;
		ReturnObject object_function()
		{
			static int seed = 1;
			ReturnObject obj(seed++);
			returned_object_random = obj.random();
			return obj;
		}

		bool object_compare(const ReturnObject& other)
		{
			executed = true;
			if (returned_object_random != other.random())
			{
				abort();
				return false;
			}
			return true;
		}

		const char* lua_code()
		{
			return
				"local times = " BENCHMARK_LOOP_COUNT_STR "/10 \n"
				"obj = object_function()\n"
				"obj:set(3)\n"
				"if(obj:get() ~= 3)then\n"
				"  error('error')\n"
				"end\n"
				"for i=1,times do\n"
				"  obj = object_function()\n"
				"  object_compare(obj)\n"
				"end\n";
		}
	}


}

void binding_begin();
void binding_end();

const char* binding_name();

void binding_global_table();
void binding_table_chain();
void binding_native_function_call();
void binding_object_set_get();
void binding_returning_object();
void binding_lua_function_call();

int main(int argc, const char* argv[])
{
	Benchmark::out.open(std::string("result.csv"), std::ios::out | std::ios::app | std::ios::binary);
	binding_begin();
	Benchmark::out << binding_name();
	std::cout << binding_name();

	Benchmark::execute_benchmark("global table", &binding_global_table);
	Benchmark::execute_benchmark("table chain", &binding_table_chain);
	Benchmark::execute_benchmark("c function call", &binding_native_function_call);
	Benchmark::execute_benchmark("lua function call", &binding_lua_function_call);
	Benchmark::execute_benchmark("C++ object member call", &binding_object_set_get);
	Benchmark::execute_benchmark("Returning a class object", &binding_returning_object);

	Benchmark::out << std::endl;
	std::cout << std::endl;


	binding_end();
}
