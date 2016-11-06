#include "benchmark_runner.hpp"

//benchmark types
#include "global_table.hpp"
#include "table_chain.hpp"
#include "c_function_call.hpp"
#include "lua_function_call.hpp"
#include "object_member_call.hpp"
#include "return_class_object.hpp"
#include "std_random_bind.hpp"

#include <emscripten/bind.h>

std::string test()
{
  return "test_function";
}

EMSCRIPTEN_BINDINGS(lua_benchmark) {
  using namespace benchmark;
    using namespace emscripten;
  class_<BenchmarkRunner>("Benchmark")
  .class_function("execute", &BenchmarkRunner::one_shot_s)
  .class_function("title", &BenchmarkRunner::title_s)
  ;
  function("test",&test);
  }
