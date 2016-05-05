#include "kaguya/kaguya.hpp"
#include "../benchmark.hpp"

void binding_begin()
{
}
void binding_end()
{
}

const char* binding_name()
{
	return "kaguya";
}
void binding_global_table()
{
	kaguya::State state;
	Benchmark::global_table(state);
}

void binding_table_chain()
{
	kaguya::State state;
	state("t1={t2={t3={}}}");
	Benchmark::table_chain_access(state);
}

void binding_native_function_call()
{
	kaguya::State state;
	state["native_function"] = &Benchmark::native_function;
	state(Benchmark::native_function_lua_code());
}

void binding_lua_function_call()
{
	kaguya::State state;
	state(Benchmark::register_lua_function_lua_code());
	kaguya::LuaFunction f = state[Benchmark::lua_function_name()];
	Benchmark::lua_function_call(f);
}

void binding_object_set_get()
{
	kaguya::State state;
	state["SetGet"].setClass(kaguya::UserdataMetatable<Benchmark::SetGet>()
		.setConstructors<Benchmark::SetGet()>()
		.add("set", &Benchmark::SetGet::set)
		.add("get", &Benchmark::SetGet::get)
		);

	Benchmark::SetGet getset;
	state["getset"] = &getset;
	state(Benchmark::object_set_get_lua_code());
}
