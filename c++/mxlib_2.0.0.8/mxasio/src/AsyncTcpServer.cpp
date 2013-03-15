/*
 * AsyncTcpServer.cpp
 *
 *  Created on: 2011-3-2
 *      Author: zhaojiangang
 */

#include <mxcore/IOUtil.h>
#include <mxcore/SystemOps.h>
#include <mxcore/MutexLock.h>
#include <mxcore/ServerSocket.h>

#include <mxcore/SocketOps.h>
#include <mxasio/AsyncTcpServer.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

static mxcore::AutoMutexLock sTcpServerLock_;
static std::set<AsyncTcpServer*> sTcpServerSet_;

static mxcore::Logger& getLogger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("mxasio.tcpserv");
}

AsyncTcpServer::ApplicationContext::ApplicationContext(int backlog,
		bool reuseAddr, const mxcore::SocketAddress& bindAddress,
		AsyncTcpApplication* application, void* attachment) :
	backlog_(backlog), reuseAddr_(reuseAddr), bindAddress_(bindAddress),
			application_(application), attachment_(attachment), sock_(-1)
{
}

AsyncTcpServer::ApplicationContext::~ApplicationContext(void)
{
}

AsyncTcpServer::Config::Config(uint32_t uid, uint32_t subProcessNum) :
	uid_(uid), subProcessNum_(subProcessNum)
{
}

AsyncTcpServer::Config::Config(const Config& src) :
	uid_(src.uid_), subProcessNum_(src.subProcessNum_)
{
}

AsyncTcpServer::Config& AsyncTcpServer::Config::operator =(const Config& other)
{
	if (this != &other)
	{
		uid_ = other.uid_;
		subProcessNum_ = other.subProcessNum_;
	}
	return *this;
}

AsyncTcpServer::Config::~Config(void)
{
}

#if !defined(WIN32) && !defined(SINGLE_PROCESS)
AsyncTcpServer::Child::Child(void) :
pid_(-1)
{
}

AsyncTcpServer::Child::~Child(void)
{
}
#endif

AsyncTcpServer::AsyncTcpServer(void) :
	state_(eClosed), config_(0, mxos::getProcesserNum() * 2), ioService_(NULL)
{
	mxcore::Lock::ScopeLock sl(sTcpServerLock_);
	sTcpServerSet_.insert(this);
}

AsyncTcpServer::~AsyncTcpServer(void)
{
	{
		mxcore::Lock::ScopeLock sl(sTcpServerLock_);
		sTcpServerSet_.erase(this);
	}
	cleanup();
}

void AsyncTcpServer::open(void) throw (mxcore::Exception&)
{
	MX_ASSERT(eClosed == state_);

	try
	{
		state_ = eOpen;
		ioService_ = mxasio::IOService::createInstance();
		openApplicationsSock();
	} catch (mxcore::IOException& e)
	{
		state_ = eClosed;
		if (NULL != ioService_)
		{
			ioService_->close(true);
			delete ioService_;
			ioService_ = NULL;
		}

		throw e;
	}
}

void AsyncTcpServer::reopen(void) throw (mxcore::Exception&)
{
	MX_ASSERT(eOpen == state_);

	state_ = eReopen;

	ioService_->close(true);
	delete ioService_;
	closeApplicationsSock();

	try
	{
		ioService_ = mxasio::IOService::createInstance();
		openApplicationsSock();
	} catch (mxcore::IOException& e)
	{
		state_ = eClosed;
		if (NULL != ioService_)
		{
			ioService_->close(true);
			delete ioService_;
			ioService_ = NULL;
		}

		throw e;
	}
}

void AsyncTcpServer::cleanup(void)
{
	if (eClosed != state_)
	{
		state_ = eClosed;
		ioService_->close();

		while (!applications_.empty())
		{
			std::set<AsyncTcpApplication*>::const_iterator it =
					applications_.begin();

			AsyncTcpApplication* application = *it;
			applications_.erase(it);

			application->shutdown(this);
		}

		delete ioService_;

#if !defined(win32) && !defined(SINGLE_PROCESS)
		cleanChildren();
#endif
	}

	for (ApplicationMap::const_iterator it = applicationMap_.begin(); it
			!= applicationMap_.end(); it++)
	{
		delete it->second;
	}
	applicationMap_.clear();
}

void AsyncTcpServer::closeApplicationsSock(void)
{
	for (ApplicationMap::const_iterator it = applicationMap_.begin(); it
			!= applicationMap_.end(); it++)
	{
		ApplicationContext* applicationCtx = it->second;

		if (-1 != applicationCtx->sock_)
		{
			mxos::closeSocket(applicationCtx->sock_);
		}
	}
}

