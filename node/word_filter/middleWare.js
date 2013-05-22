/**
 * Created with JetBrains WebStorm.
 * User: lxx
 * Date: 13-4-18
 * Time: 下午5:39
 * To change this template use File | Settings | File Templates. inflate
 */
var log = require("./logger");
var zlib = require('zlib');


var getReqBody = function(req, res, next) {
    var data = [];
    req.addListener("data", function(chunk) {
        data.push(new Buffer(chunk));
    });
    req.addListener("end", function() {
        var buffer = Buffer.concat(data);
        if(req.header("Content-Encoding") === 'gzip'){
            log.logger.trace('content-encoding:gzip');
            zlib.gunzip(buffer, function(err, result) {
                if (!err) {
                    req.body = result.toString();
                    next();
                } else {
                    next(err);
                }
            });
        }else{
            req.body = buffer.toString();
            next();
        }

    });
}

/*
var getReqBody = function(req, res, next){
    var buf = '';
    req.setEncoding('binary');
    req.on('data', function(chunk){ buf += chunk });
    req.on('end', function(){
    	zlib.gunzip(buf.toString(),function(err,data){
    		if(err){
    			log.logger.info("error:" + err);
    			next(err);
    		}else{
    			req.body = data.toString();
        		next();
    		}
    	});

    });
};
*/

exports.getReqBody = getReqBody;



































