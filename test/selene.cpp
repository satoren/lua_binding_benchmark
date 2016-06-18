#include "selene.h"
#include "benchmark/benchmark.hpp"


BENCHMARK_DEFINE_LIBRARY_NAME("Selene")


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	state("t1={t2={t3={}}}");
	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	state["native_function"] = &native_function;
	state(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	state(register_lua_function_code);
	auto f = state[lua_function_name];
	benchmark_exec(f);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	state["TestClass"].SetClass<TestClass>("set", &TestClass::set
		, "get", &TestClass::get);
	state("getset = TestClass.new()");
	state(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state;
	state["TestClass"].SetClass<TestClass>("set", &TestClass::set
		, "get", &TestClass::get);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END

