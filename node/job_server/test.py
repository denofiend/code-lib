import os
import sys
import urllib2
import json
import time
import redis
import requests
#import zlib
import StringIO
import gzip
from hashlib import sha1
 

class MyEncoder(json.JSONEncoder):
    def default(self,obj):
        #convert object to a dict
        d = {}
        d.update(obj.__dict__)
        return d
    
class MyDecoder(json.JSONDecoder):
    def __init__(self):
        json.JSONDecoder.__init__(self,object_hook=self.dict2object)
    def dict2object(self,d):
        #convert dict to object
        if'__class__' in d:
            class_name = d.pop('__class__')
            module_name = d.pop('__module__')
            module = __import__(module_name)
            class_ = getattr(module,class_name)
            args = dict((key.encode('ascii'), value) for key, value in d.items()) #get args
            inst = class_(**args) #create new instance
        else:
            inst = d
        return inst 

 
    
class InsertReqData:
    def __init__(self, version,surl,sname,shash,sprehash,spresize,relate_web,verify,ssize):
        self.version = version
        self.surl = surl
        self.sname = sname
        self.shash = shash
        self.sprehash = sprehash
        self.spresize = spresize
        self.relate_web = relate_web
        self.verify = verify
        self.ssize = ssize
                
class QueryReqData:
    def __init__(self, v,w):
        self.version = v
        self.web = w

class CorrectReqData:
    def __init__(self,version,web,rs_url,rs_hash,rs_size,errcode):
        self.version = version
        self.web = web
        self.rs_url = rs_url
        self.rs_hash = rs_hash
        self.rs_size = rs_size
        self.errcode = errcode
class Pageproperties:
    def __init__(self,url,insertTime):
        self.url = url
        self.insertTime = insertTime

class PageInfo:
    def __init__(self,totalScore,properties):
        self.totalScore = totalScore
        self.properties = properties



#urlinsert = "https://dlrecommend.maxthon.com/dlrecommend/insert"
#urlcorrect = "https://dlrecommend.maxthon.com/dlrecommend/correct"
#urlquery = "https://dlrecommend.maxthon.com/dlrecommend/query"

urlinsert = "http://127.0.0.1:8080/dlrecommend/insert"
urlcorrect = "http://127.0.0.1:8080/dlrecommend/correct"
urlquery = "http://127.0.0.1:8080/dlrecommend/query"
 
#redisHandle = redis.StrictRedis(host='localhost', port=6379, db=0)

def insert():
 
    version = "1"
    surl = "http://www.maxxthon.com/a.txxt"
    sname = "hello"
    shash = "jalsdkjfopiuewraaaaaaaa"
    sprehash = "alsdkjflasdkjfweaaaa"
    spresize = 22
    ssize = 100
    relate_web = "taaaaaaaaaaaaaaaaaaaaaaaaaaaaadskjf"
    shaObj = sha1('"%s"%s' % (relate_web , 'eUYhD8Vdl1TUWhiVP2P__GeC'))
    verify = shaObj.hexdigest()
    
    
    reqDataObj = InsertReqData(version,surl,sname,shash,sprehash,spresize,relate_web,verify,ssize)
    reqData = MyEncoder().encode(reqDataObj) 
    print reqData
    print len(reqData)
    '''
    dataSend = zlib.compress(reqData,1)
    undataSend = zlib.decompress(dataSend)
    '''
    out = StringIO.StringIO()
    f = gzip.GzipFile(fileobj=out, mode='w')
    f.write(reqData)
    f.close()
    dataSend = out.getvalue()
    print len(dataSend)
    #print undataSend
    
#    print "%s" % reqData ,"Content-Type": "application/json;charset=UTF-8"
    
    headers = {"Content-Encoding":"gzip","Accept-Encoding": "gzip"}
    res = requests.post(urlinsert,data = dataSend,headers = headers,verify=False)
    print "get result:%s" % res.text
    
      

    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)

