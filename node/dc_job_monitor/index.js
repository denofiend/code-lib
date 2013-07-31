var express = require('express');
var conf = require('./bon/config');

var app = express();
app.configure(function(){
  app.set('views', __dirname+'/view');
  app.set('view engine', 'jade');
  app.set('view options', {layout:false});
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser());
  app.use(app.router);
  app.use(express.static(__dirname+'/public'));
});

require('./router').apply(app);
app.listen(conf.server_port);
console.log('on is running on port %d in %s mode',conf.server_port,app.settings.env);

