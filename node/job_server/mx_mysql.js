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

var mysql_select = function (sql){
	log.logger.trace(">>> lang_select");

	pool.getConnection(function(err, connection) {

		if (err) {
				log.logger.error("mysql error:" + err.toString());
				return;
		}
		// Use the connection
		//connection.query('SELECT `langad`, `langios` FROM `dc_lang_adapter_tb`', function(err, rows) {
		connection.query(sql, function(err, rows) {
			//       // And done with the connection.

			if(err){

				log.logger.error("mysql error:" + err.toString());
				return;
			}

			var i = 0;
			log.logger.debug("rows length:" + rows.length);
			for (i = 0; i < rows.length; ++i) {
				var j = 0;
				log.logger.debug("rows[i].langad:" + rows[i].langad);
				log.logger.debug("rows[i].langios:" + rows[i].langios);
			}

			connection.end();

			log.logger.debug("begin emitting");
			emitter.emit('mysql_records', rows);

		});
	});

	/*
	emitter.on('records', function(records){
			log.logger.debug("records length:" + records.length);

	});
	*/
}

exports.mysql_select = mysql_select;
exports.emitter = emitter;


 
