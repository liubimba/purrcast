//
// Created by bimba on 12/21/25.
//

#include "MonitorSession.hpp"


MonitorSession::MonitorSession(MonitorSessionListener* listener, boost::asio::ip::tcp::socket socket, std::string id):
    ws_(std::move(socket)), listener_(listener), id_(std::move(id))
{
}

void MonitorSession::start()
{
    ws_.control_callback(
        [self = shared_from_this()](websocket::frame_type frameType, boost::beast::string_view data)
        {
            if (!self->listener_)
                return;
            if (frameType == websocket::frame_type::close)
                self->listener_->onClose(self, data);
        });
    ws_.async_accept(
        [self = shared_from_this()](boost::system::error_code ec)
        {
            if (!ec)
            {
                self->read_();
            }
        });
}

void MonitorSession::send(const std::string& msg)
{
    ws_.text(true);
    ws_.async_write(
        boost::asio::buffer(msg),
        [](boost::system::error_code, std::size_t)
        {
        });
}

void MonitorSession::close(websocket::close_reason reason)
{
    ws_.close(reason);
}

std::string MonitorSession::get_id() const
{
    return id_;
}

void MonitorSession::read_()
{
    ws_.async_read(
        buffer_,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t)
        {
            if (!ec)
            {
                self->buffer_.consume(self->buffer_.size());
                self->read_();
            }
        });
}
