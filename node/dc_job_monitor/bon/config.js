/* redis config */
exports.redisHost = "10.0.6.41"
exports.redisPort = 11255


/* log config */
exports.serverLogPath = "/data/logs/mx_word_filter/mx_word_filter.log"
exports.chukwaLogPath = "/data/logs/mx_word_filter/chukwa.log"
exports.logLevel = "trace"

/* other config */
exports.server_port = 8787;
exports.proportionThreshold = 60
exports.timesThreshold = 10
exports.verifyKey = 'eUYhD8Vdl1TUWhiVP2P__GeC'


/* lang adapter task config*/
exports.langTaskCrontab = "1 * * * * *"
exports.langSelectSql = "SELECT `langad`, `langios` FROM `dc_lang_adapter_tb`"
exports.langKey = "dc_lang_adapter_list"
exports.langMysqlSelectEvent = "langMysqlSelectEvent"

/* country adapter task config*/
exports.countryTaskCrontab = "1 * * * * *"
exports.countrySelectSql = "SELECT `countryad`, `countryios` FROM `dc_country_adapter_tb`"
exports.countryKey = "dc_country_adapter_list"
exports.countryMysqlSelectEvent = "countryMysqlSelectEvent"

/* channlelist task config*/
exports.channlelistTaskCrontab = "1 * * * * *"
exports.channlelistSelectSql = "SELECT `lang`, `country`, `sourcetype`, `channleid`, `version`, `updatetime`, `channlelist`, `channlelistversion`, `ifdefault` FROM `dc_channel_list_tb`"
exports.channlelistKey = "dc_channel_list_infos"
exports.channlelistMysqlSelectEvent = "channlelistMysqlSelectEvent"


/* sync task config*/
exports.syncTaskCrontab = "1 * * * * *"
exports.syncIndexSelectSql = "SELECT `source`, `rss_max_id`, `done_max_id`, `done_min_id` FROM `dc_index_job_tb`"

exports.syncIndexMysqlSelectEvent = "syncIndexMysqlSelectEvent"
exports.syncIndexRedisKey = "dc_index_job_list:"

exports.syncDoneJobSelectSql = "SELECT `status` FROM `dc_task_status_tb` WHERE `source` = '?' AND `id` = ?"
exports.syncDoneJobSelectMysqlSelectEvent = "syncDoneJobSelectMysqlSelectEvent"

exports.syncBodySelectSql = "SELECT `summary` FROM `dc_done_job_body_tb` WHERE `source` = '?' AND `id` = ?"
exports.syncBodySelectMysqlSelectEvent = "syncBodySelectMysqlSelectEvent"
exports.syncBodyRedisKey = "dc_done_job_body_cache"

exports.syncImgSelectSql = "SELECT `imgurl`, `imgsize` FROM `dc_done_job_img_tb` WHERE `source` = '?' AND `id` = ?"
exports.syncImgSelectMysqlSelectEvent = "syncImgSelectMysqlSelectEvent"
exports.syncImgKey = "dc_img:"


exports.syncBaseInfoSelectSql = "SELECT `title`, `pushdate`, `url`, `ifimg`, `lang`, `updatetime`, `contenttype`, `status`, `hashcode`, `description`, `source`, `id` FROM `dc_done_job_tb` WHERE `source` = '?' AND `id` = ?"
exports.syncBaseInfoSelectMysqlSelectEvent = "syncBaseInfoSelectMysqlSelectEvent"
exports.syncBaseInfoRedisKey = "dc_done_job_base_cache";

/* db config*/
exports.db_host = "10.0.6.247"
exports.db_user = "DataCenter"
exports.db_pwd = "DataCenter"
exports.database = "DataCenter"

