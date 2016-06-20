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


//don't work at gcc
//runtime error message: SeleneTried to copy an object of an unregistered type. Please register classes before passing instances by value.
/*
STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	sel::State state(true);
	auto random = state["random"];
	random["mt19937"].SetClass<std::mt19937,int>("__call", &std::mt19937::operator());

	typedef std::uniform_int_distribution<int> uni_int_dist;
	typedef std::uniform_int_distribution<int>::result_type (uni_int_dist::*generate_function_type)(std::mt19937&)
#ifdef _MSC_VER
		const//MSVC bug???
#endif
	;
	
	generate_function_type generate_function = &uni_int_dist::operator();
	random["uniform_int_distribution"].SetClass<uni_int_dist, int,int>("__call", generate_function);

	state(lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END
//*/
