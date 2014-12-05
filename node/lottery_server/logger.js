/**
 * New node file
 */

exports.log4js = require('log4js');
exports.log4js.clearAppenders();

exports.log4js.loadAppender('file');
exports.log4js.loadAppender('dateFile');
exports.log4js.loadAppender('multiprocess');

exports.getLogger = function(name){
	return exports.log4js.getLogger(name);
};

exports.addLogger = function(name,level,daily){
	exports.log4js.addAppender(exports.log4js.appenders.dateFile(
				exports.conf.log.path+name+'.log','.yyyyMMdd'), name);
	var logger = exports.log4js.getLogger(name);
	if(!!level){
		logger.setLevel(level);
	}else{
		logger.setLevel(exports.conf.log.level);
	}
	return logger;
};

var log4js = require('log4js');
var config = require("./config");

	log4js.configure(
  	{
    	"appenders": [{
          "type": "logLevelFilter",
      		"level": config.logLevel,
      		"category": "qh_lottery",
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
    	}],
      replaceConsole: true
  	});

exports.logger =  log4js.getLogger('qh_lottery');
exports.chukwa = log4js.getLogger('chukwa');