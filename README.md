# benchmark for lua binding library

work in progress...

http://satoren.github.io/lua_binding_benchmark/

If library has not supported feature,I wrote the wrap class.
It might not be an appropriate use for each library. please to PR if you know a better way.

It is also good with the addition of another library benchmark.

[![Build Status](https://travis-ci.org/satoren/lua_binding_benchmark.svg?branch=master)](https://travis-ci.org/satoren/lua_binding_benchmark)

##execute benchmark

```
git submodule update --init --recursive
```

#### on Linux
```
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make execute_benchmark
```

#### on Windows with MSVC++

Download lua source code and extract to root of working tree
```
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DLOCAL_LUA_DIRECTORY=luadirectoryname
```
open build/lua_binding_benchmark.sln and build 'execute_benchmark' project

##benchmark code
* global table
```C++
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			globaltable["value"] = i;
			int v = globaltable["value"];
			if (v != i) { throw std::logic_error(""); }
		}
```
* table chain
```C++
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			table["t1"]["value"] = i;
			int v = table["t1"]["value"];
			if (v != i) { throw std::logic_error(""); }
		}
```

* c function call
```C++
  // cfunction
	int native_function(int arg)
	{
		executed = true;
		return arg;
	}
```

```lua
  -- lua
	local times = 5000000
	for i=1,times do
	local r = native_function(i)
	if(r ~= i)then
	error('error')
	end
	end
```


* lua function call
```lua
lua_function=function(i)return i;end

```
```C++
		std::string teststring = "testtext";
		for (int i = 0; i < BENCHMARK_LOOP_COUNT; ++i)
		{
			std::string result = f(teststring);
			if (result != teststring) { throw std::logic_error(""); }
		}
```

* C++ object member call
```C++
//C++ class
	class SetGet
	{
	public:
		SetGet() :_mem(0.0) {}
		void set(double i)
		{
			_mem = i;
		}
		double get()const
		{
			return _mem;
		}
	private:
		double _mem;
	};
```

```lua
  -- lua
local times = 5000000
for i=1,times do
  getset:set(i)
  if(getset:get() ~= i)then
    error('error')
  end
end
```

* std random(C++11) bind
```lua
-- lua
local times = BENCHMARK_LOOP_COUNT
local rengine=random.mt19937.new(0)
local udist=random.uniform_int_distribution.new(1,6)
if(rengine() ~= 2357136044) then error('mt19937?') end
if(pcall(udist.__call,udist,udist) ~= false)then error('no error checked') end -- argument miss. error check
if(pcall(udist.__call,rengine,rengine) ~= false)then error('no error checked') end
for i=1,times do
	local value = udist:__call(rengine)--Actually below code, but __call metamethod has cost. for fair with "gen" function version.
--	local value = udist(rengine)
	if( value < 1 or value > 6)then\
		error('error')
	end
end
```
Or
```lua
local times = BENCHMARK_LOOP_COUNT
local rengine=random.mt19937(0)
local udist=random.uniform_int_distribution(1,6)
if(rengine:gen() ~= 2357136044) then error('mt19937?') end
if(pcall(udist.gen,udist,udist) ~= false)then error('no error checked') end\n"//ouch! argument miss. error ch
if(pcall(udist.gen,rengine,rengine) ~= false)then error('no error checked') end
for i=1,times do
	local value = udist:gen(rengine)
	if( value < 1 or value > 6)then
		error('error')
	end
end
```
