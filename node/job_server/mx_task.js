/**
 * New node file
 */
var log = require("./logger");
var config = require('./config');
var mx_mysql = require('./mx_mysql'); 
var redisModule = require("redis");
var async = require("async");
var crypto = require('crypto');

var redis = redisModule.createClient(config.redisPort,config.redisHost,{no_ready_check:true});
redis.on("error", function (err) {
	    log.logger.error("redis error event - " + config.redisHost + ":" + config.redisPort + " - " + err.toString());
});

function hset_redis(key, hash_key, hash_val){
	log.logger.trace(">>> hset_redis");

	redis.hset(key, hash_key, hash_val, function(err){
		if(err){

			log.logger.error("redis hset error:" + err.toString());

		}else{
			log.logger.info("redis hset success:(" + key + "," + hash_key + "," + hash_val + ")");
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

			hset_redis(config.channlelistKey, hash_key, records[i].chanlelist);

		}
	});

}

function getDoneRangeFromRedis(key, min_hash_key, max_hash_key, eventName) {
	log.logger.trace(">>> getDoneRangeFromReids: key: " + key + ", min_hash_key:" + min_hash_key + ",max_hash_key:" + max_hash_key);

	redis.hmget(key, min_hash_key, max_hash_key, function(err, rows){
		if(err){

			log.logger.error("redis hmget error:" + err.toString());

		}else{
			log.logger.debug("rows.length:", rows.length);

			for (i = 0; i < rows.length; ++i) {
				var j = 0;
				log.logger.debug("rows[i]:" + rows[i]);
			}

			log.logger.info("redis hmget success:(" + key + "," + min_hash_key + "," + max_hash_key +")");

			log.logger.debug("begin emitting: mxevent(" + eventName + ")");
			mx_mysql.emitter.emit(eventName, rows);
		}
	});
}


function do_parallel_job(source, min_id, max_id, callback) {
	var tmp_sql = config.syncDoneJobSelectSql.replace('?', source);

	for (var id = min_id; id <= max_id; ++id) {

		var sql = tmp_sql.replace('?', id);
		var eventName = config.syncDoneJobSelectMysqlSelectEvent + ":" + source + ":" + id;

		(function(sql, eventName, id){

			log.logger.debug("sql:" + sql);
			log.logger.debug("eventName:" + eventName);
			log.logger.debug("id:" + id);

			// get the status of the job
			mx_mysql.mysql_select(sql, eventName);

			mx_mysql.emitter.on(eventName, function(records){

				log.logger.debug("on eventName:" + eventName);

				if (0 === records.length) {

					log.logger.error("get nothing");

				} else if (1 != records.length) {
					log.logger.error("records's length is not 1 :" + records.length);

				} else {
					log.logger.debug("status:" + records[0].status);

					var job_status = records[0].status;

					if (0 === job_status) {

						log.logger.debug("status:" + records[0].status);
						log.logger.info("not get body success for source(" + source + "),id(" + id + ")");

					} else if (1 === job_status) {

						// get the body of the job
						log.logger.debug("job_status:" + job_status);

						var body_sql = config.syncBodySelectSql.replace('?', source);
						body_sql = body_sql.replace('?', id);

						var body_event_name = config.syncBodySelectMysqlSelectEvent + source + id;

						mx_mysql.mysql_select(body_sql, body_event_name);

						mx_mysql.emitter.on(body_event_name, function(records){
							// gzip
							log.logger.trace(">>> get body");


							if (0 === records.length) {

								log.logger.error("get nothing");

							} else if (1 != records.length) {
								log.logger.error("records's length is not 1 :" + records.length);

							} else {

								var job_body = records[0].summary;

								log.logger.debug("job_body(" + job_body + ")");

								//set body to redis 
								var body_redis_key = config.syncBodyRedisKey;
								var body_redis_hash = source + ":" + id;
								hset_redis(body_redis_key, body_redis_hash, job_body);
								

								// get the image of the job
								//
								var img_sql = config.syncImgSelectSql.replace('?', source);
								img_sql = img_sql.replace('?', id);

								var img_event_name = config.syncImgSelectMysqlSelectEvent + source + id;

								mx_mysql.mysql_select(img_sql, img_event_name);

								mx_mysql.emitter.on(img_event_name, function(records){ 

									log.logger.trace("img event name length:" + records.length);

									for (var i = 0; i < records.length; ++i) {
										log.logger.debug("i:" + i);

										var img_key = config.syncImgKey + source + ":" + id + ":" + records[i].imgsize;
										var img_value = records[i].imgurl;

										log.logger.debug("img_key(" + img_key + ",img_value(" + img_value + ")");

										(function(img_key, img_value) {
											log.logger.debug("img_key(" + img_key + ",img_value(" + img_value + ")");
											redis.set(img_key, img_value, function(err){
												if(err){

													log.logger.error("redis set error:" + err.toString());

												}else{
													log.logger.info("redis set success:(" + img_key + "," + img_value);
												}
											});


										})(img_key, img_value);
									}
									
								});



								// get base_info of the job
								exports.syncBaseInfoSelectSql = "SELECT `title`, `push_date`, `url`, `ifimg`, `lang`, `updatetime`, `contenttype`, `status`, `hashcode`, `description` FROM `dc_done_job_tb` WHERE `source` = '?' AND `id` = ?"
								var base_info_sql = config.syncBaseInfoSelectSql.replace('?', source);
								base_info_sql = base_info_sql.replace('?', id);

								var base_info_event = config.syncBaseInfoSelectMysqlSelectEvent + source + id;

								mx_mysql.mysql_select(base_info_sql, base_info_event);

								mx_mysql.emitter.on(base_info_event, function(records){ 

									
									if (1 != records.length) {
										log.logger.error("not found record in base_info of job");
										return;
									}
									var json = JSON.stringify(records[0]);
									log.logger.debug(json);

									var body_key = config.syncBaseInfoRedisKey;
									var body_hash = source + ":" + id;

									hset_redis(body_key, body_hash, json);

								});
							}
						});
					} else {
						log.logger.error("status is not 1 or 0");
					}
				}
			});
		})(sql, eventName, id);

	}
	callback(null, 'paralel_job success');
}
function body_img_job(source, min_id, max_id, callback) {
	async.parallel([
			function(callback){
				do_parallel_job(source, min_id, max_id, callback);
			}
			],
			// optional callback
			function(err, results){
				// the results array will equal ['one','two'] even though
				// the second function had a shorter timeout.
				callback(null, 'body_img_job success');
			});
}

