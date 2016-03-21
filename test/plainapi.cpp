
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
	lua_close(state);
}

struct FunctionWrap
{
	lua_State *state_;
	int ref_;

	FunctionWrap(lua_State *state, int ref) :state_(state), ref_(ref)
	{
	}

	std::string operator()(const std::string& v)
	{
		lua_rawgeti(state_, LUA_REGISTRYINDEX, ref_);
		lua_pushstring(state_,v.c_str());
		lua_pcall(state_, 1, 1, 0);
		std::string result{lua_tostring(state_,-1)};
		lua_settop(state_,0);
		return result;
	}
};

void binding_lua_function_call()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	luaL_dostring(state, Benchmark::register_lua_function_lua_code());

	lua_getglobal(state, Benchmark::lua_function_name());
	FunctionWrap f(state, luaL_ref(state, LUA_REGISTRYINDEX));
	Benchmark::lua_function_call(f);

	lua_close(state);
}



//not available luaL_setmetatable and luaL_setfuncs at Lua5.1
void setmetatable(lua_State *L, const char *tname) {
	luaL_getmetatable(L, tname);
	lua_setmetatable(L, -2);
}
void setfuncs(lua_State *L, const luaL_Reg *l) {
	for (; l->name != 0; l++) {
		lua_pushcclosure(L, l->func, 0);
		lua_setfield(L, -2, l->name);
	}
}

int setget_new(lua_State* L)
{
	void* ptr = lua_newuserdata(L, sizeof(Benchmark::SetGet));
	new(ptr) Benchmark::SetGet();
	setmetatable(L, "SetGet");
	return 1;
}
int setget_set(lua_State* L)
{
	Benchmark::SetGet* setget = static_cast<Benchmark::SetGet*>(luaL_checkudata(L, 1, "SetGet"));
	setget->set(lua_tonumber(L,2));
	return 0;
}
int setget_get(lua_State* L)
{
	Benchmark::SetGet* setget = static_cast<Benchmark::SetGet*>(luaL_checkudata(L, 1, "SetGet"));
	lua_pushnumber(L, setget->get());
	return 1;
}

void binding_object_set_get()
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);

	luaL_newmetatable(state,"SetGet");
	luaL_Reg funcs[] = 
	{
		{ "new",setget_new },
		{ 0 ,0 },
	};
	setfuncs(state, funcs);
	lua_newtable(state);
	luaL_Reg indexfuncs[] =
	{
		{ "set",setget_set },
		{ "get",setget_get },
		{ 0 ,0 },
	};
	setfuncs(state, indexfuncs);
	lua_setfield(state,-2,"__index");


	lua_setglobal(state, "SetGet");

	luaL_dostring(state,"getset = SetGet.new()");
	luaL_dostring(state,Benchmark::object_set_get_lua_code());

	lua_close(state);
}