void AsyncTcpServer::openApplicationsSock(void) throw (mxcore::IOException&)
{
	try
	{
		for (ApplicationMap::const_iterator it = applicationMap_.begin(); it
				!= applicationMap_.end(); it++)
		{
			ApplicationContext* applicationCtx = it->second;

			applicationCtx->sock_ = -1;

			mxcore::ServerSocket ss;
			ss.listen(applicationCtx->bindAddress_, applicationCtx->backlog_,
					applicationCtx->reuseAddr_, false);
			applicationCtx->sock_ = ss.detach();
		}
	} catch (mxcore::IOException& e)
	{
		closeApplicationsSock();
		throw e;
	}
}

void AsyncTcpServer::registerAll(void) throw (mxcore::IOException&)
{
	for (ApplicationMap::const_iterator it = applicationMap_.begin(); it
			!= applicationMap_.end(); it++)
	{
		ApplicationContext* applicationCtx = it->second;

		ChannelId channelId = ioService_->listen(applicationCtx->application_,
				applicationCtx->sock_, applicationCtx->attachment_);
	}
}

void AsyncTcpServer::run(void) throw (mxcore::Exception&)
{
	MX_ASSERT(eOpen == state_ || eReopen == state_);
	state_ = eRunning;

#if !defined(win32) && !defined(SINGLE_PROCESS)

	initChildren();

	if (!makeChildProcesses())
	{
		THROW2(mxcore::Exception, "Failed to make child process");
	}

	setupSingle();
	runImpl(true);
#else
	registerAll();
	if (0 != config_.uid_)
	{
		::setuid(config_.uid_);
	}
	runImpl();
#endif
	state_ = eShutdown;

	cleanup();
}

void AsyncTcpServer::shutdown(void)
{
	if (eClosed != state_)
	{
		state_ = eShutdowning;

		ioService_->wakeup();
	}
}

void AsyncTcpServer::setConfig(AsyncTcpServer::Config& config)
{
	config_ = config;

	if (config_.subProcessNum_ <= 0)
	{
		config_.subProcessNum_ = mxos::getProcesserNum() * 2;
	}
}

void AsyncTcpServer::registerTcpApplication(
		const mxcore::SocketAddress& bindAddr, int backlog, bool reuseAddr,
		AsyncTcpApplication* application, void* attachment)
{
	MX_ASSERT(eClosed == state_);

	ApplicationContext* applicationCtx = new ApplicationContext(backlog,
			reuseAddr, bindAddr, application, attachment);

	std::string key = bindAddr.toString();

	MX_ASSERT(NULL == findApplication(key));

	applications_.insert(application);
	applicationMap_.insert(std::make_pair(key, applicationCtx));
}

#if !defined(WIN32) && !defined(SINGLE_PROCESS)

void AsyncTcpServer::onConnect(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment)
{
	mxcore::SharedPtr<mxcore::ByteBuffer> message = new mxcore::ByteBuffer(128);
	service->recv(id, message, 0, false);
}

void AsyncTcpServer::onConnectError(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment, int errorCode)
{
	service->closeChannel(id);
}

void AsyncTcpServer::onMsgRead(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
		bool& callClosed)
{
	MX_ASSERT(messageList.size() == 1);

	mxcore::SharedPtr<mxcore::ByteBuffer> message = messageList.front();
	message->clear();
	service->recv(id, message, 0, false);
}

void AsyncTcpServer::onMsgReadError(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode)
{
	if (NULL == attachment)
	{
		// 如果attachment为NULL,则是在子进程中
		// 此种情况说明父进程退出了，此时子进程需要退出
		shutdown();

		getLogger().debug("Parent term, this child will exit\n");
	}
	else
	{
		uint32_t index = (uint32_t) (long) attachment;
		index -= 1;

		if (eRunning == state_)
		{
			if (index >= 0 && index < (uint32_t) childrens_.size())
			{
				pid_t oldPid = childrens_[index]->pid_;
				childrens_[index]->pid_ = -1;

				pid_t newPid = makeChildProcess(index);

				getLogger().debug("Child: [%d] term, new Child: [%d]\n",
						(int) oldPid, (int) newPid);
			}
		}
	}
}

#endif

AsyncTcpServer::ApplicationContext* AsyncTcpServer::findApplication(
		const std::string& key) const
{
	ApplicationMap::const_iterator it = applicationMap_.find(key);
	if (it != applicationMap_.end())
	{
		return it->second;
	}
	return NULL;
}

