#include "../benchmark.hpp"

#define LUAINTF_HEADERS_ONLY 1
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf/LuaIntf.h"

void binding_begin()
{   
}
void binding_end()
{
}
const char* binding_name()
{
	return "lua-intf";
}

struct LuaTableWrap
{
	LuaIntf::LuaRef table_;
	std::string current_key;
	LuaTableWrap(LuaIntf::LuaRef table) :table_(table) {}
	LuaTableWrap& operator[](const char* key)
	{
		if (current_key.empty())
		{
			current_key = key;
		}
		else
		{
			current_key +=std::string(".") + key;
		}
		return *this;
	}

	int operator=(int value)
	{
		table_[current_key] = value;
		current_key.clear();
		return value;
	}
	operator int()
	{
		int v = table_.get<int>(current_key);
		current_key.clear();
		return v;
	}
};

void binding_global_table()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	LuaTableWrap table(LuaIntf::LuaRef(state, "_G"));
	Benchmark::global_table(table);
}

void binding_table_chain()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	LuaTableWrap table(LuaIntf::LuaRef(state, "_G"));
	luaL_dostring(state,"t1={t2={t3={}}}");
	Benchmark::table_chain_access(table);
}

void binding_native_function_call()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	LuaIntf::LuaRef global_table(state, "_G");
	global_table.set("native_function",&Benchmark::native_function);
	luaL_dostring(state, Benchmark::native_function_lua_code());
}


struct FunctionWrap
{
	LuaIntf::LuaRef f_;
	FunctionWrap(LuaIntf::LuaRef f) :f_(f)
	{
	}

	std::string operator()(const std::string& v)
	{
		return f_.call<std::string>(v);
	}
};

void binding_lua_function_call()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	luaL_dostring(state, Benchmark::register_lua_function_lua_code());
	LuaIntf::LuaRef f(state, Benchmark::lua_function_name());
	Benchmark::lua_function_call(FunctionWrap(f));

	lua_close(state);
}

void binding_object_set_get()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	LuaIntf::LuaBinding(state)
		.beginClass<Benchmark::SetGet>("SetGet")
		.addConstructor(LUA_ARGS())
		.addFunction("set", &Benchmark::SetGet::set)
		.addFunction("get", &Benchmark::SetGet::get)
		.endClass();


	luaL_dostring(state, "getset = SetGet()");
	luaL_dostring(state, Benchmark::object_set_get_lua_code());
}
