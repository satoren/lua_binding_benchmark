#include "benchmark/benchmark.hpp"

#if LUA_VERSION_NUM >= 502
#include "selene.h"

BENCHMARK_DEFINE_LIBRARY_NAME("Selene")


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	state(reg_table_lua_code);
	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	state["native_function"] = &native_function;
	state(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	state(register_lua_function_code);
	auto f = state[lua_function_name];
	benchmark_exec(f);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	state["TestClass"].SetClass<TestClass>("set", &TestClass::set
		, "get", &TestClass::get);
	state("getset = TestClass.new()");
	state(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	state["TestClass"].SetClass<TestClass>("set", &TestClass::set
		, "get", &TestClass::get);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END


struct mt19937_wrap
{
	std::mt19937 content;
	mt19937_wrap(std::mt19937::result_type seed) :content(seed) {}
	unsigned int generate() {
		return content();
	}
};
struct uniform_int_distribution_wrap
{
	std::uniform_int_distribution<int> content;
	uniform_int_distribution_wrap(int min,int max) :content(min,max) {}
	int generate(mt19937_wrap* data) {
		return content(data->content);
	}
};
STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	auto random = state["random"];
	random["mt19937"].SetClass<mt19937_wrap,int>("__call", &mt19937_wrap::generate);
	
	random["uniform_int_distribution"].SetClass<uniform_int_distribution_wrap, int,int>("__call"
		, &uniform_int_distribution_wrap::generate);

	state(lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END

#endif// LUA_VERSION_NUM >= 502