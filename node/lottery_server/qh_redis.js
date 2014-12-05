/**
 * New node file
 */
var log = require("./logger");
var config = require('./config');
var redisModule = require("redis");
var crypto = require('crypto');


var redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true, return_buffers: true });

redis.on("error", function (err) {
        log.logger.error("[fatalerror] redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});

function hset_redis_callback(key, hash_key, hash_val, callback){
    hash_key = hash_key.toLowerCase();
    redis.hset(key, hash_key, hash_val, function(err){
        if (err) {
            callback("redis hset error:" + err.toString(), null);
        } else {
            callback(null, "redis hset success: key(" + key + "),hash_key(" + hash_key + ")");
        }
    });
}


function redis_del_key_callback(key, callback) {
    redis.del(key, function(err) {
        if (err) {
            callback("redis del key error: " + err.toString(), null);
        } else {
            callback(null, "redis del key  success:(" + key + ")");
        }
    });
}

function  redis_del_key(key) {
    redis.del(key, function(err) {
        if (err) {
            log.logger.error("[fatalerror] redis del key error: " + err.toString());
        } else {
            log.logger.info("redis del key  success:(" + key + ")");
        }
    });
}

function redis_set(key, value, callback) {
    redis.set(key, value, function(err){
        if (err) {
            callback("redis set error: " + err.toString(), null);
        } else {
            callback(null, "redis set success:(" + key + ")");
        }
    });
}

function redis_get(key, callback) {
    redis.get(key, function(err, value){
        if (err) {
            callback("redis get error: " + err.toString(), null);
        } else {
            callback(err, value);
        }
    });
}

function redis_incrby(key, num, callback) {
    redis.incrby(key, num, function(err, value){
        if (err) {
            callback("redis get error: " + err.toString(), null);
        } else {
            callback(err, value);
        }
    });
}
function redis_incr(key, callback) {
    redis.incr(key, function(err, value){
        if (err) {
            callback("redis get error: " + err.toString(), null);
        } else {
            callback(err, value);
        }
    });
}

function hset_redis(key, hash_key, hash_val){
    hash_key = hash_key.toLowerCase();
    redis.hset(key, hash_key, hash_val, function(err){
        if (err) {
            log.logger.error("[fatalerror] redis hset error:" + err.toString());
        } else {
            log.logger.info("redis hset success (" + key + "," + hash_key + "," + hash_val + ")");
        }
    });
}

exports.redis_get = redis_get;
exports.redis_incrby = redis_incrby;
exports.redis_incr = redis_incr;