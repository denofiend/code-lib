var cloudservice = require('./cloudservice');
var push = require('./push');
var job_service = require('./job_service');

exports.apply = function(server){
  cloudservice.apply(server);
  push.apply(server);
  job_service.apply(server);
  server.get('/test',function(req,res){
    res.render('test');
  });
};

