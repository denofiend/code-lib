var log = require("./logger");
var cluster = require('cluster');
var config = require('./config');
var cronJob = require('cron').CronJob;
var mx_mysql = require('./mx_mysql'); 
var mx_task = require('./mx_task'); 


//mx_task.lang_adapter_task();
//mx_task.country_adapter_task();
//mx_task.channle_list_task();
mx_task.sync_task();

// for the dc_lang_adapter_tb task(sync data from db to redis)
new cronJob(config.langTaskCrontab, function(){
		log.logger.info('>>> start lang adapter sync task');

		//mx_task.lang_adapter_task();


}, null, true);

  	
