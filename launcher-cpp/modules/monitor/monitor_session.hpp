//
// Created by bimba on 12/21/25.
//

#ifndef IPCSESSION_HPP
#define IPCSESSION_HPP
#include <deque>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/websocket/detail/mask.hpp>
#include "monitor_session_listener.hpp"

namespace websocket = boost::beast::websocket;

class monitor_session : public std::enable_shared_from_this<monitor_session>
{
public:
    explicit monitor_session(monitor_session_listener* listener, boost::asio::ip::tcp::socket socket, std::string id);
    void start();
    void send(const std::string& msg);
    void close(websocket::close_reason reason);
    std::string get_id() const;

private:
    void read_();

    std::string id_;
    boost::beast::flat_buffer buffer_;
    monitor_session_listener* listener_;
    websocket::stream<boost::asio::ip::tcp::socket> ws_;
};


#endif //IPCSESSION_HPP
