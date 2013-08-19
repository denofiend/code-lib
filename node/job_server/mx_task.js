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
	    log.logger.error("[fatalerror] redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});

/*
redis.on("end", function (err) {
	if (err) {
		log.logger.error("[fatalerror] " + err.toString());
	} else {
		log.logger.info("redis end by server reconnect.");
		redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true, return_buffers: true });
	}
});
*/

function MxArray(min_id, max_id) {
	var ids = new Array();
	for (var id = min_id; id <= max_id; ++id) {
		ids.push(id);
	}
	return ids;
}

function hset_redis_callback(key, hash_key, hash_val, callback){
	key = key.toLowerCase();
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
	key = key.toLowerCase();
	redis.del(key, function(err) {
		if (err) {
			callback("redis del key error: " + err.toString(), null);
		} else {
			callback(null, "redis del key  success:(" + key + ")");
		}
	});
}

function 	redis_del_key(key) {
	key = key.toLowerCase();
	redis.del(key, function(err) {
		if (err) {
			log.logger.error("[fatalerror] redis del key error: " + err.toString());
		} else {
			log.logger.info("redis del key  success:(" + key + ")");
		}
	});
}

function redis_set(key, value, callback) {
	key = key.toLowerCase();
	redis.set(key, value, function(err){
		if (err) {
			callback("redis set error: " + err.toString(), null);
		} else {
			callback(null, "redis set success:(" + key + ")");
		}
	});
}

function hset_redis(key, hash_key, hash_val){
	key = key.toLowerCase();
	hash_key = hash_key.toLowerCase();
	redis.hset(key, hash_key, hash_val, function(err){
		if (err) {
			log.logger.error("[fatalerror] redis hset error:" + err.toString());
		} else {
			log.logger.info("redis hset success (" + key + "," + hash_key + "," + hash_val + ")");
		}
	});
}

var lang_adapter_task = function (){
	log.logger.trace(">>> lang_adapter_task");

	//redis_del_key(config.langKey);

	mx_mysql.mysql_select(config.langSelectSql, config.langMysqlSelectEvent);

	mx_mysql.emitter.once(config.langMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {
			
			log.logger.debug("records[i].langad:" + records[i].langad);
			log.logger.debug("records[i].langios:" + records[i].langios);
			hset_redis(config.langKey, records[i].langios, records[i].langad);

		}
	});

}

function isJSON(data) {
	var isJson = false;
	try {
		// this works with JSON string and JSON object, not sure about others
		var json = JSON.parse(data);
		isJson = typeof json === 'object' ;
	} catch (ex) {
		log.logger.info(data + " is not json");
	}
	return isJson;
}

