
#include <functional>
#include <iostream>
#include <luacppinterface.h>

#include "../benchmark.hpp"

void binding_begin()
{   
}
void binding_end()
{
}
const char* binding_name()
{
	return "luacppinterface";
}
namespace
{
//luacppinterface is bracket operator not supported
struct GlobalTableWrap
{
	LuaTable table_;
	const char* current_key;
	GlobalTableWrap(LuaTable& table) :table_(table),current_key(0) {}
	GlobalTableWrap& operator[](const char* key)
	{
		current_key = key;
		return *this;
	}

	int operator=(int value)
	{
		table_.Set(current_key,value);
		return value;
	}
	operator int()
	{
		int v = table_.Get<int>(current_key);
		current_key = 0;
		return v;
	}
};
struct LuaTableWrap
{
	LuaTable table_;
	const char* current_key;
	LuaTableWrap(LuaTable table) :table_(table),current_key(0) {}
	LuaTableWrap(LuaTable table,const char* key) :table_(table),current_key(key) {}
	LuaTableWrap operator[](const char* key)
	{
		if(current_key)
		{
			return LuaTableWrap(table_.Get<LuaTable>(current_key),key);
		}
		else
		{
			return LuaTableWrap(table_,key);
		}
	}

	int operator=(int value)
	{
		table_.Set(current_key,value);
		return value;
	}
	operator int()
	{
		int v = table_.Get<int>(current_key);
		current_key = 0;
		return v;
	}
};
}

void binding_global_table()
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	GlobalTableWrap tablewrap(global);
	Benchmark::global_table(tablewrap);
}


void binding_table_chain()
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	lua.RunScript("t1={t2={t3={}}}");
	LuaTableWrap tablewrap(global);
	Benchmark::table_chain_access(tablewrap);
}

void binding_native_function_call()
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	auto native_function = lua.CreateFunction<int(int)>(&Benchmark::native_function);
	
	global.Set("native_function", native_function);
	lua.RunScript(Benchmark::native_function_lua_code());
}


void binding_lua_function_call()
{
}

void binding_object_set_get()
{

Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
auto SetGet = lua.CreateUserdata<Benchmark::SetGet>(new Benchmark::SetGet());
SetGet.Bind("set", &Benchmark::SetGet::set);
SetGet.Bind("get", &Benchmark::SetGet::get);

global.Set("getset", SetGet);

	lua.RunScript(Benchmark::object_set_get_lua_code());
}
