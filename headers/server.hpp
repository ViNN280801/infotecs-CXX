#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <optional>
#include <unordered_set>
#include <iostream>

#include "strmanip.hpp"
#include "server_settings.hpp"
#include "TCP_Connection.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using on_join_handler = std::function<void(std::shared_ptr<TCP_Connection>)>;
using on_leave_handler = on_join_handler;
using on_client_msg_handler = std::function<void(std::string)>;

// Responsible for handling a single client by reading the 
// request message, processing it, and then sending back the
// response message. Server class has client handlers
class Server final
{
private:
    io_context m_io_context;
    tcp::acceptor m_acceptor;
    unordered_set<std::shared_ptr<TCP_Connection>> m_connections;
    optional<tcp::socket> m_sock;

protected:
    void start_accept();

public:
    on_join_handler on_join;
    on_leave_handler on_leave;
    on_client_msg_handler on_client_msg;

    explicit Server() = default;
    explicit Server(const ushort &__port_num) : m_acceptor(m_io_context,
                                                           tcp::endpoint(
                                                               address_v4::any(),
                                                               __port_num)){};
    virtual ~Server() = default;

    int run();
    void broadcast(const string &__msg);

    // Checks if number contains of 3 more digits and is number multiple of 32
    // Returns relevant messages
    string analyze_msg_from_client(const string &__msg) const;
};
