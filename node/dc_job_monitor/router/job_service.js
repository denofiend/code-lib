
var config = require('../bon/config');
var redisModule = require("redis");
var EventEmitter = require('events').EventEmitter;
var emitter = new EventEmitter;
var async = require("async");
var zlib = require('zlib');

var redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true, return_buffers: true });

redis.on("error", function (err) {
	    log.logger.error("redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});


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

function get_done_range_from_redis_v2(key, redis_event) {
	console.log("get_don_range: key(" + key + ")");
	redis.zrange(key, 0, -1, function(err, ids) {
		if (err) {
			console.info(err.toString());
			return;
		}
		console.log("ids.length:" + ids.length);
		emitter.emit(redis_event, ids);
	});
}

exports.apply = function(server){
  server.get('/job_service',function(req,res){
    res.render('job_service');
  });

  server.get('/job_service/v1/base', function(req, res) {
	  console.log('receive get base info: '+req.socket.remoteAddress);
	  res.setHeader('content-type','application/json');
	  var source;
	  try{
		  source = req.param('source');
	  }catch(error){
		  console.info(error.toString());
		  source = false;
	  }
	  if(!source){
		  console.info('no source parameter!');
		  return response(res,'invalid parameter');
	  }

	  var idskey = config.syncIndexRedisKey + source;
	  console.log("idskey(" + idskey + ")");

	  redis.zrange(idskey, 0, -1, function(err, ids) {

		  var unzip_records = [];
		  async.map(ids, function(id, callback) {

			  var key = config.syncBaseInfoRedisKey;
			  var hash_key = source + ":" + id;
			  console.log("key(" + key + "),hash_key(" + hash_key + ")");

			  redis.hget(key, hash_key, function(err, record){
				  //console.log("record:" + record);

				  zlib.unzip(record, function(err, buffer) {

					  console.log("buffer(" + buffer.toString() + ")");
					  if (!err) {
						  unzip_records.push(JSON.parse(buffer.toString()));
					  }
					  callback(err, buffer);
				  });

			  });
		  }, function(err, results) {
			  if (err) {
				  console.log(err.toString());
			  } else {
				  console.log("results:" + results);
				  return response(res, null, unzip_records);
			  }
		  });
	  });
  });
  server.get('/job_service/v1/ids',function(req,res){
    console.log('receive get ids: '+req.socket.remoteAddress);
    res.setHeader('content-type','application/json');
	var source;
    try{
	  source = req.param('source');
    }catch(error){
		console.info(error.toString());
      	source = false;
    }
    if(!source){
      console.info('no source parameter!');
      return response(res,'invalid parameter');
    }
	var key = config.syncIndexRedisKey + source;

	get_done_range_from_redis_v2(key, 'redis_event');

	emitter.on('redis_event', function(ids){ 
		console.log("redis_event ids:" + ids);
        response(res,null, ids);
	});

  });
};

