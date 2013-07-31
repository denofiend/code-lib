exports.apply = function(server){
  server.get('/push/release',function(req,res){
    var conf = {
      cs: {
        ips:[
          '223.202.36.47',
          '223.202.36.48',
          '223.202.36.49',
          '223.202.36.50',
          '223.202.36.51'],
        port: 8801,
        count: 16,
        data:{},
      },
      bs: {
        ips:[
          '10.0.16.102',
          '10.0.16.103'],
        port: 8801,
        count: 15,
        data: {},
      },
      ps: {
        ips:[
          '10.0.16.104',
          '10.0.16.105',
          '10.0.16.106'],
        port:8801,
        count:16,
        data: {},
      }
    };
    res.render('push',{
      confstr: JSON.stringify(generate_conf(conf))});
  });

  server.get('/push/test',function(req,res){
    var conf = {
      cs: {
        ips:['10.0.6.112'],
        port: 8811,
        count: 4,
        data:{},
      },
      bs: {
        ips:['10.0.6.113'],
        port: 8821,
        count: 2,
        data: {},
      },
      ps: {
        ips:['10.0.6.113'],
        port:8841,
        count:2,
        data: {},
      }
    };
    res.render('push',{
      confstr: JSON.stringify(generate_conf(conf))});
  });

  function generate_conf(conf){
    conf.cs.ips.forEach(function(cs){
      var csd = conf.cs.data;
      var cnt = conf.cs.count;
      var pt = conf.cs.port;
      csd[cs] = {};
      for(var i=0;i<cnt;++i){
        csd[cs][pt+i] = {
          client: 0,
          bs: 0,
          ps: 0,
          tq_interval: 0,
          tq_concurrency: 0,
          tq_timeout: 0,
          tq_working: 0,
          tq_total: 0,
        };
      }
    });
    conf.ps.ips.forEach(function(ps){
      var psd = conf.ps.data;
      var cnt = conf.ps.count;
      var pt = conf.ps.port;
      psd[ps] = {};
      for(var i=0;i<cnt;++i){
        psd[ps][pt+i] = {
          client: 0,
          tq_interval: 0,
          tq_concurrency: 0,
          tq_timeout: 0,
          tq_working: 0,
          tq_total: 0,
        };
      }
    });
    conf.bs.ips.forEach(function(bs){
      var bsd = conf.bs.data;
      var cnt = conf.bs.count;
      var pt = conf.bs.port;
      bsd[bs] = {};
      for(var i=0;i<cnt;++i){
        bsd[bs][pt+i] = {
          client: 0,
          tq_interval: 0,
          tq_concurrency: 0,
          tq_timeout: 0,
          tq_working: 0,
          tq_total: 0,
        };
      }
    });
    return conf;
  }
};
