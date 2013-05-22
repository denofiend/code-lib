var log = require("./logger");
var cluster = require('cluster');
var config = require('./config');
var server = require('./server');
 
  	
if (cluster.isMaster) {
	var  logger = log.logger; 
  	for (var i = 0; i < config.processNum; i++) {
    	var worker = cluster.fork();
    	logger.info('start new process ' + worker.process.pid);
  	}
  	cluster.on('exit', function(worker, code, signal) {
    	logger.error('process ' + worker.process.pid + ' died');
    	var newWorker = cluster.fork();
    	logger.info('restart new process ' + newWorker.process.pid);
  	});
} else {
   	log.logger.info('process ' +  process.pid + ' start to work');
   	log.chukwa.info("hello,i am chukwa log");

   	process.on('uncaughtException',function(error){
   	    if(typeof(error)==='object'){
      		log.logger.error('uncaughtException on %d: %s',process.pid,error.stack?error.stack:util.inspect(error));
    	}else{
      		log.logger.error('uncaughtException on %d: %s',process.pid,error);
    	}
        var killtimer = setTimeout(function() {
            process.exit(1);
        }, 30000);

        killtimer.unref();
        cluster.worker.disconnect();
  	});
 	server.start();
}




