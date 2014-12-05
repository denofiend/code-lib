var lt_util = require('./lottery_util');
var qh_redis = require("../qh_redis");
var log = require("../logger");
var config = require("../config");

function make_lottery_lt_response(calls, invitecode, now_num, res) {
    log.logger.trace("begin make_lottery_response");
    var fs = require('fs');
    var Schema = require('protobuf').Schema;
    var schema = new Schema(fs.readFileSync('proto/lottery.desc'));
    var LotteryRes = schema['qihoo.protocol.lottery.LotteryResponse'];


    var  qlt = new Object();

    if (undefined != calls) {
        qlt.lottery = {type : 'CALLS', calls: calls};
    } else if (undefined != invitecode) {
        qlt.lottery = {type : 'INVITE_CODE', invitecode: invitecode};
    } else {
        qlt.lottery = {type : 'NO', nomsg : '木有中奖，再来一次'};
    }
    qlt.limit = config.lotteryLimit;
    qlt.num = (config.lotteryLimit - now_num) > 0 ? (config.lotteryLimit - now_num) : 0; 

    log.logger.info("lottery response:" + JSON.stringify(qlt));
    res.send(lt_util.encode_response(LotteryRes.serialize(qlt)));
}

function reward_call(pn) {
    qh_redis.redis_get(lt_util.day_total_calls_key(pn), function(error, rTotalCalls) {
        if (error) {
            log.logger.error("get total call from redis error:" + error);
            return undefined;
        }
        // 今天的话费送完了
        if (rTotalCalls >= config.totalCalls) {
            return undefined;
        }

        var calls = lt_util.random_calls(config.maxCallsn);
        calls = (config.totalCalls - rTotalCalls) > calls ? calls : (config.totalCalls - rTotalCalls);

        qh_redis.redis_incrby(lt_util.day_total_calls_key(pn), calls, function(error, totaln) {
            if (error) {
                log.logger.error("incre total call from redis error:" + error);
                return undefined;
            }
            return calls;
        });

    });
}

function reward_invitecode(pn) {
    // TODO
    return "12345";
}

function lottery_lt(pn, res) {
    log.logger.trace("begin lottery, pn:" + pn);
   
    // 查看这个今天的摇奖次数
    qh_redis.redis_get(lt_util.day_lottery_limit_key(pn), function(error, now_num) {
        log.logger.debug("now_num:" + now_num);

        // Redis错误或者摇奖次数用完了，直接返回客户端没有中奖
        if (error || now_num >= config.lotteryLimit) {
            make_lottery_lt_response(undefined, undefined, now_num, res);
            return;
        }

        var calls = undefined, invitecode = undefined;
        // 按照概率，没有中奖
        if (false === lt_util.is_reward(config.lotteryPercentage)) {
            log.logger.debug("thank you, not reward");
        } else {
            // 通话分钟 和 邀请码 各 50% 的中奖概率。
            if (false === lt_util.is_reward(50)) {
                // 中奖，奖励分钟数
                calls = reward_call(pn);
            } else {
                // 中奖，奖励邀请码
                invitecode = reward_invitecode(pn);
            }
        }

        qh_redis.redis_incr(lt_util.day_lottery_limit_key(pn), function(error, incrn) {
            if (error) {
                log.logger.error("error:" + error);
                make_lottery_lt_response(calls, invitecode, now_num+1, res); 
            } else {
                make_lottery_lt_response(calls, invitecode, incrn, res);
            } 
        });
    });
}

exports.lottery_lt = lottery_lt;
