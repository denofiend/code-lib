var http = require('http'),
    https = require('https'),
    fs = require('fs'),
    redis = require('redis');

exports.http_request = function http_request(url,options,callback){
  var net = http;
  var host = null;
  var port = 80;

  var opts = {};
  var cb = callback;
  if(typeof(options)==='function'){
    cb = options;
  }else if(typeof(callback)==='function'){
    opts = options?options:opts;
    cb = callback;
  }

  if(typeof(url)!=='string'){
    return cb('invalid url');
  }

  try{
    var s1 = url.split('://');
    var s2 = s1.length===1?s1[0]:s1[1];
    if(s1.length>1&&s1[0]==='https'){
      net = https;
      port = 443;
    }
    var idx = s2.indexOf('/');
    var s3 = s2.substr(0,idx);
    var idx2 = s3.indexOf(':');
    if(idx2>0){
      host = s3.substr(0,idx2);
      port = parseInt(s3.substr(idx2+1));
    }else{
      host = s3;
    }
    var path = s2.substr(idx);
  }catch(error){
    return cb('invalid url: '+error);
  }
  var start_time = new Date().getTime();
  var params = {
    host: host,
    port: port,
    path: path,
    method: opts.hasOwnProperty('method')?opts.method:'GET',
    headers: opts.hasOwnProperty('headers')?opts.headers:{},
  };
  var req = net.request(params,function(res){
    res.setEncoding('binary');
    if(!res.headers.hasOwnProperty('content-length')
    || res.headers['content-length']<=0){
      return cb(null,{
        code: res.statusCode,
        headers: res.headers,
        body: null,
        time: (new Date().getTime()-start_time),
      });
    }
    var chunks = [];
    var totalsize = 0;
    res.on('data',function(chunk){
      chunks.push(new Buffer(chunk,'binary'));
      totalsize += chunk.length;
    });
    res.on('end',function(){
      var data = new Buffer(totalsize);
      var offset = 0;
      for(var i=0;i<chunks.length;++i){
        chunks[i].copy(data,offset,0,chunks[i].length);
        offset += chunks[i].length;
      }
      cb(null,{
        code: res.statusCode,
        headers: res.headers,
        body: data,
        time: (new Date().getTime()-start_time),
      });
    });
    res.on('error',function(error){
      cb(error);
    });
  });
  req.on('error',function(error){
    cb(error);
  });
  if(opts.hasOwnProperty('body')){
    req.write(opts.body);
  }
  req.end();
};

try{
  exports.conf = JSON.parse(fs.readFileSync(__dirname+'/conf.json'));
}catch(error){
  console.error(error);
  process.exit(-1);
}

var drc = exports.conf.redis['device-s'];
exports.device_redis = redis.createClient(drc.port,drc.host,drc.options);
var trc = exports.conf.redis['transfile-s'];
exports.transfile_redis = redis.createClient(trc.port,trc.host,trc.options);

