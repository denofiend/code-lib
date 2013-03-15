/*
 *        Name: IOApplication.h
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_IOAPPLICATION_H__
#define __MXASIO_IOAPPLICATION_H__

#include <list>
#include <mxasio/ChannelId.h>
#include <mxcore/SharedPtr.h>
#include <mxcore/ByteBuffer.h>

namespace mxasio
{

class IOService;

class IOApplication
{
public:
	virtual ~IOApplication(void)
	{
	}

	static const int ERR_TIMEOUT = -2;
	static const int ERR_EOF = -1;
	static const int ERR_OK = 0;

	/**
	 * 当某个Channel被关闭的时候由IOService调用，说明id为给定id的Channel已经
	 * 被关闭了，应用层应该清理这个Channel相关的资源， IOService只有在关闭的时候
	 * 才会主动调用这个函数，否则这个调用都是由应用程序调用IOServer::closeChannel
	 * 函数，然后再由IOService触发的，所以当某个Channel出错的时候IOService只
	 * 负责通知应用层该Channel出错了，并不会主动关闭这个Channel
	 *
	 * @param service
	 * 	该channel注册的service
	 *
	 * @param id
	 * 	被关闭的channel的id
	 *
	 * @param attachment
	 *  注册该channel时应用层传递的应用相关信息
	 *
	 * @param isAcceptor
	 *  如果为true则说明这个Channel是一个ServerSocketChannel，否则是一个SocketChannel
	 */
	virtual void onClose(IOService* service, const ChannelId& id,
			void* attachment, bool isAcceptor) = 0;

	/**
	 * 当某个ServerSocketChannel上有新的客户端连接过来，IOService会调用该函数通知应用层，
	 * newChannelId参数则是新客户端的ChannelId，应用层需要为这个新的Channel返回和这个Channel
	 * 相关的void*数据，如果应用层处理过程中出错或者不想接受这个新的连接，那么应用层必须调用
	 * IOService::closeChannel函数，把newChannelId传给该函数，并把callClosed置为true，
	 * 说明应用层已经close了这个Channel,IOService不需要再处理这个Channel了
	 *
	 * @param service
	 *  该应用注册到的IOService
	 *
	 * @param id
	 *  ServerSocketChannel的id
	 *
	 * @param attachment
	 *  ServerSocketChannel的应用相关信息
	 *
	 * @param newChannelId
	 *  新建立的SocketChannel的id
	 *
	 * @param callClosed
	 *  用于标识应用层是否关闭了这个新SocketChannel，如果应用层再此函数中调用了
	 *  IOService::closeChannel(newChannelId) 则必须把callClosed置为true
	 */
	virtual void
	* onNewClient(IOService* service, const ChannelId& id, void* attachment,
			const ChannelId& newChannelId, bool& callClosed) = 0;

	/**
	 * 当一个SocketChannel(调用IOService::connect函数生成的)成功connect到远程的服务器的
	 * 时候IOService会调用此函数来通知应用层，应用层应该在此函数中做出相应的动作(发送或接收)
	 */
	virtual void
	onConnect(IOService* service, const ChannelId& id, void* attachment) = 0;

	/**
	 * 当一个SocketChannel connect失败的时候IOService会调用此函数来通知应用层，
	 * errorCode为connect失败的错误码
	 */
	virtual void
	onConnectError(IOService* service, const ChannelId& id, void* attachment,
			int errorCode) = 0;

	/**
	 * 当一个SocketChannel上读取到数据后IOService会调用此函数来通知应用层，messageList是
	 * 从该SocketChannel上读取到的消息列表，列表中的对象都是应用层调用IOService::recv函数
	 * 是传递给IOService的，如果应用层决定要关闭这个SocketChannel则需要调用IOService::
	 * closeChannel函数，并且必须把callClosed的置为true
	 */
	virtual void
			onMsgRead(
					IOService* service,
					const ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool& callClosed) = 0;

	/**
	 * 当从一个SocketChannel上读取数据出错时IOService会嗲用该函数来通知应用层，message是正要
	 * 读取的消息对象，errorCode为错误码
	 */
	virtual void
	onMsgReadError(IOService* service, const ChannelId& id, void* attachment,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode) = 0;

	/**
	 * 当向SocketChannel上发送完一些message时会调用此函数来通知应用层，isHungry如果为true表示
	 * 该SocketChannel上已经没有数据要发送了，如果应用层决定要关闭这个SocketChannel则需要调用IOService::
	 * closeChannel函数，并且必须把callClosed的置为true
	 */
	virtual void
			onMsgWrite(
					IOService* service,
					const ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool isHungry, bool& callClosed) = 0;

	/**
	 * 当向SocketChannel上发送message出错时会调用此函数来通知应用层，errorCode为错误码
	 */
	virtual void
	onMsgWriteError(IOService* service, const ChannelId& id, void* attachment,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode) = 0;
};

class IOApplicationAdaptor: public IOApplication
{
public:
	virtual void onClose(IOService* service, const ChannelId& id,
			void* attachment, bool isAcceptor)
	{
	}

	virtual void * onNewClient(IOService* service, const ChannelId& id,
			void* attachment, const ChannelId& newChannelId, bool& callClosed)
	{
		return NULL;
	}

	virtual void onConnect(IOService* service, const ChannelId& id,
			void* attachment)
	{
	}

	virtual void onConnectError(IOService* service, const ChannelId& id,
			void* attachment, int errorCode)
	{
	}

	virtual void onMsgRead(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool& callClosed)
	{
	}

	virtual void onMsgReadError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
	}

	virtual void onMsgWrite(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool isHungry, bool& callClosed)
	{
	}

	virtual void onMsgWriteError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
	}
};

} // namespace mxasio

#endif /* __MXASIO_IOAPPLICATION_H__ */
