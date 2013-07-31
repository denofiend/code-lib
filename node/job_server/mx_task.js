/**
 * New node file
 */
var log = require("./logger");
var config = require('./config');
var mx_mysql = require('./mx_mysql'); 
var redisModule = require("redis");
var async = require("async");
var zlib = require('zlib');

var redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true, return_buffers: true });

redis.on("error", function (err) {
	    log.logger.error("redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});

function MIN_ID_HASH_KEY(source){
	return source + ":min_id";
}

function MAX_ID_HASH_KEY(source){
	return source + ":max_id";
}

function MxArray(min_id, max_id) {
	var ids = new Array();
	for (var id = min_id; id <= max_id; ++id) {
		ids.push(id);
	}
	return ids;
}

function hset_redis_callback(key, hash_key, hash_val, callback){
	log.logger.trace(">>> hset_redis_callback");

	async.series([
			function(callback){
				redis.hset(key, hash_key, hash_val, function(err){
					if (err) {
						callback("redis hset error:" + err.toString(), null);
					} else {
						callback(null, "redis hset success: key(" + key + "),hash_key(" + hash_key + ")");
					}
				});
			},
			function(callback){
				redis.expire(key, config.keyTimeOut, function(err){
					log.logger.debug("redis.expire, key(" + key + ")");
					if(err){
						callback("redis set timeout 30 days error:" + err.toString(), null);
					}else{
						callback(null, "redis hset success with timeout 30 days:(" + key + "," + hash_key + ")"); 
					}
				});
			}],
			function(err, results){
				callback(err, results);
			});

}

function hset_redis(key, hash_key, hash_val){
	log.logger.trace(">>> hset_redis");

	async.series([
			function(callback){
				redis.hset(key, hash_key, hash_val, function(err){
					if (err) {
						callback("redis hset error:" + err.toString(), null);
					} else {
						callback(null, "redis hset success: key(" + key + "),hash_key(" + hash_key + ")");
					}
				});
			},
			function(callback){
				redis.expire(key, config.keyTimeOut, function(err){
					log.logger.debug("redis.expire, key(" + key + ")");
					if(err){
						callback("redis set timeout 30 days error:" + err.toString(), null);
					}else{
						callback(null, "redis hset success with timeout 30 days:(" + key + "," + hash_key + "," + hash_val + ")"); 
					}
				});
			}],
			function(err, results){
				if(err){
					log.logger.error("redis hset error:" + err.toString());
				}else{
					log.logger.info("redis hset success with timeout 30 days:(" + key + "," + hash_key + "," + hash_val + ")");
				}
			});

}

var lang_adapter_task = function (){
	log.logger.trace(">>> lang_adapter_task");

	mx_mysql.mysql_select(config.langSelectSql, config.langMysqlSelectEvent);

	mx_mysql.emitter.on(config.langMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {
			
			log.logger.debug("records[i].langad:" + records[i].langad);
			log.logger.debug("records[i].langios:" + records[i].langios);
			hset_redis(config.langKey, records[i].langios, records[i].langad);

		}
	});

}

var country_adapter_task = function (){
	log.logger.trace(">>> country_adapter_task");

	mx_mysql.mysql_select(config.countrySelectSql, config.countryMysqlSelectEvent);

	mx_mysql.emitter.on(config.countryMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[i].countryad:" + records[i].countryad);
			log.logger.debug("records[i].countryios:" + records[i].countryios);

			hset_redis(config.countryKey, records[i].countryios, records[i].countryad);

		}
	});

}

var channle_list_task = function (){
	log.logger.trace(">>> channel_task");

	mx_mysql.mysql_select(config.channlelistSelectSql, config.channlelistMysqlSelectEvent);

	mx_mysql.emitter.on(config.channlelistMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[i].lang:" + records[i].lang);
			log.logger.debug("records[i].country:" + records[i].country);
			log.logger.debug("records[i].sourcetype:" + records[i].sourcetype);
			log.logger.debug("records[i].channleid:" + records[i].channleid);
			log.logger.debug("records[i].version:" + records[i].version);
			log.logger.debug("records[i].updatetime:" + records[i].updatetime);
			log.logger.debug("records[i].channlelist:" + records[i].channlelist);
			log.logger.debug("records[i].channlelist_version:" + records[i].channlelistversion);
			log.logger.debug("records[i].ifdefault:" + records[i].ifdefault);

			var hash_key = records[i].lang + ":" + records[i].country + ":" + records[i].sourcetype + ":" + records[i].channleid + ":" + records[i].version;
			/* if is default channle list */
			if (1 === records[i].isdefault) {
				hash_key = "default_channle_list";
			}

			hset_redis(config.channlelistKey, hash_key, records[i].channlelist);

		}
	});

}

