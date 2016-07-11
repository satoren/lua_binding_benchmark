#pragma once
#include <fstream>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <ctime>

constexpr int BENCHMARK_LOOP_COUNT = 5000000;
#define BENCHMARK_LOOP_COUNT_STR "5000000"


template<typename T, typename O>T benchmark_type_cast(O && obj)
{
	return std::forward<O>(obj);
}

namespace benchmark
{
	class BenchmarkRunner
	{
		BenchmarkRunner() {}
		BenchmarkRunner(const BenchmarkRunner&);
		BenchmarkRunner& operator=(const BenchmarkRunner&);

		typedef bool(*FunctionType)();

		typedef std::map<std::string, FunctionType> FunctionMapType;

		FunctionMapType functions_;
		std::string title_;
	public:
		struct result
		{
			static const int TRY_COUNT = 10;

			void put(uint32_t time)
			{
				assert(times_count < TRY_COUNT);
				times[times_count++] = time;
				if (executed())
				{
					std::sort(times, times + TRY_COUNT);
				}
			}

			uint32_t time(int index = 0)const
			{
				return times[index];
			}

			result() :times_count(0) {}
			bool executed()const
			{
				return times_count == TRY_COUNT;
			}
		private:
			uint32_t times[TRY_COUNT];
			size_t times_count;
		};
		static BenchmarkRunner& instance()
		{
			static BenchmarkRunner ins;
			return ins;
		}

		void add(const std::string& name, FunctionType function)
		{
			functions_[name] = function;
		}
		void set_title(std::string t) { title_.swap(t); }

		result execute(FunctionType f)const
		{
			result res = {};

			for (int i = 0; i < result::TRY_COUNT; ++i)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				auto start = clock();
				bool exec = f();
				auto end = clock();
				if (exec)
				{
					res.put(static_cast<uint32_t>((end - start) / (CLOCKS_PER_SEC / 1000.0)));
				}
			}
			return res;
		}
		result execute(const std::string& name)const
		{
			FunctionMapType::const_iterator it = functions_.find(name);
			if (it != functions_.end())
			{
				return execute(it->second);
			}
			return result();
		}

		template<typename outtype>
		bool execute(const std::vector<std::string>& tests, outtype& out)
		{
			out << title_;
#ifdef BENCHMARK_WITHOUT_TYPESAFE
			out << "(*1)";
#endif
			for (auto name : tests)
			{
				result result = execute(name);
				out << ",";
				if (result.executed())
				{
					out << result.time();
				}
			}
			out << std::endl;
			return true;
		}
	};

#define BENCHMARK_FUNCTION_DEF(NAME,FUNCTION_NAME) bool FUNCTION_NAME();\
		struct FUNCTION_NAME##_register{\
			FUNCTION_NAME##_register()\
			{\
				benchmark::BenchmarkRunner::instance().add(NAME, &FUNCTION_NAME);\
			}\
		} FUNCTION_NAME##REG;\
		bool FUNCTION_NAME()

#define BENCHMARK_DEFINE_LIBRARY_NAME(NAME) struct libname_register{\
			libname_register(){ benchmark::BenchmarkRunner::instance().set_title(NAME); }\
		} libname_reg;
}
