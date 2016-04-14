function split(str, pat)
   local t = {}
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

out = assert(io.open("library_info.csv","w"))
for submoduleline in io.popen("git submodule status"):lines() do
local hash,libpath,version = table.unpack(split(submoduleline," "))
version = string.match(version, '%((.*)%)') or version

for l in io.popen("cd "..libpath.." && git remote -vv"):lines() do
url = string.match(l, '	(.*) %(fetch%)') or url
end
libname = string.match(libpath, 'binding_libs/(.*)')
out:write(string.format('%s,%s,%s,%s\n', libname,version,hash,url))
end