function get_status_of_task(source, id, callback) {
	/* get status from the db */
	var tmp_sql = config.syncDoneJobSelectSql.replace('?', source);
	var sql = tmp_sql.replace('?', id);
	var eventName = config.syncDoneJobSelectMysqlSelectEvent + ":" + source + ":" + id;

	mx_mysql.mysql_select(sql, eventName);

	mx_mysql.emitter.on(eventName, function(records){
		log.logger.debug("mysql here is right");

		if (0 === records.length) {
			callback(null, "not found record in mysql for source(" + source + "),id(" + id +")");

		} else if (1 != records.length) {
			callback("records's length is not 1", null);
		} else {
			var status = records[0].status;
			log.logger.debug("status is :" + status);

			if (1 === status) {
				callback(null, 'status is 1, just go');
			}
			else {
				callback('status is not 1', null);
			}
		}
	});
}

function do_img_sync_task(source, id, callback){
	log.logger.debug(id + " job's status is 1, let's go to set img to memcache!");
	// get the image of the job
	var img_sql = config.syncImgSelectSql.replace('?', source);
	img_sql = img_sql.replace('?', id);

	var img_event_name = config.syncImgSelectMysqlSelectEvent + source + id;
	mx_mysql.mysql_select(img_sql, img_event_name);

	mx_mysql.emitter.on(img_event_name, function(records){ 

		log.logger.trace("img event name length:" + records.length);
		async.map(records, 
			function(record, callback){
				var img_key = config.syncImgKey + source + ":" + id + ":" + record.imgsize;
				var img_value = record.imgurl;

				log.logger.debug("img_key(" + img_key + ",img_value(" + img_value + ")");

				async.series([
					function(callback){
						redis.set(img_key, img_value, function(err){
							if (err) {
								callback("redis set error: " + err.toString(), null);
							} else {
								callback(null, "redis set success:(" + img_key + "," + img_value);
							}
						});
					},
					function(callback){
						redis.expire(img_key, config.keyTimeOut, function(err){
							log.logger.debug("redis.expire, key(" + img_key + ")");
							if(err){
								callback("redis set timeout 30 days error:" + err.toString(), null);
							}else{
								callback(null, "redis set success with timeout 30 days:(" + img_key + "," + img_value);
							}
						});
					}],
					function(err, results){
						callback(err, results);
					});
			},
			function(err, results){
				callback(err, results);
			}
		);
	});

}

function do_base_sync_task(source, id, callback){
	// get base_info of the job
	var base_info_sql = config.syncBaseInfoSelectSql.replace('?', source);
	base_info_sql = base_info_sql.replace('?', id);

	var base_info_event = config.syncBaseInfoSelectMysqlSelectEvent + source + id;

	mx_mysql.mysql_select(base_info_sql, base_info_event);

	mx_mysql.emitter.on(base_info_event, function(records){ 

		if (1 != records.length) {
			callback("not found record in base_info of job", null);
			return;
		}
		var json = JSON.stringify(records[0]);
		log.logger.debug(json);

		// gzip the base info.
		zlib.deflate(json, function(err, buffer) {
			if (err) {
				callback(err.toString(), null);
			} else {

				var body_key = config.syncBaseInfoRedisKey;
				var body_hash = source + ":" + id;

				hset_redis_callback(body_key, body_hash, buffer, callback);
				//hset_redis_callback(body_key, body_hash, json, callback);
			}
		});

	});
}

