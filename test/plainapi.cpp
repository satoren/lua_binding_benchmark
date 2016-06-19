extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "benchmark/benchmark.hpp"

BENCHMARK_DEFINE_LIBRARY_NAME("plain api")

inline void stackDump(lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

		case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;
		case LUA_TUSERDATA:
			if (luaL_getmetafield(L, i, "__name") == LUA_TSTRING)
			{
				printf("userdata:%s", lua_tostring(L, -1));
				lua_pop(L, 1);
				break;
			}
		default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
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
		lua_getglobal(state_, current_key);
		int v = static_cast<int>(lua_tointeger(state_, -1));
		lua_settop(state_, 0);
		current_key = 0;
		return v;
	}
};


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
		int v = static_cast<int>(lua_tointeger(state_, -1));
		lua_settop(state_, 0);
		current_key = 0;
		return v;
	}
};

typedef int bind_function_type(int);
template<bind_function_type F>
int native_function_binding(lua_State* L)
{
	int arg = static_cast<int>(lua_tointeger(L, 1));
	int result = F(arg);
	lua_pushinteger(L, result);
	return 1;
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




GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		GlobalTable t(state);
		benchmark_exec(t);
	}
	lua_close(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		luaL_dostring(state, "t1={t2={t3={}}}");
		TableChain t(state);
		benchmark_exec(t);
	}
	lua_close(state);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		lua_pushcclosure(state, native_function_binding<native_function>, 0);
		lua_setglobal(state, "native_function");
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
		lua_getglobal(state, lua_function_name);


		int ref = luaL_ref(state, LUA_REGISTRYINDEX);
		auto fn = [&](const std::string& s) {
			lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
			lua_pushstring(state, s.c_str());
			lua_pcall(state, 1, 1, 0);
			std::string result{ lua_tostring(state,-1) };
			lua_settop(state, 0);
			return result;
		};
		benchmark_exec(fn);
	}
	lua_close(state);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		auto newf = [](lua_State* L) {
			void* ptr = lua_newuserdata(L, sizeof(TestClass));
			new(ptr) TestClass();
			setmetatable(L, "TestClass");
			return 1;
		};
		auto setf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			setget->set(lua_tonumber(L, 2));
			return 0;
		};
		auto getf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			lua_pushnumber(L, setget->get());
			return 1;
		};
		auto gcf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			setget->~TestClass();
			return 0;
		};

		luaL_newmetatable(state, "TestClass");
		luaL_Reg funcs[] =
		{
			{ "new", newf },
			{ "__gc",gcf },
			{ 0 ,0 },
		};
		setfuncs(state, funcs);
		lua_newtable(state);
		luaL_Reg indexfuncs[] =
		{
			{ "set",setf },
			{ "get",getf },
			{ 0 ,0 },
		};
		setfuncs(state, indexfuncs);
		lua_setfield(state, -2, "__index");


		lua_setglobal(state, "TestClass");
		luaL_dostring(state, "getset = TestClass.new()");
		luaL_dostring(state, lua_code);

	}
	lua_close(state);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	lua_State *state = luaL_newstate(); luaL_openlibs(state);
	{
		auto newf = [](lua_State* L) {
			void* ptr = lua_newuserdata(L, sizeof(TestClass));
			new(ptr) TestClass();
			setmetatable(L, "TestClass");
			return 1;
		};
		auto setf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			setget->set(lua_tonumber(L, 2));
			return 0;
		};
		auto getf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			lua_pushnumber(L, setget->get());
			return 1;
		};
		auto gcf = [](lua_State* L) {
			TestClass* setget = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			setget->~TestClass();
			return 0;
		};
		luaL_newmetatable(state, "TestClass");
		luaL_Reg funcs[] =
		{
			{ "new", newf },
			{ "__gc",gcf },
			{ 0 ,0 },
		};
		setfuncs(state, funcs);
		lua_newtable(state);
		luaL_Reg indexfuncs[] =
		{
			{ "set",setf },
			{ "get",getf },
			{ 0 ,0 },
		};
		setfuncs(state, indexfuncs);
		lua_setfield(state, -2, "__index");

		auto objf = [](lua_State* L)
		{
			void* ptr = lua_newuserdata(L, sizeof(TestClass));
			new(ptr) TestClass(object_function());
			setmetatable(L, "TestClass");
			return 1;
		};
		auto objcomp = [](lua_State*  L)
		{
			TestClass* obj = static_cast<TestClass*>(luaL_checkudata(L, 1, "TestClass"));
			bool result = object_compare(*obj);
			lua_pushboolean(L, result);
			return 1;
		};

		lua_pushcclosure(state, objf, 0);
		lua_setglobal(state, "object_function");
		lua_pushcclosure(state, objcomp, 0);
		lua_setglobal(state, "object_compare");

		luaL_dostring(state, lua_code);

	}
	lua_close(state);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END



STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
#define STD_MT19937_NAME "std.mt19937"
#define STD_UNI_INT_DIST_NAME "std.uniform_int_distribution"

	using std::mt19937;

	lua_State *L = luaL_newstate(); luaL_openlibs(L);
	{
		lua_newtable(L);//create random table
		int random_table_index = lua_gettop(L);

		//bind mt19937
		{
			auto newf = [](lua_State* L) {
				void* ptr = lua_newuserdata(L, sizeof(mt19937));
				new(ptr) mt19937(lua_tointeger(L, 1));
				setmetatable(L, STD_MT19937_NAME);
				return 1;
			};
			auto callf = [](lua_State* L) {
				mt19937* mtengine = static_cast<mt19937*>(luaL_checkudata(L, 1, STD_MT19937_NAME));
				lua_pushinteger(L, (*mtengine)());
				return 1;
			};
			auto gcf = [](lua_State* L) {
				mt19937* mtengine = static_cast<mt19937*>(luaL_checkudata(L, 1, STD_MT19937_NAME));
				mtengine->~mt19937();
				return 0;
			};

			luaL_newmetatable(L, STD_MT19937_NAME);
			luaL_Reg funcs[] =
			{
				{ "new", newf },
				{ "__gc",gcf },
				{ "__call",callf },
				{ 0 ,0 },
			};
			setfuncs(L, funcs);
			lua_newtable(L);
			luaL_Reg indexfuncs[] =
			{
				{ 0 ,0 },
			};
			setfuncs(L, indexfuncs);
			lua_setfield(L, -2, "__index");
			lua_setfield(L, random_table_index, "mt19937");
		}

		//bind  std::uniform_int_distribution<int>
		{
			typedef std::uniform_int_distribution<int> uni_int_dist;

			auto newf = [](lua_State* L) {
				void* ptr = lua_newuserdata(L, sizeof(uni_int_dist));
				new(ptr) uni_int_dist(luaL_optinteger(L, 1,0), luaL_optinteger(L,2, std::numeric_limits<int>::max()));
				setmetatable(L, STD_UNI_INT_DIST_NAME);
				return 1;
			};
			auto callf = [](lua_State* L) {
				uni_int_dist* dist = static_cast<uni_int_dist*>(luaL_checkudata(L, 1, STD_UNI_INT_DIST_NAME));
				mt19937* engine = static_cast<mt19937*>(luaL_checkudata(L, 2, STD_MT19937_NAME));
				lua_pushinteger(L, (*dist)(*engine));
				return 1;
			};
			auto gcf = [](lua_State* L) {
				uni_int_dist* dist = static_cast<uni_int_dist*>(luaL_checkudata(L, 1, STD_UNI_INT_DIST_NAME));
				dist->~uni_int_dist();
				return 0;
			};

			luaL_newmetatable(L, STD_UNI_INT_DIST_NAME);
			luaL_Reg funcs[] =
			{
				{ "new", newf },
				{ "__gc",gcf },
				{ "__call",callf },
				{ 0 ,0 },
			};
			setfuncs(L, funcs);
			lua_newtable(L);
			luaL_Reg indexfuncs[] =
			{
				{ 0 ,0 },
			};
			setfuncs(L, indexfuncs);
			lua_setfield(L, -2, "__index");
			lua_setfield(L, random_table_index, "uniform_int_distribution");
		}
		lua_setglobal(L,"random");

		int ret = luaL_dostring(L,lua_code);
		if (ret != 0) { printf("%s\n", lua_tostring(L, -1)); }
	}
	lua_close(L);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END

