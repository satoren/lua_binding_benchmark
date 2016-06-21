#include "kaguya/kaguya.hpp"
#include "benchmark/benchmark.hpp"

BENCHMARK_DEFINE_LIBRARY_NAME("kaguya")


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	state("t1={t2={t3={}}}");
	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	state["native_function"] = &native_function;
	state(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	state(register_lua_function_code);
	kaguya::LuaFunction f = state[lua_function_name];
	benchmark_exec(f);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
state["TestClass"].setClass(kaguya::UserdataMetatable<TestClass>()
		.setConstructors<TestClass()>()
		.addFunction("set", &TestClass::set)
		.addFunction("get", &TestClass::get)
	);
state("getset = TestClass.new()");
	state(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	state["TestClass"].setClass(kaguya::UserdataMetatable<TestClass>()
		.addFunction("set", &TestClass::set)
		.addFunction("get", &TestClass::get)
	);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END


STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;    
	kaguya::LuaTable random = state.newTable();

	random["mt19937"].setClass(kaguya::UserdataMetatable<std::mt19937>()
		.setConstructors<std::mt19937(std::mt19937::result_type)>()
		.addFunction("__call", &std::mt19937::operator())
	);

	typedef std::uniform_int_distribution<int> uni_int_dist;	
	random["uniform_int_distribution"].setClass(kaguya::UserdataMetatable<uni_int_dist>()
		.setConstructors<uni_int_dist(), uni_int_dist(int), uni_int_dist(int, int)>()
		.addStaticFunction("__call", [](uni_int_dist& dist, std::mt19937& gen) {return dist(gen); })
		);

	state["random"] = random;

	state.dostring(lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END