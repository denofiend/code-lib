/**
 * New node file
 */
var log = require("./logger");
var config = require('./config');
var redisModule = require("redis");
var async = require("async");
var crypto = require('crypto');


var redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true});
redis.on("error", function (err) {
    log.logger.error("redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});


function uniResponse(res,bodyObj){
	var bodyStr = JSON.stringify(bodyObj);
	res.writeHead(200, {"Content-Type": "application/json;charset=UTF-8",
       "connection": "close"});
	res.write(bodyStr);
	res.end();
	log.logger.info("response body:" + bodyStr);
};

function responseOk(res){
    var timestamp = Date.parse(new Date())/1000;
	var bodyObj = {"code":0,"desc": "ok","time":timestamp};
	uniResponse(res,bodyObj);
};

function responseQueryOk(res,dataObj){
    var timestamp = Date.parse(new Date())/1000;
    var bodyObj = {"code":0,"desc": "ok","time":timestamp,"data":dataObj};
    uniResponse(res,bodyObj);
};

function responseProtocolErr(res){
    var timestamp = Date.parse(new Date())/1000;
    var bodyObj = {"code":1,"desc": "protocol error","time":timestamp};
	uniResponse(res,bodyObj);
};

function responseSystemErr(res){
    var timestamp = Date.parse(new Date())/1000;
	var bodyObj = {"code":2,"desc": "system internal error","time":timestamp};
	uniResponse(res,bodyObj);
};

function responseQueryNotFound(res){
    var timestamp = Date.parse(new Date())/1000;
    var bodyObj = {"code":3,"desc": "not found","time":timestamp};
    uniResponse(res,bodyObj);
};

function responseVerifyErr(res){
    var timestamp = Date.parse(new Date())/1000;
    var bodyObj = {"code":4,"desc": "verify failed","time":timestamp};
	uniResponse(res,bodyObj);
};


 
var responseQuery = function (req,res){
    log.logger.trace("query service body:" + req.body);
    var reqObj = JSON.parse(req.body);

    if(!reqObj.web){
        log.logger.error("'web' field not defined in json body");
        responseprotocolErr(res);
        return;
    }
    var hashUrl = getSha1(reqObj.web);

    redis.hget("dlrecmd:" + hashUrl + ":page","totalScore",function(err,totalScore){
        if(err){
            log.logger.error("get page info error from redis");
            responseSystemErr(res);
            return;
        }
        if(!totalScore){
        	log.logger.trace("totalScore not find in dlrecmd:page:" + hashUrl);
            responseQueryNotFound(res);
            return;
        }
        redis.zrange('dlrecmd:' + hashUrl + ":pagesources",-1,-1,'withscores',function(err,result){
            if(err){
                log.logger.error("get most probable source error from redis");
                responseSystemErr(res);
                return;
            }
            if(result.length === 0){
            	log.logger.trace("not find in dlrecmd:pagesources:" + hashUrl);
                responseQueryNotFound(res);
                return;
            }
            log.logger.trace("dlrecmd:pagesources:" + hashUrl + ":" + JSON.stringify(result));
            var sourceHash =  result[0];
            var sourceScore = result[1];
            var proportion = Math.floor(sourceScore*100/totalScore);
            if(proportion >= config.proportionThreshold  && sourceScore >= config.timesThreshold){
                redis.zcard("dlrecmd:" + sourceHash + ":sourceurls",function(err,memsTotal){
                    if(err){
                        log.logger.error("get source memsTotal error from redis");
                        responseSystemErr(res);
                        return;
                    }
                    if(!memsTotal){
                    	log.logger.trace("memsTotal not find in dlrecmd:sourceurls:" + sourceHash);
                        responseQueryNotFound(res);
                        return;
                    }
                    log.logger.trace("memsTotal:" + memsTotal);
                    var index = (Date.parse(new Date())/1000) % memsTotal;
                    redis.zrange("dlrecmd:" + sourceHash + ":sourceurls",index,index,function(err,urls){
                        if(err){
                            log.logger.error("get source url error from redis");
                            responseSystemErr(res);
                            return;
                        }
                        if(urls.length === 0){
                        	log.logger.trace("urls not find in dlrecmd:sourceurls::" + sourceHash + ",index" + index );
                            responseQueryNotFound(res);
                            return;
                        }
                        redis.hgetall("dlrecmd:" + sourceHash + ":source",function(err,obj){
                            if(err){
                                log.logger.error("get source info error from redis");
                                responseSystemErr(res);
                                return;
                            }
                            if(!obj){
                            	log.logger.trace("source info not find in dlrecmd:source:" + sourceHash);
                                responseQueryNotFound(res);
                                return;
                            }
                            var dataObj = {};
                            dataObj.source_url = urls[0];
                            dataObj.sname = obj.name;
                            dataObj.ssize = obj.size;
                            dataObj.shash = sourceHash;
                            responseQueryOk(res,dataObj);
                            return;
                        })
                    });
                });
                return;
            }
            log.logger.trace("no source satisfy the conditions,s:" + sourceScore + ",p:" + proportion);
            responseQueryNotFound(res);
            return;

        });
    });


}


var responseInsert = function (req,res){
    log.logger.trace("insert service body:" + req.body);

    if(!req.body){
        log.logger.error("insert service body is null");
        responseProtocolErr(res);
        return;
    }

    var bodyObj = JSON.parse(req.body);
    if(!bodyObj.version || !bodyObj.surl|| !bodyObj.sname|| !bodyObj.shash || !bodyObj.ssize
    ||!bodyObj.verify || !bodyObj.relate_web) {
        log.logger.error("some memmbers are not defined in json");
        responseProtocolErr(res);
        return;
    }
	
	var hashStr = getSha1(JSON.stringify(bodyObj.relate_web) + config.verifyKey);
	if(hashStr !== bodyObj.verify){
        log.logger.error("verify failed");
        responseVerifyErr(res);
        return;		
	}
    

    var slot = (Date.parse(new Date())/1000) % config.userDownLoadListsNum;
    var key = 'dlrecmd:' + slot + ':userdl';
    redis.rpush(key,req.body,function(err){
        if(err){
            log.logger.error("redis lpush error in responseInsert");
            responseSystemErr(res)
        }else{
            responseOk(res);
        }
    });
}




var responseCorrect = function (req,res){
    log.logger.trace("correct service body:" + req.body);
    if(!req.body){
        log.logger.error("correct service body is null");
        responseProtocolErr(res);
        return;
    }


    var bodyObj = JSON.parse(req.body);
    if(!bodyObj.version || !bodyObj.web|| !bodyObj.rs_url|| !bodyObj.rs_hash
        || !bodyObj.rs_size|| !bodyObj.errcode) {
        log.logger.error("some memmbers are not defined in json");
        responseProtocolErr(res);
        return;
    }

    var slot = (Date.parse(new Date())/1000) % config.userDownLoadFailListsNum;
    var key = 'dlrecmd:' + slot + ':faildl';
    redis.rpush(key,req.body,function(err){
        if(err){
            log.logger.error("redis lpush error in responseCorrect");
            responseSystemErr(res)
        }else{
            responseOk(res);
        }
    });
}


var getSha1 = function(sourceStr){
    var shasum = crypto.createHash('sha1');
    var buf = new Buffer(sourceStr,'utf8');
    shasum.update(buf);
    hashStr = shasum.digest('hex'); 
    return hashStr;
}

exports.responseInsert = responseInsert;
exports.responseQuery = responseQuery;
exports.responseCorrect = responseCorrect;




 

/*
var responseQuery = function (req,res){
    log.logger.trace("query service body:" + req.body);
    var reqObj = JSON.parse(req.body);

    if(!reqObj.web){
        log.logger.error("'web' field not defined in json body");
        responseprotocolErr(res);
        return;
    }
    var hashUrl = getSha1(reqObj.web);
    
    redis.multi()
		.hget("dlrecmd:" + hashUrl + ":page","totalScore")
		.zrange('dlrecmd:' + hashUrl + ":pagesources",-1,-1,'withscores')
		.exec(function(err,replies){
        	if(err){
            	log.logger.error("get totalScore and max score source error from redis:" + reqObj.web);
            	responseSystemErr(res);
            	return;
        	}
        	if(!replies[0] || !replies[1] || replies[1].length === 0){
        		log.logger.trace("totalScore or pagesources not found:" + reqObj.web + ",sha1:" + hashUrl);
            	responseQueryNotFound(res);
            	return;        		
        	}
            log.logger.trace( hashUrl + ":" + JSON.stringify(replies));
            
            var sourceHash =  replies[1][0];
            var sourceScore = replies[1][1];
            var totalScore = replies[0];
            var proportion = Math.floor(sourceScore*100/totalScore);
            if(proportion >= config.proportionThreshold  && sourceScore >= config.timesThreshold){
            	redis.multi()
            		.zrange("dlrecmd:" + sourceHash + ":sourceurls",0,-1)	
            		.hgetall("dlrecmd:" + sourceHash + ":source")
            		.exec(function(err,replies){
                    	if(err){
                        	log.logger.error("get source info error from redis");
                        	responseSystemErr(res);
                        	return;
                    	} 
        				if(!replies[0] || !replies[1] ||replies[0].length === 0){
        					log.logger.trace("source info not found,sourceHash:" + sourceHash);
            				responseQueryNotFound(res);
            				return;        		
        				}
        				var memsTotal = replies[0].length;        	           			
            			var index = (Date.parse(new Date())/1000) % memsTotal;
            			var url = replies[0][index];
            			var sourceInfo = replies[1];
            			
                        var dataObj = {};
                        dataObj.source_url = url;
                        dataObj.sname = sourceInfo.name;
                        dataObj.ssize = sourceInfo.size;
                        dataObj.shash = sourceHash;
                        responseQueryOk(res,dataObj);
                        return;            			            			
            		});            	            	
            }else{
            	log.logger.trace("no source satisfy the conditions,s:" + sourceScore + ",p:" + proportion);
            	responseQueryNotFound(res);            
            }
            return;      	
        	        					
		});
}
*/
