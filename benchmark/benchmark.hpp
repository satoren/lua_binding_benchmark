#pragma once

extern "C" {
#include <lua.h>
}

#include "benchmark_runner.hpp"

//benchmark types
#include "global_table.hpp"
#include "table_chain.hpp"
#include "c_function_call.hpp"
#include "lua_function_call.hpp"
#include "object_member_call.hpp"
#include "return_class_object.hpp"
#include "std_random_bind.hpp"

#if EMSCRIPTEN
#include "emscripten_bind.hpp"
#else
#include "console_csvout.hpp"
#endif