function index_set(source, min_id, max_id, callback) {
	var index_key = config.syncIndexRedisKey;
	var min_hash_key = source + ":min_id";
	hset_redis(index_key, min_hash_key, min_id);

	var max_hash_key = source + ":max_id";
	hset_redis(index_key, max_hash_key, max_id);

	callback(null, 'redis success');
}

function sync_start(source, db_min_id, db_max_id, redis_min_id, redis_max_id) {
	log.logger.trace("sync_start: source(" + source + "), db[" + db_min_id + "," + db_max_id + "],redis[" + redis_min_id + "," + redis_max_id + "]");

	if ((db_min_id === redis_min_id && db_max_id === redis_max_id) || (0 === db_min_id && 0 === db_max_id)) {

		log.logger.info("not need sync. source(" + source + "),[db_min_id, db_max_id]->[" + db_min_id + "," + db_max_id + "]");
		return;
	}


	var max_id = db_max_id;
	var min_id;
	if (null === redis_min_id) {
		min_id = db_min_id;
	} else{
		min_id = redis_max_id + 1;
	}

	log.logger.debug("[" + min_id + "," + max_id + "]");
			//do_parallel_job(source, min_id, max_id), 

	async.series([
			function(callback){body_img_job(source, min_id, max_id, callback)},
			function(callback){index_set(source, min_id, max_id, callback)}
			],
			function(err, results) {

				if(err){

					log.logger.error("redis set error:" + err.toString());
				}
				log.logger.debug("results:"+ results);
			});

}

var sync_task = function (){
	log.logger.trace(">>> sync_task");

	// get the [db_min_id, db_max_id] from mysql db.
	//
	mx_mysql.mysql_select(config.syncIndexSelectSql, config.syncIndexMysqlSelectEvent);

	mx_mysql.emitter.on(config.syncIndexMysqlSelectEvent, function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[i].source:" + records[i].source);
			log.logger.debug("records[i].done_min_id:" + records[i].done_min_id);
			log.logger.debug("records[i].done_max_id:" + records[i].done_max_id);

			(function(i){

				log.logger.debug("i:" + i);
				var record = new Object;
				record.source = records[i].source;
				record.done_min_id = records[i].done_min_id;
				record.done_max_id = records[i].done_max_id;
				
				record.key = config.syncIndexKey;
				record.min_hash_key = record.source + ":done_min_id";
				record.max_hash_key = record.source + ":done_max_id";
				record.redis_event = "redis_get:" + i;

			 	getDoneRangeFromRedis(record.key, record.min_hash_key, record.max_hash_key, record.redis_event);
				

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


 
