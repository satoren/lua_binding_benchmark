#include "../benchmark.hpp"


BENCHMARK_DEFINE_LIBRARY_NAME("skelton code")


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	benchmark_exec(global_table);//pass to
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	"t1={t2={t3={}}}"//create table 
	benchmark_exec(table);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	global_table["native_function"] = &native_function;//bind native_function to lua global(native_function)
	state(lua_code);//execute lua_code
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	state(register_lua_function_code);//register lua function
	kaguya::LuaFunction f = state[lua_function_name]; //get lua function reference
	benchmark_exec(f);//execute with lua function reference
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	//bind TestClass::set TestClass::get

	TestClass getset;
	state["getset"] = &getset;//TestClass assign to "getset" of global
	lua_dostring(L,lua_code);//execute lua_code
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	kaguya::State state;
	state["ReturnObject"].setClass(kaguya::UserdataMetatable<TestClass>()
		.addFunction("set", &TestClass::set)
		.addFunction("get", &TestClass::get)
	);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END