extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "luabind/luabind.hpp"
#include "luabind/object.hpp"


template<typename T>T benchmark_type_cast(luabind::adl::index_proxy<luabind::object>&& obj)
{
	return luabind::object_cast<T>(obj);
}
template<typename T>T benchmark_type_cast(luabind::adl::index_proxy<luabind::adl::index_proxy<luabind::object>>&& obj)
{
	return luabind::object_cast<T>(obj);
}

#include "benchmark/benchmark.hpp"

BENCHMARK_DEFINE_LIBRARY_NAME("luabind-deboostified")



GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		auto globaltable = globals(L);
		benchmark_exec(globaltable);
	}
	lua_close(L);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{

	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		luaL_dostring(L, reg_table_lua_code);
		auto globaltable = globals(L);
		benchmark_exec(globaltable);
	}
	lua_close(L);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		module(L)
			[
				def("native_function", &native_function)
			];
		luaL_dostring(L, lua_code);
	}
	lua_close(L);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		luaL_dostring(L, register_lua_function_code);
		luabind::object fn = globals(L)[lua_function_name];
		benchmark_exec([&](const std::string& x) {return luabind::call_function<std::string>(fn, x); });
	}
	lua_close(L);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		module(L)
			[
				class_<TestClass>("TestClass")
				.def(constructor<>())
			.def("set", &TestClass::set)
			.def("get", &TestClass::get)
			];
		luaL_dostring(L, "getset = TestClass()");
		luaL_dostring(L, lua_code);
	}
	lua_close(L);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		module(L)
			[
				class_<TestClass>("TestClass")
					.def(constructor<>())
					.def("set", &TestClass::set)
					.def("get", &TestClass::get),
				def("object_function", &object_function),
				def("object_compare", &object_compare)
			];
		luaL_dostring(L, lua_code);
	}
	lua_close(L);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END


STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);

		typedef std::uniform_int_distribution<int> uni_int_dist;

#ifdef _MSC_VER
		typedef int (uni_int_dist::*generate_function_type)(std::mt19937&)const;
#else
		typedef int (uni_int_dist::*generate_function_type)(std::mt19937&);
#endif
		generate_function_type generate_function =&uni_int_dist::operator()<std::mt19937>;

		module(L, "random")
			[
				class_<std::mt19937>("mt19937")
				.def(constructor<std::mt19937::result_type>())
				.def("gen", &std::mt19937::operator()),
				class_<uni_int_dist>("uniform_int_distribution")
				.def(constructor<int,int>())
				.def("gen", generate_function)
			];
		luaL_dostring(L, call_constructor_version_lua_code);
	}
	lua_close(L);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END