#if !defined(WIN32) && !defined(SINGLE_PROCESS)
void AsyncTcpServer::childMain(uint32_t index, int pipeSock_)
{
	try
	{
		// 关闭所有子进程相关的数据
		//		for (uint32_t i = 0; i < childrens_.size(); i++)
		//		{
		//			if (childrens_[i]->pid_ != -1)
		//			{
		//				if (!ioService_->isChannelClosed(childrens_[i]->pipeChannelId_))
		//				{
		//					ioService_->closeChannel(childrens_[i]->pipeChannelId_,
		//							true, true);
		//				}
		//			}
		//		}
		//		delete ioService_;
		ioService_ = mxasio::IOService::createInstance();

		// 注册所有applicatoin
		registerAll();
		// 注册pipe
		ioService_->connect(this, pipeSock_, NULL);

		if (0 != config_.uid_)
		{
			::setuid(config_.uid_);
		}
		runImpl();
	}
	catch (mxcore::IOException& e)
	{
		getLogger().error("Child: [%d] exception: [%s] [%s:%d]\n",
				(int) mxos::getPid(), e.getMessage().c_str(),
				e.getFilename().c_str(), e.getLineNumber());
	}

	exit(0);
}

void AsyncTcpServer::initChildren(void)
{
	for (uint32_t i = 0; i < config_.subProcessNum_; i++)
	{
		Child* child = new Child;
		childrens_.push_back(child);
	}
}

void AsyncTcpServer::cleanChildren(void)
{
	for (uint32_t i = 0; i < (uint32_t) childrens_.size(); i++)
	{
		delete childrens_[i];
	}
	childrens_.clear();
}

bool AsyncTcpServer::makeChildProcesses(void)
{
	for (uint32_t i = 0; i < (uint32_t) childrens_.size(); i++)
	{
		if (-1 == makeChildProcess(i))
		{
			return false;
		}
	}
	return true;
}

pid_t AsyncTcpServer::makeChildProcess(uint32_t index)
{
	int socketPair[2];

	try
	{
		mxcore::IOUtil::createSocketPair(socketPair);

		pid_t pid = fork();

		if (0 == pid)
		{
			::signal(SIGTERM, NULL);
			::signal(SIGQUIT, NULL);
			::signal(SIGINT, NULL);
			::signal(SIGKILL, NULL);
			::signal(SIGCHLD, NULL);
			::signal(SIGALRM, NULL);
			::signal(SIGPIPE, NULL);
			mxos::closeSocket(socketPair[0]);
			childMain(index, socketPair[1]);
			return 0;
		}
		else if (pid > 0)
		{
			::mxos::closeSocket(socketPair[1]);
			childrens_[index]->pipeChannelId_ = ioService_->connect(this,
					socketPair[0], (void*) (index + 1));
			childrens_[index]->pid_ = pid;
			return pid;
		}
		else
		{
			mxos::closeSocket(socketPair[0]);
			mxos::closeSocket(socketPair[1]);
			return -1;
		}
	}
	catch (mxcore::IOException& e)
	{
		return -1;
	}
}

static void sig_pipe(int signo)
{
}

static void sig_alarm(int signo)
{
	static int sWorkingHour = 0;

	getLogger().info(
			"Server have been working %d hours\n", ++sWorkingHour);

	::alarm(3600);
}

static void sig_int(int signo)
{
	mxcore::Lock::ScopeLock sl(sTcpServerLock_);
	for (std::set<AsyncTcpServer*>::iterator it = sTcpServerSet_.begin(); it
			!= sTcpServerSet_.end(); it++)
	{
		AsyncTcpServer* tcpServer = *it;
		tcpServer->shutdown();
	}
	sTcpServerSet_.clear();
}

static void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
	}
}

void AsyncTcpServer::setupSingle(void)
{
	::signal(SIGTERM, sig_int);

	// 捕捉 "CTRL + \"
	::signal(SIGQUIT, sig_int);

	// 捕捉 "CTRL + C"
	::signal(SIGINT, sig_int);

	// 捕捉kill
	::signal(SIGKILL, sig_int);

	// 子进程状态改变，一般是子进程退出
	::signal(SIGCHLD, sig_chld);

	// 每小时执行一次的任务
	::signal(SIGALRM, sig_alarm);

	// 捕捉信号SIGPIPE,
	::signal(SIGPIPE, sig_pipe);

	::alarm(3600);
}
#endif

void AsyncTcpServer::runImpl(bool isMain)
{
	try
	{
		while (eRunning == state_)
		{
			if (!isMain)
			{
				for (ApplicationMap::const_iterator it =
						applicationMap_.begin(); it != applicationMap_.end(); it++)
				{
					it->second->application_->beforeSelect(this);
				}
			}
			ioService_->select();
			if (!isMain)
			{
				for (ApplicationMap::const_iterator it =
						applicationMap_.begin(); it != applicationMap_.end(); it++)
				{
					it->second->application_->afterSelect(this);
				}
			}
		}
	} catch (mxcore::Exception& e)
	{
		getLogger().error2(e.getErrorCode(), "%s %s:%d\n",
				e.getMessage().c_str(), e.getFilename().c_str(),
				e.getLineNumber());
	}
}

} // namespace mxasio
