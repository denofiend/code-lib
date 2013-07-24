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


var lang_adapter_task = function (){
	log.logger.trace(">>> lang_adapter_task");

	mx_mysql.mysql_select(config.langSelectSql);

	mx_mysql.emitter.on('mysql_records', function(records){
		log.logger.debug("get reponse from mysql records length:" + records.length);

		for (var i = 0; i < records.length; ++i) {

			log.logger.debug("records[i].langad:" + records[i].langad);
			log.logger.debug("records[i].langios:" + records[i].langios);

			var langios = records[i].langios;
			var langad = records[i].langad;
			var key = config.langKey;

			redis.hset(key, langios, langad, function(err){
				if(err){

					log.logger.error("redis hset error:" + err.toString());

				}else{
					log.logger.info("redis hset success:(" + key, + "," + langios + "," + langad +")");
				}
			});
		}
	});

}

exports.lang_adapter_task = lang_adapter_task;

 
