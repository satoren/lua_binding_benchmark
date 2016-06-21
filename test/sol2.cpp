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

/* //do not work
STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.open_libraries();
	state.new_usertype<std::mt19937,int>("random_mt19937",
		"__call", &std::mt19937::operator()
	);

	typedef std::uniform_int_distribution<int> uni_int_dist;
	state.new_usertype<uni_int_dist, int,int>("random_uniform_int_distribution",
		"__call", [](uni_int_dist& dist, std::mt19937& gen) {return dist(gen); }
	);
	//I want register to random.mt19937 ,random.uniform_int_distribution
	state.script("random={mt19937=random_mt19937,uniform_int_distribution=random_uniform_int_distribution}");
	state.script(lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END
//*/
