#include "../benchmark.hpp"
#include "sol.hpp"

void binding_begin()
{
}
void binding_end()
{
}
const char* binding_name()
{
	return "sol2";
}

void binding_global_table()
{
	sol::state state;
	Benchmark::global_table(state);
}

void binding_table_chain()
{
	sol::state state;
	state.script("t1={t2={t3={}}}");
	Benchmark::table_chain_access(state);
}

void binding_native_function_call()
{
	sol::state state;

	state.set_function("native_function", &Benchmark::native_function);
	state.script(Benchmark::native_function_lua_code());
}

void binding_lua_function_call()
{
	sol::state state;
	state.script(Benchmark::register_lua_function_lua_code());
	sol::function f = state[Benchmark::lua_function_name()];
	Benchmark::lua_function_call(f);
}

void binding_object_set_get()
{
	sol::state state;
	state.new_usertype<Benchmark::SetGet>("SetGet",
		"set", &Benchmark::SetGet::set,
		"get", &Benchmark::SetGet::get
		);
	state.script("getset = SetGet.new()");
	state.script(Benchmark::object_set_get_lua_code());
}


void binding_returning_object()
{
	using namespace Benchmark::returning_class_object;
	sol::state state;
	state.new_usertype<ReturnObject>("ReturnObject",
		"set", &ReturnObject::set,
		"get", &ReturnObject::get
		);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state.script(lua_code());
}
