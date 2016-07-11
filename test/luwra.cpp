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
	state.loadStandardLibrary();
	state.runString(reg_table_lua_code);

	benchmark_exec(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.loadStandardLibrary();

	state["native_function"] = LUWRA_WRAP(native_function);
	state.runString(lua_code);

}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.loadStandardLibrary();
	state.runString(register_lua_function_code);

	luwra::Function<std::string> func = state[lua_function_name];
	benchmark_exec(func);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.loadStandardLibrary();

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
	state.loadStandardLibrary();

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


int generate_function(std::uniform_int_distribution<int>& dist, std::mt19937& gen)
{
	return dist(gen);
}

//can not work at MSVC++2015 update 2
STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	luwra::StateWrapper state;
	state.loadStandardLibrary();

	state.registerUserType<std::mt19937(std::mt19937::result_type)>(
		"random_mt19937",
		{
			{ "gen", LUWRA_WRAP(std::mt19937::operator()) }
		}
	);

	typedef std::uniform_int_distribution<int> uni_int_dist;


//	typedef int (uni_int_dist::*generate_function_type)(std::mt19937&);
//	generate_function_type generate_function = static_cast<int (uni_int_dist::*)(std::mt19937&)>(&uni_int_dist::operator()<std::mt19937>);
//	auto generate_function = [](uni_int_dist& dist, std::mt19937& gen) {return dist(gen); };
	state.registerUserType<uni_int_dist(int,int)>(
		"random_uniform_int_distribution",	{
			{ "gen", LUWRA_WRAP(generate_function) }
		}
	);

	//I want register to random.mt19937 ,random.uniform_int_distribution
	state.runString("random={mt19937=random_mt19937,uniform_int_distribution = random_uniform_int_distribution}");
	state.runString(call_constructor_version_lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END
