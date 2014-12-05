/**
 * New node file
 */

/* redis config */
exports.redisHost = "127.0.0.1"
exports.redisPort = 6379
exports.keyTimeOut = 30*24*60*60


/* log config */
exports.serverLogPath = "/data/logs/lottery.log"
exports.chukwaLogPath = "data/logs/chukwa.log"
exports.logLevel = "trace"

/* 中奖概率 */
exports.lotteryPercentage = 50; // 50%

/* 一个用户一天摇奖次数 */
exports.lotteryLimit = 3;	// 

/* 一天送出的总分钟数 */
exports.totalCalls = 3;	// 

/* 一次送分钟的最大值*/
exports.maxCalls = 50;
exports.maxCallsn = 8; // = (maxCalls - 10) / 5


/* rc4 key */
exports.rc4Key = "";
