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
	Benchmark::global_table(state);
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
