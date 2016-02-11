# lua_binding_benchmark

work in progress...

http://satoren.github.io/lua_binding_benchmark/
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

* native function
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
	local times = 1000000
	for i=1,times do
	local r = native_function(i)
	if(r ~= i)then
	error('error')
	end
	end
```


* native function
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
local times = 1000000
for i=1,times do
getset:set(i)
if(getset:get() ~= i)then
error('error')
end
end
```
