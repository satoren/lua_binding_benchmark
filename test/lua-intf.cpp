#define LUAINTF_HEADERS_ONLY 1
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf/LuaIntf.h"

#include "benchmark/benchmark.hpp"


BENCHMARK_DEFINE_LIBRARY_NAME("lua-intf")



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
			current_key += std::string(".") + key;
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

GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaTableWrap table(LuaIntf::LuaRef(state, "_G"));
		benchmark_exec(table);
	}
	lua_close(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaTableWrap table(LuaIntf::LuaRef(state, "_G"));
		luaL_dostring(state, reg_table_lua_code);
		benchmark_exec(table);
	}
	lua_close(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaIntf::LuaRef global_table(state, "_G");
		global_table.set("native_function", &native_function);
		luaL_dostring(state, lua_code);
	}
	lua_close(state);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		luaL_dostring(state, register_lua_function_code);
		LuaIntf::LuaRef f(state, lua_function_name);
		benchmark_exec([&](const std::string& v) {return f.call<std::string>(v); });
	}
	lua_close(state);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaIntf::LuaBinding(state)
			.beginClass<TestClass>("TestClass")
			.addConstructor(LUA_ARGS())
			.addFunction("set", &TestClass::set)
			.addFunction("get", &TestClass::get)
			.endClass();

		luaL_dostring(state, "getset = TestClass()");
		luaL_dostring(state, lua_code);
	}
	lua_close(state);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaIntf::LuaBinding(state)
			.beginClass<TestClass>("TestClass")
			.addConstructor(LUA_ARGS())
			.addFunction("set", &TestClass::set)
			.addFunction("get", &TestClass::get)
			.endClass();

		LuaIntf::LuaRef global_table(state, "_G");
		global_table.set("object_function", &object_function);
		global_table.set("object_compare", &object_compare);

		luaL_dostring(state, lua_code);
	}
	lua_close(state);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END


STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		LuaIntf::LuaBinding(state)
			.beginModule("random")
				.beginClass<std::mt19937>("mt19937")
				.addConstructor(LUA_ARGS(std::mt19937::result_type))
				.addFunction("gen", &std::mt19937::operator())
				.endClass();

		typedef std::uniform_int_distribution<int> uni_int_dist;
		LuaIntf::LuaBinding(state)
			.beginModule("random")
			.beginClass<uni_int_dist>("uniform_int_distribution")
			.addConstructor(LUA_ARGS(int,int))
			.addMetaFunction("gen", [](uni_int_dist& dist, std::mt19937& gen) {return dist(gen); })
			.endClass();
		
		luaL_dostring(state, call_constructor_version_lua_code);
	}
	lua_close(state);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END