//using webworker
var loaded = false;
Module.monitorRunDependencies = function(left) {
     loaded=left==0;
 }


self.addEventListener('message', function(e) {
var exec = function(){
  if(loaded)
  {
    var title = Module.Benchmark.title()
    var ret = Module.Benchmark.execute(e.data)
    var res={}
    res[title] = ret;
    self.postMessage(res);
  }
  else {
    setTimeout(exec, 100);
  }
  }
  exec();
}, false);
