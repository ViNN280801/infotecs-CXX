#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using msg_handler = function<void(string)>;
using err_handler = function<void()>;

// Implemetns TCP connection between client-side and 
// server-side with handling messages and errors
class TCP_Connection final : public std::enable_shared_from_this<TCP_Connection>
{
private:
    tcp::socket m_sock;
    string m_str;
    queue<string> m_outgoing_msgs;
    asio::streambuf m_buf;

    msg_handler m_msg_handler;
    err_handler m_err_handler;

protected:
    void check_error(system::error_code __err);

    // Wait for a new message from the client
    inline void async_read();
    void on_read(system::error_code __err, size_t __bytes_transfered);

    inline void async_write();
    void on_write(system::error_code __err, size_t __bytes_transfered);

public:
    explicit TCP_Connection() = default;
    explicit TCP_Connection(tcp::socket &&__sock_rhs);

    virtual ~TCP_Connection() = default;

    inline const string &get_str() const { return m_str; }
    tcp::socket &get_socket() { return m_sock; }
    void start(msg_handler &&__msg_handler, err_handler &&__err_handler);
    void send_msg(const string &__msg);
    inline static std::shared_ptr<TCP_Connection> create_connection(tcp::socket &&__sock_rhs);
};

void TCP_Connection::check_error(system::error_code __err)
{
    if (__err)
    {
        m_sock.close(__err);
        m_err_handler();

        return;
    }
}

void TCP_Connection::async_read()
{
    async_read_until(
        m_sock, m_buf, "\n",
        [self = shared_from_this()](system::error_code err, size_t bytes_transfered)
        {
            self->on_read(err, bytes_transfered);
        });
}

void TCP_Connection::on_read(system::error_code __err, size_t __bytes_transfered)
{
    check_error(__err);

    stringstream msg;
    msg << istream(&m_buf).rdbuf();
    m_buf.consume(__bytes_transfered);

    m_msg_handler(msg.str());

    async_read();
}

void TCP_Connection::async_write()
{
    asio::async_write(
        m_sock, buffer(m_outgoing_msgs.front()),
        [self = shared_from_this()](system::error_code err, size_t bytes_transfered)
        {
            self->on_write(err, bytes_transfered);
        });
}

void TCP_Connection::on_write(system::error_code __err, size_t __bytes_transfered)
{
    // To avoid: "maybe unused variable"
    (void)__bytes_transfered;

    check_error(__err);

    m_outgoing_msgs.pop();

    if (!m_outgoing_msgs.empty())
        async_write();
}

TCP_Connection::TCP_Connection(tcp::socket &&__sock_rhs) : m_sock(std::move(__sock_rhs))
{
    system::error_code err;
    stringstream ss;

    ss << m_sock.remote_endpoint();

    m_str = ss.str();
}

void TCP_Connection::start(msg_handler &&__msg_handler, err_handler &&__err_handler)
{
    m_msg_handler = std::move(__msg_handler);
    m_err_handler = std::move(__err_handler);

    async_read();
}

void TCP_Connection::send_msg(const string &__msg)
{
    bool is_idle{m_outgoing_msgs.empty()};
    m_outgoing_msgs.push(__msg);

    if (is_idle)
        async_write();
}

std::shared_ptr<TCP_Connection> TCP_Connection::create_connection(tcp::socket &&__sock_rhs)
{
    return std::shared_ptr<TCP_Connection>(new TCP_Connection(std::move(__sock_rhs)));
}
