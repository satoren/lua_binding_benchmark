#include "luabind/luabind.hpp"
#include "luabind/object.hpp"


namespace Benchmark
{
	template<typename T>T benchmark_type_cast(luabind::adl::index_proxy<luabind::object>&& obj)
	{
		return luabind::object_cast<T>(obj);
	}
	template<typename T>T benchmark_type_cast(luabind::adl::index_proxy<luabind::adl::index_proxy<luabind::object>>&& obj)
	{
		return luabind::object_cast<T>(obj);
	}
}
#include "../benchmark.hpp"

void binding_begin()
{
}
void binding_end()
{
}
const char* binding_name()
{
	return "luabind-deboostified";
}
void binding_global_table()
{

	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		auto globaltable = globals(L);
		Benchmark::global_table(globaltable);
	}
	lua_close(L);
}

void binding_table_chain()
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		luaL_dostring(L, "t1={t2={t3={}}}");
		auto globaltable = globals(L);
		Benchmark::table_chain_access(globaltable);
	}
	lua_close(L);
}

void binding_native_function_call()
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		module(L)
			[
				def("native_function", &Benchmark::native_function)
			];
		luaL_dostring(L, Benchmark::native_function_lua_code());
	}
	lua_close(L);
}


struct LuaFunctionWrap {
	luabind::object fn;
	LuaFunctionWrap(luabind::object fnc) : fn(fnc) {
	}

	std::string operator() (const std::string& x) {
		return luabind::call_function<std::string>(fn, x);
	}
};
void binding_lua_function_call()
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		luaL_dostring(L, Benchmark::register_lua_function_lua_code());

		luaL_dostring(L, Benchmark::native_function_lua_code());
		LuaFunctionWrap f(globals(L)[Benchmark::lua_function_name()]);
		Benchmark::lua_function_call(f);
	}
	lua_close(L);
}

void binding_object_set_get()
{
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;

		open(L);
		module(L)
			[
				class_<Benchmark::SetGet>("SetGet")
				.def(constructor<>())
			.def("set", &Benchmark::SetGet::set)
			.def("get", &Benchmark::SetGet::get)
			];
		luaL_dostring(L, "getset = SetGet()");
		luaL_dostring(L, Benchmark::object_set_get_lua_code());
	}
	lua_close(L);
}


void binding_returning_object()
{
	using namespace Benchmark::returning_class_object;
	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		using namespace luabind;
		open(L);
		module(L)
			[
				class_<ReturnObject>("ReturnObject")
				.def(constructor<>())
			.def("set", &ReturnObject::set)
			.def("get", &ReturnObject::get),


			def("object_function", &object_function),
			def("object_compare", &object_compare)
			];
		luaL_dostring(L, lua_code());
	}
	lua_close(L);
}
