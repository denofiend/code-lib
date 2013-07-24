/**
 * New node file
 */

var log4js = require('log4js');
var config = require("./config");

	log4js.configure(
  	{
    	"appenders": [{
      		"type": "logLevelFilter",
      		"level": config.logLevel,
      		"category": "mx_word_filter",
      		"appender": {
        		"type": "file",  		
				"filename": config.serverLogPath,
    			"layout": {
          			"type": "pattern",
          			"pattern": "%[%d(%x{pid})[%p]%]%m",
          			"tokens": {
            			"pid" : function() { return process.pid; }
          			}
        		}
      		}
    	},
		{
      		"type": "logLevelFilter",
      		"level": config.logLevel,
      		"category": "chukwa",
      		"appender": {
        		"type": "file",  		
				"filename": config.chukwaLogPath,
    			"layout": {
          			"type": "pattern",
          			"pattern": "%[%d(%x{pid})[%p]%]%m",
          			"tokens": {
            			"pid" : function() { return process.pid; }
          			}
        		}
      		}
    	}]
  	});




exports.logger =  log4js.getLogger('mx_word_filter');
exports.chukwa = log4js.getLogger('chukwa');






/*
var initLogReceiver = function(){
	log4js.configure(
  	{
    	"appenders": [{
      		"type": "logLevelFilter",
      		"level": config.logLevel,
      		"category": "smartDownload",
      		"appender": {
        		"type": "multiprocess",
        		"mode": "master",        		
        		"loggerPort": config.serverLogPort,
        		"loggerHost": "127.0.0.1",
        		"appender": {
    				"type": "dateFile",
    				"filename": config.serverLogPath,
    				"pattern": ".yyyy-MM-dd",    				
    				"reserveDays":config.logReservedDays,
    				"layout": {
          				"type": "pattern",
          				"pattern": "%[%d(%x{pid})[%p]%]%m%n",
          				"tokens": {
            				"pid" : function() { return process.pid; }
          				}
        			}
        		}
      		}
    	},
    	{
      		"type": "logLevelFilter",
      		"level": config.logLevel,
      		"category": "chukwa",
      		"appender": {
        		"type": "multiprocess",
        		"mode": "master",
        		"loggerPort": config.chukwaLogPort,
        		"loggerHost": "127.0.0.1",
        		"appender": {
    				"type": "dateFile",
    				"filename": config.chukwaLogPath,
    				"pattern": ".yyyy-MM-dd",    				
    				"reserveDays":config.logReservedDays,
    				"layout": {
          				"type": "pattern",
          				"pattern": "%[%d(%x{pid})[%p]%]%m%n",
          				"tokens": {
            				"pid" : function() { return process.pid; }
          				}
        			}
        		}
      		}
    	}]
  	});
  	
	logger = exports.logger =  log4js.getLogger('smartDownload');
	chukwa = exports.chukwa = log4js.getLogger('chukwa');
}

var initLogSender = function(){
	log4js.configure(
  	{
    	"appenders": [{
      	"type": "logLevelFilter",
      	"level": config.logLevel,
      	"category": "smartDownload",
      	"appender": {      		
        	"type": "multiprocess",
        	"mode": "worker",
        	"loggerPort":config.serverLogPort,
        	"loggerHost": "127.0.0.1"
      		}
    	},
    	{
      	"type": "logLevelFilter",
      	"level": config.logLevel,
      	"category": "chukwa",
      	"appender": {      		
        	"type": "multiprocess",
        	"mode": "worker",
        	"loggerPort":config.chukwaLogPort,
        	"loggerHost": "127.0.0.1"
      		}
    	}]
  	});
  	
	logger = exports.logger = log4js.getLogger('smartDownload');
	chukwa = exports.chukwa = log4js.getLogger('chukwa');
}

exports.initLogReceiver = initLogReceiver;
exports.initLogSender = initLogSender;
 
*/ 

/* 


	log4js.configure(
  	{
    	"appenders": [{
      		"type": "logLevelFilter",
      		"level": config.logLevel,
        		"appender": {
    				"type": "dateFile",
    				"filename": config.logPath,
    				"pattern": ".yyyy-MM-dd",
    				"category": "contacts",
    				"reserveDays":config.logReservedDays,
    				"layout": {
          				"type": "pattern",
          				"pattern": "%[%d(%x{pid})[%p]%]%m",
          				"tokens": {
            				"pid" : function() { return process.pid; }
          				}
        			}
        		}
    	}]
  	});
  	
 exports.logger =  log4js.getLogger('contacts');
*/

 





/*
log4js.configure({
  appenders: [
	{
    	"type": "dateFile",
    	"filename": config.onlineLogPath,
    	"pattern": ".yyyy-MM-dd",
    	"category": "online",
    	"reserveDays":config.logReservedDays
  	},
  	{
    	"type": "dateFile",
    	"filename": config.scoreLogPath,
    	"pattern": ".yyyy-MM-dd",
    	"category": "score",
    	"reserveDays":config.logReservedDays
  	},
  	{
    	"type": "dateFile",
    	"filename": config.httpLogPath,
    	"pattern": ".yyyy-MM-dd",
    	"category": "http",
    	"reserveDays":config.logReservedDays
  	}
  ]
});
 
log4js.clearAppenders('console');
log4js.loadAppender('dateFile');
log4js.addAppender(log4js.appenders.dateFile(config.onlineLogPath), 'online');
log4js.addAppender(log4js.appenders.dateFile(config.scoreLogPath), 'score');
log4js.addAppender(log4js.appenders.dateFile(config.httpLogPath), 'http');
 
var onlineLogger = log4js.getLogger('online');
onlineLogger.setLevel('debug');

var scoreLogger = log4js.getLogger('score');
scoreLogger.setLevel('debug');

var httpLogger = log4js.getLogger('http');
httpLogger.setLevel('debug');
 
 
 

 

exports.onlineLogger = onlineLogger
exports.scoreLogger = scoreLogger
exports.httpLogger = httpLogger

*/



