/**
 * New node file
 */

exports.redisHost = "10.100.15.7"
exports.redisPort = 6379

exports.serverPort = 8080
exports.processNum = 1

exports.serverLogPath = "/data/logs/mx_word_filter/mx_word_filter.log"
exports.chukwaLogPath = "/data/logs/mx_word_filter/chukwa.log"
exports.logLevel = "trace"

exports.userDownLoadListsNum = 10
exports.userDownLoadFailListsNum = 10

exports.proportionThreshold = 60
exports.timesThreshold = 10
exports.verifyKey = 'eUYhD8Vdl1TUWhiVP2P__GeC'


/* lang adapter task config*/
exports.langTaskCrontab = "1 * * * * *"
exports.langSelectSql = "SELECT `langad`, `langios` FROM `dc_lang_adapter_tb`"
exports.langKey = "dc_lang_adapter_list"


/* db config*/
exports.db_host = "10.0.6.247"
exports.db_user = "DataCenter"
exports.db_pwd = "DataCenter"
exports.database = "DataCenter"








