#include <functional>
#include <iostream>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <luacppinterface.h>

#include "benchmark/benchmark.hpp"

BENCHMARK_DEFINE_LIBRARY_NAME("luacppinterface")


namespace
{
	//luacppinterface is bracket operator not supported
	struct GlobalTableWrap
	{
		LuaTable table_;
		const char* current_key;
		GlobalTableWrap(LuaTable& table) :table_(table), current_key(0) {}
		GlobalTableWrap& operator[](const char* key)
		{
			current_key = key;
			return *this;
		}

		int operator=(int value)
		{
			table_.Set(current_key, value);
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
		LuaTableWrap(LuaTable table) :table_(table), current_key(0) {}
		LuaTableWrap(LuaTable table, const char* key) :table_(table), current_key(key) {}
		LuaTableWrap operator[](const char* key)
		{
			if (current_key)
			{
				return LuaTableWrap(table_.Get<LuaTable>(current_key), key);
			}
			else
			{
				return LuaTableWrap(table_, key);
			}
		}

		int operator=(int value)
		{
			table_.Set(current_key, value);
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

GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	GlobalTableWrap tablewrap(global);
	benchmark_exec(tablewrap);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	lua.RunScript("t1={t2={t3={}}}");
	LuaTableWrap tablewrap(global);
	benchmark_exec(tablewrap);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	auto fn = lua.CreateFunction<int(int)>(&native_function);

	global.Set("native_function", fn);
	lua.RunScript(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	lua.RunScript(register_lua_function_code);
	LuaFunction<std::string(std::string)> fx = global.Get<LuaFunction<std::string(std::string)>>(lua_function_name);

	benchmark_exec([&](std::string x) {
		return fx.Invoke(x); 
	});
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	auto SetGet = lua.CreateUserdata(new TestClass());
	SetGet.Bind("set", &TestClass::set);
	SetGet.Bind("get", &TestClass::get);

	global.Set("getset", SetGet);

	lua.RunScript(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	auto object_f = lua.CreateFunction<LuaUserdata<TestClass>()>([&] {
		auto retobj = lua.CreateUserdata<TestClass>(new TestClass(object_function()));
		retobj.Bind("set", &TestClass::set);
		retobj.Bind("get", &TestClass::get);
		return retobj; });
	global.Set("object_function", object_f);

	auto object_compare_f = lua.CreateFunction<bool(LuaUserdata<TestClass>)>([](LuaUserdata<TestClass> userdata)
	{
		return object_compare(*userdata.GetPointer());
	});
	global.Set("object_compare", object_compare_f);
	lua.RunScript(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END
