#include "../benchmark.hpp"

void binding_begin()
{
	Benchmark::title_only = true;
	Benchmark::out.close();
	Benchmark::out.open(std::string("result.csv"), std::ios::out | std::ios::binary);
}
void binding_end()
{

}


const char* binding_name()
{
	return "Library + " LUA_VERSION;
}
void binding_global_table()
{
}

void binding_table_chain()
{
}

void binding_native_function_call()
{
}
void binding_lua_function_call()
{
}


void binding_object_set_get()
{
}

void binding_returning_object()
{
}
