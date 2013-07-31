var bon = require('../bon');

function response(res,error,items){
  if(error){
    return res.end(JSON.stringify({error:error}));
  }else{
    return res.end(JSON.stringify({
      error: null,
      items: items
    }));
  }
}

exports.apply = function(server){
  server.get('/cloudservice',function(req,res){
    res.render('cloudservice');
  });

  server.get('/cloudservice/api/devices',function(req,res){
    console.log('receive get devices request from: '+req.socket.remoteAddress);
    res.setHeader('content-type','application/json');
    var userid = 0;
    try{
      userid = parseInt(req.param('user'));
    }catch(error){
      response(res,error);
    }
    bon.http_request(bon.conf.server['api-ps-push-s']+'/devices?user='+userid,{},function(error,result){
      if(error){
        console.error(error);
        response(res,error);
      }else{
        var onlines = [];
        console.log(result);
        if(result.code === 200){
          onlines = JSON.parse(result.body);
        }
        bon.device_redis.hgetall('device:'+userid,function(error,replies){
          if(error){
            console.error(error);
            response(res,error);
          }else{
            console.log(replies);
            var devices = [];
            for(var k in replies){
              var item = JSON.parse(replies[k]);
              item['online'] = (onlines.indexOf(k)>=0);
              devices.push(item);
            }
            response(res,null,devices);
          }
        });
      }
    });
  });

  server.get('/cloudservice/api/files',function(req,res){
    console.log('receive get files request from: '+req.socket.remoteAddress);
    res.setHeader('content-type','application/json');
    var user = req.param('user');
    var app = req.param('app');
    if(!user||!app){
      console.info('invalid parameter user:%s, app:%s',user,app);
      return response(res,'invalid parameters');
    }
    bon.http_request(bon.conf.server['cs-s']+'/filesync/ex/ls/?option=ts',{
      headers: {
        'X-Maxthon-FileSync-User': user,
        'X-Maxthon-FileSync-App': app,
      }
    },function(error,result){
      if(error){
        console.error(error);
        response(res,error);
      }else{
        var files = {items:[]};
        console.info(result);
        if(result.code===200 && result.body){
          files = JSON.parse(result.body);
        }
        response(res,null,files.items);
      }
    })
  });

  server.get('/cloudservice/api/messages',function(req,res){
    console.log('receive get messages request from: '+req.socket.remoteAddress);
    res.setHeader('content-type','application/json');
    var user = 0;
    try{
      user = parseInt(req.param('user'));
    }catch(error){
      user = false;
    }
    if(!user){
      console.info('no user parameter!');
      return response(res,'invalid parameter');
    }
    var k = new Buffer(11);
    k.write('dsnd:',0);
    k.writeInt32BE(user,5);
    k.write(':*',9);
    bon.transfile_redis.keys(k,function(error,keys){
      if(error){
        console.error(error);
        response(res,error);
      }else{
        console.log(keys);
        var count = keys.length;
        var index = 0;
        var messages = [];
        if(count===0){
          return response(res,null,messages);
        }else{
          for(var i=0;i<keys.length;++i){
            (function(k){
              var devicebuf = new Buffer(40);
              k.copy(devicebuf,0,10,50);
              var device = devicebuf.toString('utf8');
              bon.transfile_redis.hgetall(k,function(error,items){
                console.log(items);
                if(error){
                  console.error(error);
                }else{
                  for(var k in items){
                    var item = JSON.parse(items[k]);
                    item['to_device'] = device;
                    messages.push(item);
                  }
                }
                index++;
                if(index===count){
                  response(res,null,messages);
                }
              });
            })(keys[i]);
          }
        }//if count!=0
      }
    });
  });
};

