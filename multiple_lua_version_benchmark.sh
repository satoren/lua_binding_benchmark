build_test_versions=("lua-5.3.3" "lua-5.2.4" "lua-5.1.5")

build_and_exec_test(){
  if [ ! -e "build$1" ]; then
    mkdir "build$1"
  fi
  cd "build$1"
  cmake ../ -DCMAKE_BUILD_TYPE=Release -DLUA_SEARCH_LIB_NAME=$1 -DRESULT_OUTPUT_DIR="result/$1"
  make execute_benchmark
  cd ../
}

for luaversion in "${build_test_versions[@]}"
do
  if [ ! -e $luaversion ]; then
    if [ ! -e ${luaversion}.tar.gz ]; then
      curl https://www.lua.org/ftp/${luaversion}.tar.gz -o ${luaversion}.tar.gz
    fi
    tar zxf ${luaversion}.tar.gz
  fi
    build_and_exec_test $luaversion
done

test_versions=("luajit")
for luaversion in "${test_versions[@]}"
do
  build_and_exec_test $luaversion
done
