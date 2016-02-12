#include "../benchmark.hpp"

void binding_begin()
{    
	Benchmark::title_only = true;
	Benchmark::out.close();
	Benchmark::out.open(std::string("result.csv"), std::ios::out | std::ios::binary);
}
void binding_end()
{
    
}


const char* binding_name()
{
	return "Library";
}
void binding_global_table()
{
}

void binding_table_chain()
{
}

void binding_native_function_call()
{
}
void binding_lua_function_call()
{
	/*
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	luaL_dostring(state, Benchmark::register_lua_function_lua_code());

	lua_getglobal(state, Benchmark::lua_function_name());
	FunctionWrap f(state, luaL_ref(state, LUA_REGISTRYINDEX));
	Benchmark::lua_function_call(f);

	lua_close(state);*/
}


void binding_object_set_get()
{
}