def qurey():
    
    pageUrl = "http://page1.com"
    insertTime = "20130422"
    pageProperties = Pageproperties(pageUrl,insertTime)
    page1hash = '7d78f4187a6096db43c0878674349951fa93a86e'
    pageKey = 'dlrecmd:%s:page' % page1hash
    redisClinet.hset(pageKey,'totalScore',70)
    info = MyEncoder().encode(pageProperties) 
    print "page1 info:%s"  % info
    redisClinet.hset(pageKey,'info',info)
    
    source1Hash = "source1888888881"
    source2Hash = "source2888888882"
    page1SourceSetKey = "dlrecmd:%s:pagesources" % page1hash
    
    redisClinet.delete(page1SourceSetKey)
    
    redisClinet.zadd(page1SourceSetKey,50,source1Hash)
    redisClinet.zadd(page1SourceSetKey,20,source2Hash)
    
    source1InfoKey = 'dlrecmd:%s:source' % source1Hash
    source2InfoKey = 'dlrecmd:%s:source' % source2Hash
    
    redisClinet.hset(source1InfoKey,'name','source1')
    redisClinet.hset(source1InfoKey,'size','9990')
    redisClinet.hset(source1InfoKey,'totalScore',100)
    
    redisClinet.hset(source2InfoKey,'name','source2')
    redisClinet.hset(source2InfoKey,'size','8880')
    redisClinet.hset(source2InfoKey,'totalScore',20)
    
    source1Urlskey = 'dlrecmd:%s:sourceurls' % source1Hash
    redisClinet.zadd(source1Urlskey,20,'http://aa.txt')
    redisClinet.zadd(source1Urlskey,30,'http://bb.txt')

    source2Urlskey = 'dlrecmd:%s:sourceurls' % source2Hash
    redisClinet.zadd(source2Urlskey,20,'http://cc.txt')
    redisClinet.zadd(source2Urlskey,30,'http://dd.txt')
        
    version = "1"
    web = pageUrl
    reqDataObj = QueryReqData(version,web)
    reqData = MyEncoder().encode(reqDataObj) 
    print reqData    
    headers = {"Content-Type": "application/json;charset=UTF-8"}
    res = requests.post(urlquery,data = reqData,headers = headers,verify=False)
    print "get result:%s" % res.text
    
    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)
        
    redisClinet.zadd(page1SourceSetKey,50,source2Hash)
    redisClinet.hset(pageKey,'totalScore',100)
    res = requests.post(urlquery,data = reqData,headers = headers,verify=False)
    print "get result:%s" % res.text
    
    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 3:
        print "response error"
        sys.exit(-1)    
        
    redisClinet.zadd(page1SourceSetKey,100,source2Hash)
    redisClinet.hset(pageKey,'totalScore',150)
    res = requests.post(urlquery,data = reqData,headers = headers,verify=False)
    print "get result:%s" % res.text
    
    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)   

def correct():
    version = "1"
    web = "alsdkjf;lasdkjf;"
    rs_url = "alsdkjf;lasdkjf;"
    rs_hash = "alsdkjf;lasdkjf;"
    rs_size = "alsdkjf;lasdkjf;"
    errcode = "11"
    reqDataObj = CorrectReqData(version,web,rs_url,rs_hash,rs_size,errcode)
    reqData = MyEncoder().encode(reqDataObj)
    print reqData
    headers = {"Content-Type": "application/json;charset=UTF-8"}
    res = requests.post(urlcorrect,data = reqData,headers = headers,verify=False)
    print "get result:%s" % res.text

    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)
        
###########################################

def insertperf(output):
    version = "1"
    surl = "http://www.maxxthon.com/a.txxt"
    sname = "hello"
    shash = "jalsdkjfopiuewraaaaaaaa"
    sprehash = "alsdkjflasdkjfweaaaa"
    spresize = 22
    ssize = 100
    relate_web = "taaaaaaaaaaaaaaaaaaaaaaaaaaaaadskjf"
    shaObj = sha1('"%s"%s' % (relate_web , 'eUYhD8Vdl1TUWhiVP2P__GeC'))
    verify = shaObj.hexdigest()
    
    
    reqDataObj = InsertReqData(version,surl,sname,shash,sprehash,spresize,relate_web,verify,ssize)
    reqData = MyEncoder().encode(reqDataObj) 

    '''
    dataSend = zlib.compress(reqData,1)
    undataSend = zlib.decompress(dataSend)
    '''
    out = StringIO.StringIO()
    f = gzip.GzipFile(fileobj=out, mode='w')
    f.write(reqData)
    f.close()
    dataSend = out.getvalue()

    #print undataSend
    
