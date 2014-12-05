var crypto = require('crypto');  
var log = require("../logger");
var config = require("../config");
var fs = require('fs');
var Schema = require('protobuf').Schema;
var schema = new Schema(fs.readFileSync('proto/lottery.desc'));

function random (low, high) {
    return Math.random() * (high - low) + low;
}


function is_reward(chance) {
        return random(1, 10) <= (chance / 10);
}

function random_calls (n) {
    return 5 * random(1, n);
}


function day_lottery_limit_key(pn) {
    return getDateTime() + ":cl:" + pn;
}

function day_total_calls_key(pn) {
    return getDateTime() + ":tcl:" + pn;
}

function getDateTime() {
    var date = new Date();
    var year = date.getFullYear();
    var month = date.getMonth() + 1;
    month = (month < 10 ? "0" : "") + month;
    var day  = date.getDate();
    day = (day < 10 ? "0" : "") + day;

    return year + ":" + month + ":" + day;
}

/* Buffer */
function decode_request(req_body) {
    var decipher = crypto.createDecipheriv("rc4", config.rc4Key, '');     

    return Buffer.concat([decipher.update(req_body, "", ""), decipher.final("")]);
}

/* Buffer */
function encode_response(res_body) {
    var cipher = crypto.createCipheriv("rc4", config.rc4Key, '');    

    return Buffer.concat([cipher.update(res_body, "", ""), cipher.final("")]);
}

function parser_req(req_body, protobuf_message) {
    var Req = schema[protobuf_message];
    return Req.parse(decode_request(req_body));
}

function new_protobuf_message(protobuf_message) {
	return schema[protobuf_message];
}


exports.is_reward = is_reward;
exports.random_calls = random_calls;
exports.day_lottery_limit_key = day_lottery_limit_key;
exports.day_total_calls_key = day_total_calls_key;
exports.decode_request = decode_request;
exports.encode_response = encode_response;
exports.parser_req = parser_req;
exports.new_protobuf_message = new_protobuf_message;
