#if LUA_VERSION_NUM > 502
#define lua_tounsigned lua_tointeger
#endif
#include "sol.hpp"

#include "benchmark/benchmark.hpp"

BENCHMARK_DEFINE_LIBRARY_NAME("sol")

struct TableWrap
{
	sol::state& state_;
	std::vector<const char*> keys_;
	TableWrap(sol::state& state) :state_(state) {}
	TableWrap& operator[](const char* key)
	{
		keys_.push_back(key);
		return *this;
	}

	int operator=(int value)
	{
		if (keys_.empty())
		{
			return 0;
		}
		if (keys_.size() == 1)
		{
			state_.set(keys_.front(), value);
			return value;
		}
		sol::table tbl = state_[keys_.front()];
		for (size_t i = 1; i<keys_.size() - 1; ++i)
		{
			tbl = tbl[keys_[i]];
		}
		tbl.set(keys_.back(), value);
		keys_.clear();
		return value;
	}
	operator int()
	{
		if (keys_.empty())
		{
			return 0;
		}
		if (keys_.size() == 1)
		{
			return state_[keys_.front()];
		}
		sol::table tbl= state_[keys_.front()];
		for (size_t i = 1; i<keys_.size() - 1; ++i)
		{
			tbl = tbl[keys_[i]];
		}
		int v = tbl.get<int>(keys_.back());
		keys_.clear();
		return v;
	}
};
struct FunctionWrap
{
	sol::function f_;
	FunctionWrap(sol::function f) :f_(f)
	{
	}

	std::string operator()(const std::string& v)
	{
		return f_.call<std::string>(v);
	}
};


GLOBAL_TABLE_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	benchmark_exec(state);
}
GLOBAL_TABLE_BENCHMARK_FUNCTION_END

TABLE_CHAIN_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.script("t1={t2={t3={}}}");
	TableWrap table(state);
	benchmark_exec(table);
}
TABLE_CHAIN_BENCHMARK_FUNCTION_END

C_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.set_function("native_function", native_function);
	state.script(lua_code);
}
C_FUNCTION_CALL_BENCHMARK_FUNCTION_END

LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.script(register_lua_function_code);

	sol::function f = state[lua_function_name];
	FunctionWrap fwrap(f);
	benchmark_exec(fwrap);
}
LUA_FUNCTION_CALL_BENCHMARK_FUNCTION_END

OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.new_usertype<TestClass>("TestClass", "set", &TestClass::set, "get", &TestClass::get);
	state.script("getset = TestClass.new()");
	state.script(lua_code);
}
OBJECT_MEMBER_CALL_BENCHMARK_FUNCTION_END

RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_BEGIN
{
	sol::state state;
	state.new_usertype<TestClass>("TestClass",
		"set", &TestClass::set,
		"get", &TestClass::get
		);
	state["object_function"] = &object_function;
	state["object_compare"] = &object_compare;
	state.script(lua_code);
}
RETURN_CLASS_OBJECT_BENCHMARK_FUNCTION_END

STD_RANDOM_BIND_BENCHMARK_FUNCTION_BEGIN
{
	typedef std::uniform_int_distribution<int> uni_int_dist;
	
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	
	lua.script("random={}");
	sol::table random = lua["random"];
	
	sol::usertype<std::mt19937> mtut(
		sol::constructors<sol::types<int>>(),
		"__call", &std::mt19937::operator()
	);
	random.set_usertype("mt19937", mtut);
	
	sol::usertype<uni_int_dist> distmt(
		sol::constructors<sol::types<int, int>>(),
		"__call", [](uni_int_dist& dist, std::mt19937& gen) {return dist(gen); }
	);
	random.set_usertype( "uniform_int_distribution", distmt );
	
	lua.script(lua_code);
}
STD_RANDOM_BIND_BENCHMARK_FUNCTION_END

