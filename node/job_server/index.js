var log = require("./logger");
var config = require('./config');
var cronJob = require('cron').CronJob;
var mx_task = require('./mx_task'); 



//mx_task.lang_adapter_task();
//mx_task.country_adapter_task();
//mx_task.channle_list_task();
//mx_task.sync_task();
//mx_task.check_consistency();

// for the dc_lang_adapter_tb task(sync data from db to redis)
new cronJob(config.langTaskCrontab, function(){
		log.logger.info('>>> start lang adapter sync task');

		mx_task.lang_adapter_task();
}, null, true);

// for the dc_country_adapter_tb task(sync data from db to redis)
new cronJob(config.countryTaskCrontab, function(){
	log.logger.info('>>> start country adapter sync task');

	mx_task.country_adapter_task();

}, null, true);

// for the dc_channlelist task(sync data from db to redis)
new cronJob(config.channlelistTaskCrontab, function(){
		log.logger.info('>>> start country adapter sync task');

			mx_task.channle_list_task();

}, null, true);

// for the dc_sync_task(sync data from db to redis)
new cronJob(config.syncTaskCrontab, function(){
		log.logger.info('>>> start body and base info sync task');

			mx_task.sync_task();

}, null, true);

/*
// for the check_consistency_task(sync data from db to redis)
new cronJob(config.checkConsistencyCron, function(){
	log.logger.info('>>> start check index and body sync task');

	mx_task.check_consistency();

}, null, true);
*/

  	
