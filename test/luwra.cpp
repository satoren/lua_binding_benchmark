#include "../benchmark.hpp"
#include <luwra.hpp>

void binding_begin() {}

void binding_end() {}

const char* binding_name() {
	return "luwra";
}

void binding_global_table() {
	luwra::StateWrapper state;
	state.loadStandardLibrary();

	Benchmark::global_table(state);
}

void binding_table_chain() {
	luwra::StateWrapper state;
	state.runString("t1={t2={t3={}}}");

	Benchmark::table_chain_access(state);
}

void binding_native_function_call() {
	luwra::StateWrapper state;

	state["native_function"] = LUWRA_WRAP(Benchmark::native_function);
	state.runString(Benchmark::native_function_lua_code());
}

void binding_lua_function_call() {
	luwra::StateWrapper state;
	state.runString(Benchmark::register_lua_function_lua_code());

	luwra::NativeFunction<std::string> func = state[Benchmark::lua_function_name()];
	Benchmark::lua_function_call(func);
}

void binding_object_set_get() {
	luwra::StateWrapper state;

	state.registerUserType<Benchmark::SetGet()>(
		"SetGet",
		{
			LUWRA_MEMBER(Benchmark::SetGet, set),
			LUWRA_MEMBER(Benchmark::SetGet, get)
		}
	);

	state.runString("getset = SetGet()");
	state.runString(Benchmark::object_set_get_lua_code());
}

void binding_returning_object()
{
	using namespace Benchmark::returning_class_object;
	luwra::StateWrapper state;

	state.registerUserType<ReturnObject()>(
		"ReturnObject",
		{
			LUWRA_MEMBER(ReturnObject, set),
			LUWRA_MEMBER(ReturnObject, get)
		}
	);

	state["object_function"] = LUWRA_WRAP(object_function);
	state["object_compare"] = LUWRA_WRAP(object_compare);
	state.runString(lua_code());
}