function do_index_task_v2(source, id, callback) {
	log.logger.trace(">>> do_index_task_v2: source(" + source + "),id(" + id + ")");
	var key = config.syncIndexRedisKey + source;

	async.series([
			function(callback){
				redis.zadd(key, id, id, function(err, result) {
					if (err) {
						callback("redis zadd error:" + err.toString(), null);
					} else {
						callback(null, "redis zadd success: key(" + key + "),id (" + id + ")");
					}
				});
			},
			function(callback){
				redis.expire(key, config.keyTimeOut, function(err){
					log.logger.debug("redis.expire, key(" + key + ")");
					if(err){
						callback("redis set timeout 30 days error:" + err.toString(), null);
					}else{
						callback(null, "redis zadd success with timeout 30 days:(" + key + "," + id + ")");
					}
				});
			}],
			function(err, results){
				callback(err, results);
			});
}

function get_done_range_from_redis_v2(key, redis_event) {
	log.logger.trace(">>> get_done_range_from_redis_v2: key(" + key + ")");

	async.series([
			function(callback) {	redis.zrange(key, 0, 0, function(err, min_id) { callback(err, min_id);});},
			function(callback) {	redis.zrange(key, -1, -1, function(err, max_id) { callback(err, max_id);});}
			],
			function(err, results) {
				if (err) {
					log.logger.error(err.toString());
				} else {
					log.logger.debug("results:" + results);
					mx_mysql.emitter.emit(redis_event, results);
				}
			});


}

function sync_start(source, db_min_id, db_max_id, redis_min_id, redis_max_id) {
	log.logger.trace("sync_start: source(" + source + "), db[" + db_min_id + "," + db_max_id + "],redis[" + redis_min_id + "," + redis_max_id + "]");

	if ((db_min_id === redis_min_id && db_max_id === redis_max_id) || (0 === db_min_id && 0 === db_max_id)) {

		log.logger.info("not need sync. source(" + source + "), db[" + db_min_id + "," + db_max_id + "],redis[" + redis_min_id + "," + redis_max_id + "]");

		return;
	}

	var max_id = db_max_id;
	var min_id = redis_max_id;
	if (null === redis_min_id) {
		min_id = 0;
	}
	min_id++;

	log.logger.debug("[" + min_id + "," + max_id + "]");

	
	if (min_id > max_id) {
		log.logger.info("not need sync. source(" + source + "), db[" + db_min_id + "," + db_max_id + "],redis[" + redis_min_id + "," + redis_max_id + "]");
		return;
	}
	

	async.map(MxArray(min_id, max_id), 
			function(id, callback){

				log.logger.debug("begin sync job_id:" + id);

				async.series([
					function(callback) {	get_status_of_task(source, id, callback);},
					function(callback) {	do_img_sync_task(source, id, callback);},
					function(callback) {	do_base_sync_task(source, id, callback);},
					function(callback) {	do_index_task_v2(source, id, callback);}
					],
					function(err, results) {
						callback(err, results);
					});

			},
			function(err, results) {
				if(err){
					log.logger.error(err.toString());
				}
				else{
					log.logger.info("results:"+ results);
				}
			});

}

var sync_task = function (){
	log.logger.trace(">>> begin sync_task");

	// get the [db_min_id, db_max_id] from mysql db.
	log.logger.debug("get [db_min_id, db_max_id] from db");
	mx_mysql.mysql_select(config.syncIndexSelectSql, config.syncIndexMysqlSelectEvent);
	mx_mysql.emitter.on(config.syncIndexMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[" + i + "] -> source(" + records[i].source + "), done_min_id(" +  records[i].done_min_id + "), done_max_id(" + records[i].done_max_id + ")");

			(function(i){

				var record = new Object;
				record.source = records[i].source;
				record.done_min_id = records[i].done_min_id;
				record.done_max_id = records[i].done_max_id;
				
				record.key = config.syncIndexRedisKey + record.source;
				record.redis_event = "redis_get:" + i;

				log.logger.debug("get [redis_min_id, redis_max_id] from redis");
			 	get_done_range_from_redis_v2(record.key, record.redis_event);

				mx_mysql.emitter.on(record.redis_event, function(replies){
					log.logger.info("after get [min_id, max_id] from redis length:" + replies.length + " i:" + i); 

					if (2 != replies.length) {
						log.logger.error("replies's lengthis not 2");
						return;
					}

					sync_start(record.source, record.done_min_id, record.done_max_id, replies[0], replies[1]);

				});

			})(i);
		}
	});

}

exports.lang_adapter_task = lang_adapter_task;
exports.country_adapter_task = country_adapter_task;
exports.channle_list_task = channle_list_task;
exports.sync_task = sync_task;


 
