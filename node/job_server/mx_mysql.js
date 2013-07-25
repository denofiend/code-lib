/**
 * New node file
 */
var log = require("./logger");
var config = require('./config');
var async = require("async"); 
var EventEmitter = require('events').EventEmitter;
var emitter = new EventEmitter;

var mysql = require('mysql');
var pool  = mysql.createPool({
	host     : config.db_host,
	user     : config.db_user,
	password : config.db_pwd,
	database : config.database
});

var mysql_select = function (sql, mx_event){
	log.logger.trace(">>> mysql_select sql(" + sql + ")");

	pool.getConnection(function(err, connection) {

		if (err) {
				log.logger.error("mysql error:" + err.toString());
				return;
		}
		// Use the connection
		connection.query(sql, function(err, rows) {

			if(err){

				log.logger.error("mysql error:" + err.toString());
				connection.end();
				return;
			}

			log.logger.debug("rows length:" + rows.length);

			connection.end();

			log.logger.debug("begin emitting: mxevent(" + mx_event + ")");
			emitter.emit(mx_event, rows);

		});
	});

}

exports.mysql_select = mysql_select;
exports.emitter = emitter;


 
