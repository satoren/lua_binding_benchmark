
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

struct LuaFunctionWrap {
	LuaFunction<std::string(std::string)> fx;
	LuaFunctionWrap(LuaTable table) : fx(table.Get<LuaFunction<std::string(std::string)>>(Benchmark::lua_function_name())) {
	}

	std::string operator() (std::string x) {
		return fx.Invoke(x);
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
	Lua lua;
	LuaTable global = lua.GetGlobalEnvironment();
	LuaFunction<std::string(std::string)> fx = global.Get<LuaFunction<std::string(std::string)>>(Benchmark::lua_function_name());
	lua.RunScript(Benchmark::register_lua_function_lua_code());
	LuaFunctionWrap f(global);
	Benchmark::lua_function_call(f);
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

struct object_function_wrap
{
		Lua& lua;
		object_function_wrap(Lua& l):lua(l){}
	LuaUserdata<Benchmark::returning_class_object::ReturnObject> operator()()
	{
		using namespace Benchmark::returning_class_object;
		auto retobj = lua.CreateUserdata<ReturnObject>(new ReturnObject(object_function()));
		retobj.Bind("set", &ReturnObject::set);
		retobj.Bind("get", &ReturnObject::get);
   return retobj;
	}
};

struct object_compare_wrap
{
	bool operator()(LuaUserdata<Benchmark::returning_class_object::ReturnObject> userdata)
	{
			return object_compare(*userdata.GetPointer());
	}
};

void binding_returning_object()
{
	using namespace Benchmark::returning_class_object;

		Lua lua;
		LuaTable global = lua.GetGlobalEnvironment();
		auto object_f = lua.CreateFunction<LuaUserdata<ReturnObject>()>(object_function_wrap(lua));
		global.Set("object_function", object_f);

  	auto object_compare_f = lua.CreateFunction<bool(LuaUserdata<ReturnObject>)>(object_compare_wrap());
		global.Set("object_compare", object_compare_f);
		lua.RunScript(lua_code());

}
