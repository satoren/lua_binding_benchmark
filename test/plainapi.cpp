
#include "../benchmark.hpp"

void binding_begin()
{   
}
void binding_end()
{
}

const char* binding_name()
{
	return "plain api";
}



struct GlobalTable
{
	lua_State *state_;
	const char* current_key;
	GlobalTable(lua_State *state) :state_(state), current_key(0) {}
	GlobalTable& operator[](const char* key)
	{
		current_key = key;
		return *this;
	}

	int operator=(int value)
	{
		lua_pushnumber(state_, value);
		lua_setglobal(state_, current_key);
		current_key = 0;
		return value;
	}
	operator int()
	{
		lua_getglobal(state_,  current_key);
		int v = static_cast<int>(lua_tonumber(state_, -1));
		lua_settop(state_, 0);
		current_key = 0;
		return v;
	}
};
void binding_global_table()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);

	GlobalTable t(state);
	Benchmark::global_table(t);
	lua_close(state);
}



struct TableChain
{
	lua_State *state_;
	const char* current_key;
	TableChain(lua_State *state) :state_(state), current_key(0) {}
	TableChain& operator[](const char* key)
	{
		if (current_key == 0)
		{
			lua_pushglobaltable(state_);
		}
		else
		{
			lua_getfield(state_, -1, current_key);
		}
		current_key = key;
		return *this;
	}

	int operator=(int value)
	{
		lua_pushnumber(state_, value);
		lua_setfield(state_, -2, current_key);
		lua_settop(state_, 0);
		current_key = 0;
		return value;
	}
	operator int()
	{
		lua_getfield(state_, -1, current_key);
		int v = static_cast<int>(lua_tonumber(state_, -1));
		lua_settop(state_, 0);
		current_key = 0;
		return v;
	}
};
void binding_table_chain()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	luaL_dostring(state,"t1={t2={t3={}}}");
	TableChain t(state);
	Benchmark::table_chain_access(t);
	lua_close(state);
}



int static_native_function_binding(lua_State* L)
{
	int arg = lua_tonumber(L, 1);
	int result = Benchmark::native_function(arg);
	lua_pushnumber(L, result);
	return 1;
}
void binding_native_function_call()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	lua_pushcclosure(state, static_native_function_binding, 0);
	lua_setglobal(state, "native_function");

	luaL_dostring(state,Benchmark::native_function_lua_code());
}
void binding_object_set_get()
{
}