#    print "%s" % reqData ,"Content-Type": "application/json;charset=UTF-8"
    
    headers = {"Content-Encoding":"gzip","Accept-Encoding": "gzip"}
    start = time.time()
    res = requests.post(urlinsert,data = dataSend,headers = headers)
    end = time.time()
    cost = end -start
    output.write("insertcost:%.3f" % cost)
    output.write( "\n")
    
      

    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)


def qureyperf(output):
            
    version = "1"
    web = "http://page1.com"
    reqDataObj = QueryReqData(version,web)
    reqData = MyEncoder().encode(reqDataObj) 
 
    headers = {"Content-Type": "application/json;charset=UTF-8"}
    start = time.time()
    res = requests.post(urlquery,data = reqData,headers = headers)
    end = time.time()
    cost = end -start
    output.write("querycost:%.3f" % cost)
    output.write( "\n")
    
    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)
        
  

def correctperf(output):
    version = "1"
    web = "alsdkjf;lasdkjf;"
    rs_url = "alsdkjf;lasdkjf;"
    rs_hash = "alsdkjf;lasdkjf;"
    rs_size = "alsdkjf;lasdkjf;"
    errcode = "11"
    reqDataObj = CorrectReqData(version,web,rs_url,rs_hash,rs_size,errcode)
    reqData = MyEncoder().encode(reqDataObj)

    headers = {"Content-Type": "application/json;charset=UTF-8"}
    start = time.time()
    res = requests.post(urlcorrect,data = reqData,headers = headers)
    end = time.time()
    cost = end -start
    output.write("correctcost:%.3f" % cost)
    output.write( "\n")

    resObj = MyDecoder().decode(res.text);
    if resObj["code"] != 0:
        print "response error"
        sys.exit(-1)
        
#redisClinet = redis.StrictRedis(host='10.100.15.7', port=6379, db=0)
redisClinet = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

if sys.argv[1] == "func":
    insert()
    qurey()
    correct()
    print 'test over successfully'
    sys.exit(0)
    

procNum = int(sys.argv[2])
loopNum = int(sys.argv[3])


print "procNum:%d,loopNum:%d" % (procNum,loopNum)

pageUrl = "http://page1.com"
insertTime = "20130422"
pageProperties = Pageproperties(pageUrl,insertTime)
page1hash = '7d78f4187a6096db43c0878674349951fa93a86e'
pageKey = 'dlrecmd:%s:page' % page1hash
redisClinet.hset(pageKey,'totalScore',70)
info = MyEncoder().encode(pageProperties) 
redisClinet.hset(pageKey,'info',info)

source1Hash = "source1888888881"
source2Hash = "source2888888882"
page1SourceSetKey = "dlrecmd:%s:pagesources" % page1hash

#redisClinet.delete(page1SourceSetKey)

redisClinet.zadd(page1SourceSetKey,50,source1Hash)
redisClinet.zadd(page1SourceSetKey,20,source2Hash)

source1InfoKey = 'dlrecmd:%s:source' % source1Hash
source2InfoKey = 'dlrecmd:%s:source' % source2Hash

redisClinet.hset(source1InfoKey,'name','source1')
redisClinet.hset(source1InfoKey,'size','9990')
redisClinet.hset(source1InfoKey,'totalScore',100)

redisClinet.hset(source2InfoKey,'name','source2')
redisClinet.hset(source2InfoKey,'size','8880')
redisClinet.hset(source2InfoKey,'totalScore',20)

source1Urlskey = 'dlrecmd:%s:sourceurls' % source1Hash
redisClinet.zadd(source1Urlskey,20,'http://aa.txt')
redisClinet.zadd(source1Urlskey,30,'http://bb.txt')

source2Urlskey = 'dlrecmd:%s:sourceurls' % source2Hash
redisClinet.zadd(source2Urlskey,20,'http://cc.txt')
redisClinet.zadd(source2Urlskey,30,'http://dd.txt')
        
        
num = 0
while num < procNum:
    pid = os.fork()    
    if(pid == 0):       
        fileName = "/home/liaoxixun/test/file%d" % num
        output = open(fileName,"w")  
        i = 0;
        while(i < loopNum):                     
            insertperf(output)
            qureyperf(output)
            correctperf(output)
            
            i = i + 1
        output.close()
        sys.exit(0)
    num = num + 1
    
try:
    while(1):
        child = os.wait()
        print "child %d returned,status:%d" % (child[0], child[1])
except OSError:
    print "all child returned"
 
