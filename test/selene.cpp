#include "selene.h"
#include "../benchmark.hpp"

void binding_begin()
{
}
void binding_end()
{
}
const char* binding_name()
{
	return "Selene";
}

void binding_global_table()
{
	sel::State state;
	Benchmark::global_table(state);
}

void binding_table_chain()
{
	sel::State state;
	state("t1={t2={t3={}}}");
	Benchmark::table_chain_access(state);
}

void binding_native_function_call()
{
	sel::State state;
	state["native_function"] = &Benchmark::native_function;
	state(Benchmark::native_function_lua_code());
}

void binding_lua_function_call()
{
	sel::State state;

	state(Benchmark::register_lua_function_lua_code());
	auto f = state[Benchmark::lua_function_name()];
	Benchmark::lua_function_call(f);
}

void binding_object_set_get()
{
	sel::State state;
	state["SetGet"].SetClass<Benchmark::SetGet>("set", &Benchmark::SetGet::set
		, "get", &Benchmark::SetGet::get);
	state("getset = SetGet.new()");

	state(Benchmark::object_set_get_lua_code());
}


void binding_returning_object()
{
	using namespace Benchmark::returning_class_object;
	sel::State state;
	state["ReturnObject"].SetClass<ReturnObject>("set", &ReturnObject::set
		, "get", &ReturnObject::get);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state(lua_code());
}