var country_adapter_task = function (){
	log.logger.trace(">>> country_adapter_task");

	//redis_del_key(config.countryKey);

	mx_mysql.mysql_select(config.countrySelectSql, config.countryMysqlSelectEvent);

	mx_mysql.emitter.once(config.countryMysqlSelectEvent, function(records){
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

	//redis_del_key(config.channlelistKey);

	mx_mysql.mysql_select(config.channlelistSelectSql, config.channlelistMysqlSelectEvent);

	mx_mysql.emitter.once(config.channlelistMysqlSelectEvent, function(records){
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
			if (1 === Number(records[i].ifdefault)) {
				hash_key = "default_channle_list";
			}
			log.logger.debug("hash_key:" + hash_key);

			if (isJSON(records[i].channlelist)) {

				hset_redis(config.channlelistKey, hash_key, records[i].channlelist);
			}

		}
	});

}

function get_status_of_task(source, id, callback) {
	log.logger.trace(">>> get_status_of_task");
	/* get status from the db */
	var tmp_sql = config.syncDoneJobSelectSql.replace('?', source);
	var sql = tmp_sql.replace('?', id);
	var eventName = config.syncDoneJobSelectMysqlSelectEvent + ":" + source + ":" + id;

	mx_mysql.mysql_select(sql, eventName);

	mx_mysql.emitter.once(eventName, function(records){
		if (0 === records.length) {
			//callback(null, "not found record in mysql for source(" + source + "),id(" + id +")");
			callback("not found status record in mysql for source(" + source + "),id(" + id +")", null);

		} else if (1 != records.length) {
			callback("records's length is not 1 for source(" + source + "),id(" + id + "),records(" + records + ")", null);
		} else {
			var status = Number(records[0].status);
			log.logger.debug("status is :" + status);

			if (1 === status) {
				callback(null, 'status is 1, just go');
			}
			else {
				callback("status is not 1 for source(" + source + "),id(" + id + ")", null);
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

	mx_mysql.emitter.once(img_event_name, function(records){ 

		log.logger.trace("img event name length:" + records.length);
		async.map(records, 
			function(record, callback){
				var img_key = config.syncImgKey + source + ":" + id + ":" + record.imgsize;
				var img_value = record.imgurl;

				log.logger.debug("img_key(" + img_key + ",img_value(" + img_value + ")");

				redis_set(img_key, img_value, callback);

			},
			function(err, results){
				callback(err, results);
			}
		);
	});

}

function do_body_sync_task(source, max_id, id, callback) {
	log.logger.trace("do_body_sync_task: source(" + source + "), max_id(" + max_id + "),id(" + id + ")");

	if (id + config.syncBodycount < max_id){
		callback(null, "not need sync body to redis for source(" + source + "), max_id(" + max_id + "),id(" + id + ")");
		return;
	}
	var body_sql = config.syncBodySelectSql.replace('?', source);
	body_sql = body_sql.replace('?', id);

	var body_event_name = config.syncBodySelectMysqlSelectEvent + source + id;
	mx_mysql.mysql_select(body_sql, body_event_name);

	mx_mysql.emitter.once(body_event_name, function(records){

		if (1 != records.length) {
			callback("not found body of job source(" + source + "),id" + id + ")", null);
			return;
		}

		// gzip the base info.
		zlib.deflate(records[0].summary, function(err, buffer) {
			if (err) {
				callback(err.toString(), null);
			} else {

				var body_redis_key = config.syncBodyRedisKey + ":" +  source + ":" + id;

				redis_set(body_redis_key, buffer, callback);
			}
		});


	});

}
function do_base_sync_task(source, id, callback){
	log.logger.trace("do_base_sync_task: source(" + source + "),id(" + id + ")");
	// get base_info of the job
	var base_info_sql = config.syncBaseInfoSelectSql.replace('?', source);
	base_info_sql = base_info_sql.replace('?', id);

	var base_info_event = config.syncBaseInfoSelectMysqlSelectEvent + source + id;

	mx_mysql.mysql_select(base_info_sql, base_info_event);

	mx_mysql.emitter.once(base_info_event, function(records){ 

		if (1 != records.length) {
			callback("not found record in base_info of job source(" + source + "),id" + id + ")", null);
			return;
		}

		try{

			var json = JSON.stringify(records[0]);
			log.logger.debug(json);
		} catch (ex) {
			callback("JSON stringify error: " + ex.toString(), null);
		}


		// gzip the base info.
		zlib.deflate(json, function(err, buffer) {
			if (err) {
				callback(err.toString(), null);
			} else {

				var base_key = config.syncBaseInfoRedisKey;
				var base_hash = source + ":" + id;

				redis_set(base_key + ":" + base_hash, buffer, callback);
			}
		});

	});
}

function do_index_task_v2(source, id, callback) {
	log.logger.trace(">>> do_index_task_v2: source(" + source + "),id(" + id + ")");

	var key = config.syncIndexRedisKey + source;
	redis.zadd(key, id, id, function(err, result) {
		if (err) {
			callback("redis zadd error:" + err.toString(), null);
		} else {
			callback(null, "redis zadd success: key(" + key + "),id (" + id + ")");
		}
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
					log.logger.error("[fatalerror] " + err.toString());
				} else {
					log.logger.debug("results:" + results);
					mx_mysql.emitter.emit(redis_event, results);
				}
			});
}
function get_set_count_from_redis(key, redis_event) {
	key = key.toLowerCase();
	redis.zcount(key, '-inf', '+inf', function(err, results) {
		if (err) {
			log.logger.error("[fatalerror] get_set_count err: " + err.toString());
		} else {
			log.logger.debug("get_set_count results:" + results);
			mx_mysql.emitter.emit(redis_event, results);
		}
	});
}

function do_img_del_task(source, id, callback) {
	log.logger.debug("begin del img source(" + source + ",id(" + id + ")");

	var img_sql = config.syncImgSelectSql.replace('?', source);
	img_sql = img_sql.replace('?', id);

	var img_event_name = config.syncImgSelectMysqlSelectEvent + source + id + ":del";
	mx_mysql.mysql_select(img_sql, img_event_name);

	mx_mysql.emitter.once(img_event_name, function(records){ 

		log.logger.trace("img event name length:" + records.length);
		async.map(records, 
			function(record, callback){
				var img_key = config.syncImgKey + source + ":" + id + ":" + record.imgsize;

				log.logger.debug("del record img_key(" + img_key + ")");

				redis_del_key_callback(img_key, callback);

			},
			function(err, results){
				callback(err, results);
			}
			);
	});
}

function do_base_del_task(source, id, callback) {
	log.logger.debug("begin del base source(" + source + ",id(" + id + ")");

	var body_key = config.syncBaseInfoRedisKey.toLowerCase();
	var body_hash = source + ":" + id;
	body_hash = body_hash.toLowerCase();

	redis.hdel(body_key, body_hash, function(err) {
		if (err) {
			callback("del base error:" + err.toString(), null);
		} else {
			callback(null, "del base success, body_key(" + body_key + "), body_hash(" + body_hash + ")");
		}
	});
}

function do_index_del_v2(source, id, callback) {
	log.logger.debug("begin del index source(" + source + ",id(" + id + ")");

	var key = config.syncIndexRedisKey + source;
	key = key.toLowerCase();

	redis.zrem(key, id, function(err, result) {
		if (err) {
			callback("redis zemd error:" + err.toString(), null);
		} else {
			callback(null, "redis zrem success: key(" + key + "),id (" + id + ")");
		}
	});

}


function del_old_records(source, sta, end) {
	log.logger.trace("begin del old records [" + sta + "," + end + "] of source:" + source);
	async.map(MxArray(sta, end), 
			function(id, callback){

				log.logger.debug("begin sync job_id:" + id);

				async.series([
					function(callback) {	do_img_del_task(source, id, callback);},
					function(callback) {	do_base_del_task(source, id, callback);},
					function(callback) {	do_index_del_v2(source, id, callback);}
					],
					function(err, results) {
						callback(err, results);
					});

			},
			function(err, results) {
				if(err){
					log.logger.error("[fatalerror] " + err.toString());
				}
				else{
					log.logger.info("results:"+ results);
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
					function(callback) {	do_body_sync_task(source, max_id, id, callback);},
					function(callback) {	do_index_task_v2(source, id, callback);}
					],
					function(err, results) {
						callback(err, results);
					});

			},
			function(err, results) {
				if(err){
					if (err.toString().match('status is not 1 for source') || err.toString().match('not found status record in mysql for source')) {
						log.logger.info("[not-found]:" + err.toString());
					} else {
						log.logger.error("[fatalerror] " + err.toString());
					}
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
	mx_mysql.emitter.once(config.syncIndexMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[" + i + "] -> source(" + records[i].source + "), done_min_id(" +  records[i].done_min_id + "), done_max_id(" + records[i].done_max_id + ")");

			(function(i){

				var record = new Object;
				record.source = records[i].source;
				record.done_min_id = records[i].done_min_id;
				record.done_max_id = records[i].done_max_id;
				record.cache_count = records[i].cache_count;
				
				record.key = config.syncIndexRedisKey + record.source;
				record.redis_event = record.key + ":redis_get:" + i;

				log.logger.debug("get [redis_min_id, redis_max_id] from redis");
			 	get_done_range_from_redis_v2(record.key, record.redis_event);

				mx_mysql.emitter.once(record.redis_event, function(replies){
					log.logger.info("after get [min_id, max_id] from redis length:" + replies.length + " i:" + i); 

					if (2 != replies.length) {
						log.logger.error("[fatalerror] replies's lengthis not 2");
						return;
					}

					sync_start(record.source, record.done_min_id, record.done_max_id, replies[0], replies[1]);

				});

				var totalRecordsEvent = "get_set_count_from_redis:" + i;
				var totalRecords = get_set_count_from_redis(record.key, totalRecordsEvent);

				mx_mysql.emitter.once(totalRecordsEvent, function(totalRecords) {
					log.logger.info("total Records of the " + record.source + ":" + totalRecords + ", cache_count(" + record.cache_count + ")");

					if (1.5 * record.cache_count <= totalRecords) {
						del_old_records(record.source, 0, (totalRecords - record.cache_count));
					}

				});
				

			})(i);
		}
	});

}


function redis_exists_callback(key, callback) {
	key = key.toLowerCase();
	redis.exists(key, function(err, res) { 
		if (err) {
			callback(err.toString(), null)
		} else { 
			log.logger.debug("key(" + key + "), res(" + res + ")");
			if (0 === Number(res)) {
				callback(null, key + " is not exists"); 
			} else { 
				callback(key + " is exists", null) 
			} 
		}
	});
}

var check_consistency = function() {
	log.logger.debug("check_consistency");

	var check_event = config.syncIndexMysqlSelectEvent + ":check";
	mx_mysql.mysql_select(config.syncIndexSelectSql, check_event);

	mx_mysql.emitter.once(check_event, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			(function(i){
				var source = records[i].source;
				var key = config.syncIndexRedisKey + source;

				redis.zrange(key, config.checkConsistencyCount, -1, function(err, ids) {
					async.map(ids, 
						function(id, callback){

							var body_key = config.syncBaseInfoRedisKey + ":" + source + ":" + id;

							log.logger.debug("begin check consistency source(" + source + "),id(" + id +")");

							async.series([
								function(callback) {	redis_exists_callback(body_key, callback);},
								function(callback) {	do_base_sync_task(source, id, callback);},
								],
								function(err, results) {
									callback(err, results);
								});

						},
						function(err, results) {
							if(err){
								log.logger.info("check consistency ERROR:" + err.toString());
							}
							else{
								log.logger.info("check results:"+ results);
							}
						});
				});

			})(i);

		}});

}

exports.lang_adapter_task = lang_adapter_task;
exports.country_adapter_task = country_adapter_task;
exports.channle_list_task = channle_list_task;
exports.sync_task = sync_task;
exports.check_consistency = check_consistency;

 
