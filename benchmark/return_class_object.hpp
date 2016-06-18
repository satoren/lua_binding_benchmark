#pragma once

#include "benchmark_runner.hpp"

namespace benchmark
{
	namespace return_class_object
	{
		int call_count_ = 0;
		class TestClass
		{
		public:
			TestClass() :_mem(0), _random(0) {
			}
			TestClass(int seed) :_mem(0), _random(0) {
				_random = seed;
				_data.resize(10, _random);
			}

			TestClass(const TestClass&) = default;
			TestClass& operator=(const TestClass&) = default;
			TestClass(TestClass&&) = default;
			TestClass& operator=(TestClass&& other) = default;
			~TestClass() {}
			bool operator==(const TestClass& other)
			{
				return _data == other._data;
			}
			bool operator!=(const TestClass& other)
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


		int returned_object_seed;
		TestClass object_function()
		{
			static int seed = 1;
			TestClass obj(seed++);
			returned_object_seed = obj.random();
			return obj;
		}
		bool object_compare(const TestClass& testobj)
		{
			call_count_++;

			if (returned_object_seed != testobj.random())
			{
				abort();
				return false;
			}
			return true;
		}

		const char* lua_code =
			"local times = " BENCHMARK_LOOP_COUNT_STR "\n"
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

#define RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN BENCHMARK_FUNCTION_DEF("Returning a class object",return_class_object_bench_function){ using namespace benchmark::return_class_object;call_count_=0;

#define RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END return call_count_ == BENCHMARK_LOOP_COUNT;}
