/*
 * AsyncTcpServer.h
 *
 *  Created on: 2011-3-2
 *      Author: zhaojiangang
 */

#ifndef __MXASIO_ASYNCTCPSERVER_H__
#define __MXASIO_ASYNCTCPSERVER_H__

#include <map>
#include <set>
#include <vector>
#include <mxcore/LoggerFactory.h>

#include <mxasio/IOService.h>
#include <mxasio/AsyncTcpApplication.h>

namespace mxasio
{

class AsyncTcpServer
#if !defined(WIN32) && !defined(SINGLE_PROCESS)
: public mxasio::IOApplicationAdaptor
#endif
{
	class ApplicationContext
	{
	private:
		ApplicationContext(const ApplicationContext&);
		ApplicationContext& operator =(const ApplicationContext&);
	public:
		ApplicationContext(int backlog, bool reuseAddr,
				const mxcore::SocketAddress& bindAddress,
				AsyncTcpApplication* application, void* attachment);
		~ApplicationContext(void);
	public:
		int backlog_;
		bool reuseAddr_;
		mxcore::SocketAddress bindAddress_;
		AsyncTcpApplication* application_;
		void* attachment_;

		int sock_;
	};
#if !defined(WIN32) && !defined(SINGLE_PROCESS)
	class Child
	{
	private:
		Child(const Child&);
		Child& operator =(const Child&);
	public:
		Child(void);
		~Child(void);
	public:
		pid_t pid_;
		ChannelId pipeChannelId_;
	};
#endif
public:
	class Config
	{
	public:
		Config(uint32_t uid, uint32_t subProcessNum);
		Config(const Config& src);
		Config& operator =(const Config& other);
		~Config(void);
	public:
		uint32_t uid_;
		uint32_t subProcessNum_;
	};

	enum
	{
		eClosed, eOpen, eReopen, eRunning, eShutdowning, eShutdown
	};

	AsyncTcpServer(void);

	~AsyncTcpServer(void);

	void open(void) throw (mxcore::Exception&);

	void reopen(void) throw (mxcore::Exception&);

	void run(void) throw (mxcore::Exception&);

	void shutdown(void);

	void setConfig(AsyncTcpServer::Config& config);

	int getState(void) const;

	mxasio::IOService* getIOService(void) const;

	void
	registerTcpApplication(const mxcore::SocketAddress& bindAddr, int backlog,
			bool reuseAddr, AsyncTcpApplication* application, void* attachment);
#if !defined(WIN32) && !defined(SINGLE_PROCESS)
	virtual void
	onConnect(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment);

	virtual void
	onConnectError(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment, int errorCode);

	virtual void
	onMsgRead(
			mxasio::IOService* service,
			const mxasio::ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool& callClosed);

	virtual void
	onMsgReadError(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode);

#endif
private:
	void cleanup(void);

	void closeApplicationsSock(void);

	void openApplicationsSock(void) throw (mxcore::IOException&);

	void registerAll(void) throw (mxcore::IOException&);

	ApplicationContext* findApplication(const std::string& key) const;
#if !defined(WIN32) && !defined(SINGLE_PROCESS)
	void childMain(uint32_t index, int pipeSock_);

	void initChildren(void);

	void cleanChildren(void);

	bool makeChildProcesses(void);

	pid_t makeChildProcess(uint32_t index);

	void setupSingle(void);
#endif
	void runImpl(bool isMain = false);
private:
	int state_;
	Config config_;
	mxasio::IOService* ioService_;

	std::set<AsyncTcpApplication*> applications_;

	typedef std::map<std::string, ApplicationContext*> ApplicationMap;
	ApplicationMap applicationMap_;
#if !defined(WIN32) && !defined(SINGLE_PROCESS)
	std::vector<Child*> childrens_;
#endif
};

inline int AsyncTcpServer::getState(void) const
{
	return state_;
}

inline mxasio::IOService* AsyncTcpServer::getIOService(void) const
{
	return ioService_;
}

} // namespace mxasio

#endif /* __MXASIO_ASYNCTCPSERVER_H__ */
