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


var word_filter_get = function (req,res){
	log.logger.trace(">>> word_filter_get body:"+ req.body);

    if(!req.body){
        log.logger.error("insert service body is null");
		make_mx_response(res, new Mx_response(200, 'http request body is empty'));
        return;
    }

    var bodyObj = JSON.parse(req.body);
	log.logger.debug("bodyOjb.word:"+bodyObj.word);

	redis.smembers("nickname", function(err, words){

		var mx_response = new Mx_response(110, 'Tragedy, the word is illegal');

        if(err){
            log.logger.error("redis smembers error in word_filter_get");

			mx_response.code = 300;
			mx_response.message = "reids smembers error in word_filter_get";

		}else{

			var i = 0;
			for (i = 0; i < words.length; ++i) {
				var n = bodyObj.word.indexOf(words[i]);

				if (-1 != n) {
					break;
				}
			}

			log.logger.debug("words.length:" + words.length + ", i:"+i);

			if ((words.length === undefined) || words.length === i) {
				log.logger.debug("words.length:" + words.length + ", i:"+i);
				mx_response.code = 1;
				mx_response.message = 'Congratulations, this word is ok';
			}
		}

		make_mx_response(res, mx_response);

	});

}

var word_filter_set = function (req,res){
	log.logger.trace(">>> word_filter_set body:"+ req.body);

	var mx_response = new Mx_response(1, 'set success');

    if(!req.body){
        log.logger.error("word_filter_set body is null");
		make_mx_response(res, new Mx_response(200, 'http request body is empty'));
        return;
    }

    var bodyObj = JSON.parse(req.body);

	var words = redis.sadd("nickname", bodyObj.word, function(err, ret){
        if(err || 0 === ret){
            log.logger.error("redis sadd error in word_filter_set");
			mx_response.code = 300;
			mx_response.message = "reids sadd error in word_filter_set";
			make_mx_response(res, mx_response);
			return;
		}
	});

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

exports.word_filter_get = word_filter_get;
exports.word_filter_set = word_filter_set;

 
