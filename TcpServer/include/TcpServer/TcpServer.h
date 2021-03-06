#pragma once

#include <stddef.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <memory>
#include <functional>

#include "EPoll.h"
#include "TcpSocket.h"

class TcpServer
{
    public:
        static const constexpr int LISTEN_BACKLOG = 5;
        typedef std::function<bool(TcpSocket& socket)> ConnectionCallback;
        typedef std::function<bool(int timerId, uint64_t expirationCount)> TimerCallback;
        typedef size_t ListenerHandle;

    public:
        TcpServer() = default;
        TcpServer(const TcpServer& other) = delete;
        ~TcpServer() = default;

        bool Listen(uint16_t port);
        int Poll(int timeout_ms);
        void Broadcast(const void *buf, size_t count, bool more);
        void Broadcast(std::string s);
        ListenerHandle AddConnectionEstablishedListener(ConnectionCallback listener);
        ListenerHandle AddConnectionClosedListener(ConnectionCallback listener);
        ListenerHandle AddDataAvailableListener(ConnectionCallback listener);
        ListenerHandle AddTimerListener(TimerCallback listener);
        void RemoveListener(ListenerHandle listenerHandle);
        EPoll& GetEPoll();

        int AddIntervalTimer(uint64_t interval_us);

    private:
        EPoll _epoll;
        std::map<int, TcpSocket> _serverSockets;
        std::map<int, TcpSocket> _clientSockets;
        std::map<ListenerHandle, ConnectionCallback> _connectionEstablishedListeners;
        std::map<ListenerHandle, ConnectionCallback> _connectionClosedListeners;
        std::map<ListenerHandle, ConnectionCallback> _dataAvailableListeners;
        std::map<ListenerHandle, TimerCallback> _timerListeners;
        std::vector<int> _timerHandles;
        ListenerHandle _nextListenerHandle = 0;

        bool EpollEvent(const epoll_event& ev);
        bool ServerSocketEvent(TcpSocket& socket, uint32_t events);
        void RemoveServerSocket(TcpSocket& socket);
        bool ClientSocketEvent(TcpSocket& socket, uint32_t events);
        void TimerEvent(int timerId, uint64_t expirationCount);
        void RemoveClientSocket(TcpSocket& socket);
        void MakeConnectionCallback(std::map<ListenerHandle, ConnectionCallback>& callbackMap, TcpSocket& socket);

        ListenerHandle MakeListenerHandle();

};
