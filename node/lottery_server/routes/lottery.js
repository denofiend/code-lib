var express = require('express');
var router = express.Router();
var log = require("../logger");
var config = require("../config");
var qh_redis = require("../qh_redis");
var crypto = require('crypto');  
var lt_util = require('./lottery_util');

/*  1. 摇奖接口 */
router.use('/lt', function(req, res) {
    log.logger.trace("begin lotter request");

    var ltreq = lt_util.parser_req(req.body, 'qihoo.protocol.lottery.LotteryRequest');
    log.logger.info("after rc4 decocde, protobuf decode: req:" + JSON.stringify(ltreq));

    var lottery_lt = require("./lottery_lt");
    lottery_lt.lottery_lt(ltreq.pn, res);
});

/* 2. 获取摇奖历史接口：只返回邀请码 */
router.use('/lth', function(req, res) {
    log.logger.trace("begin mget lottery history request");

    var preq = lt_util.parser_req(req.body, 'qihoo.protocol.lottery.MgetLotterHistoryRequest');

    log.logger.info("after rc4 decocde, protobuf decode: req:" + JSON.stringify(preq));

    var Res = lt_util.new_protobuf_message('qihoo.protocol.lottery.MgetLotterHistoryResponse');

    var qh_lottery = {history : [{type : 'INVITE_CODE', invitecode: 12345}]};

    res.send(encode_response(Res.serialize(qh_lottery)));
});

/* 3. 打完电话以后，摇奖接口：只返回通话时长*/
router.use('/clt', function(req, res) {
    log.logger.trace("begin call lottery request");

    var preq = lt_util.parser_req(req.body, 'qihoo.protocol.lottery.AfterCallLotteryRequest');

    log.logger.info("after rc4 decocde, protobuf decode: req:" + JSON.stringify(preq));

    var Res = lt_util.new_protobuf_message('qihoo.protocol.lottery.AfterCallLotteryResponse');

    var qh_lottery = {lotter: {type : 'CALLS', calls: 15}};

    res.send(encode_response(Res.serialize(qh_lottery)));
});

/* 4. 获取剩余摇奖次数：客户端如果没有本地数据的时候来服务器拿。*/
router.use('/clt', function(req, res) {
    log.logger.trace("begin call lottery request");


    var preq = lt_util.parser_req(req.body, 'qihoo.protocol.lottery.LeftLotteryNumRequest');

    log.logger.info("after rc4 decocde, protobuf decode: req:" + JSON.stringify(preq));

    var Res = lt_util.new_protobuf_message('qihoo.protocol.lottery.LeftLotteryNumResponse');
    var qh_lottery = {num: 3};

    res.send(encode_response(Res.serialize(qh_lottery)));
});

module.exports = router;
