#include <luwra.hpp>
#include "benchmark/benchmark.hpp"


BENCHMARK_DEFINE_LIBRARY_NAME("luwra")


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.loadStandardLibrary();

	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.runString("t1={t2={t3={}}}");

	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;

	state["native_function"] = LUWRA_WRAP(native_function);
	state.runString(lua_code);

}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.runString(register_lua_function_code);

	luwra::NativeFunction<std::string> func = state[lua_function_name];
	benchmark_exec(func);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;

	state.registerUserType<TestClass()>(
		"TestClass",
		{
			LUWRA_MEMBER(TestClass, set),
			LUWRA_MEMBER(TestClass, get)
		}
	);

	state.runString("getset = TestClass()");
	state.runString(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;

	state.registerUserType<TestClass()>(
		"TestClass",
		{
			LUWRA_MEMBER(TestClass, set),
			LUWRA_MEMBER(TestClass, get)
		}
	);

	state["object_function"] = LUWRA_WRAP(object_function);
	state["object_compare"] = LUWRA_WRAP(object_compare);
	state.runString(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END
