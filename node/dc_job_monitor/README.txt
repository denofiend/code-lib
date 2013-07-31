#配置说明：
./bon/conf.json
{
    "server": {
        "port": 8080, //服务启动端口
        "api-ps-push-s": "http://10.0.16.106", //需要访问的push api服务器
        "cs-s": "https://cs-s.maxthon.com" //需要访问的云存储服务器
    },
    "redis":{
        "transfile-s":{ //transfile服务器使用的redis配置
            "host": "10.0.6.114",
            "port": 8702,
            "options": {"detect_buffers":true}
        },
        "device-s":{ //device服务器使用的redis配置
            "host": "10.0.11.100",
            "port": 16379,
            "options": {"detect_buffers":false}
        }
    }
}

#部署 node >= v0.6.19
建议使用forever启动node进程


