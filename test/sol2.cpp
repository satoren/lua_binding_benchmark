#include "sol.hpp"

#include "benchmark/benchmark.hpp"


BENCHMARK_DEFINE_LIBRARY_NAME("sol2")

GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.script("t1={t2={t3={}}}");
	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state["native_function"] = &native_function;
	state.script(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.script(register_lua_function_code);
	sol::function f = state[lua_function_name];
	benchmark_exec(f);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.new_usertype<TestClass>("TestClass",
		"set", &TestClass::set,
		"get", &TestClass::get
		);
	state.script("getset = TestClass.new()");
	state.script(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.new_usertype<TestClass>("TestClass",
		"set", &TestClass::set,
		"get", &TestClass::get
		);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state.script(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END