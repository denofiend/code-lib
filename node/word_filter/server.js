/*
var express = require('express');
 
var app = express();
 
app.get('/wines', function(req, res) {
	    res.send([{name:'wine1'}, {name:'wine2'}]);
});
app.get('/wines/:id', function(req, res) {
	    res.send({id:req.params.id, name: "The Name", description: "description"});
});
 
app.listen(3000);
console.log('Listening on port 3000...');
*/

var mx_word_filter = require("./mx_word_filter")
var config = require('./config');
var log = require("./logger");
var middleWare = require("./middleWare");
var express = require('express');

var start = function(){
	var app = express();



	app.use(middleWare.getReqBody);

	//app.set('views', __dirname + './');
	//app.use(express.static(__dirname + './html'));
	//app.engine('html', require('ejs').renderFile);
	app.set('views', __dirname + '/html');
	app.engine('html', require('ejs').renderFile);
	app.engine('js', require('ejs').renderFile);


	app.post('/nickname/filter/get/v1', mx_word_filter.word_filter_get);
	app.post('/nickname/filter/set/v1', mx_word_filter.word_filter_set);

	app.get('/hello.html', function(req, res) {

		res.render('hello.html');
	});

	app.listen(config.serverPort);
	log.logger.info("server listen on port:" + config.serverPort);
};

exports.start = start;

