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

function make_mx_response(res, mx_response){
	res.send({code:mx_reponse.code, message:mx_response.message});
}

function Mx_response(code, message){
	this.code = code;
	this.message = message;
}

var word_filter = function (req,res){
	log.logger.trace(">>> word_filter body:"+ req.body);

	var mx_response = new Mx_response(110, 'Tragedy, the word is illegal');

    if(!req.body){
        log.logger.error("insert service body is null");
		make_mx_response(res, new Mx_response(200, 'http request body is empty'));
        return;
    }

	var words = redis.zrange("nickname", 0, -1, function(err){
        if(err){
            log.logger.error("redis zrange error in word_filter");
			mx_response.code = 300;
			mx_response.message = "reids zrange error in word_filter";
			make_mx_response(res, new Mx_response(300, 'redis zrange error in word_filter'));
			return;
		}
	});

    var bodyObj = JSON.parse(req.body);
	var i = 0;

	for (i = 0; i < words.length; ++i) {
		var n = bodyObj.word.indexOf(words[i]);

		if (-1 != n) {
			break;
		}
	}

	if (words.length === i) {
		mx_response.code = 1;
		mx_response.message = 'Congratulations, this word is ok';
	}

	make_mx_response(res, mx_response);
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

exports.word_filter = word_filter;

 
