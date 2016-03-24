#include "../benchmark.hpp"


#if LUA_VERSION_NUM > 502
#define lua_tounsigned lua_tointeger
#endif

#include "sol.hpp"


void binding_begin()
{   
}
void binding_end()
{
}
const char* binding_name()
{
	return "sol";
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
	// Adding the test here to just have comparison numbers up anyways
	executed = true;
	for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
	{
		sol::table t1s = state["t1"];
		t1s["value"] = i;
		sol::table t1g = state["t1"];
		int v = t1g["value"];
		if (v != i) { throw std::logic_error(""); }
	}
}

struct FunctionWrap
{
	sol::function f_;
	FunctionWrap(sol::function f) :f_(f)
	{
	}

	std::string operator()(const std::string& v)
	{
		return f_.call<std::string>(v);
	}
};


void binding_lua_function_call()
{
	sol::state state;
	state.script(Benchmark::register_lua_function_lua_code());

	sol::function f = state["lua_function"];
	FunctionWrap fwrap(f);
	Benchmark::lua_function_call(fwrap);
}
void binding_native_function_call()
{
	sol::state state;

	//compile error at MSVC2015
#ifndef _MSC_VER
	state.set_function("native_function", Benchmark::native_function);
	state.script(Benchmark::native_function_lua_code());
#endif
}


void binding_object_set_get()
{
	sol::state state;
	//compile error at MSVC2015
#ifndef _MSC_VER
	state.new_userdata<Benchmark::SetGet>("SetGet", "set", &Benchmark::SetGet::set, "get", &Benchmark::SetGet::get);
	state.script("getset = SetGet.new()");
	state.script(Benchmark::object_set_get_lua_code());
#endif
}
