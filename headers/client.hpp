#pragma once

#include <boost/asio.hpp>
#include <mutex>
#include <functional>
#include <queue>
#include <iostream>
#include <atomic>

#include "strmanip.hpp"
#include "server_settings.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using msg_handler = function<void(string)>;

// Implements client-side functionality. Can write and read messages
// between client and server. And of course start and stop the client-side program
class Client final
{
private:
    strmanip m_shared_buffer;

    io_context m_io_context;
    tcp::socket m_sock;
    tcp::resolver::results_type m_endpoints;
    asio::streambuf m_buf;
    queue<string> m_outgoing_msgs;
    mutex m_mutex;
    atomic<bool> m_state;

protected:
    void check_error(system::error_code __err);

    inline void async_read();
    void on_read(system::error_code __err, size_t __bytes_transfered);

    inline void async_write();
    void on_write(system::error_code __er);

    // Returns calulated sum of all digits in string as a string
    string processing_input_string(const string &__str);

    // Returns 'true' if string contains only from digits and doesn't exceed 64 symbols
    bool is_checked_input_string(const string &__str);

    // Returns sorted input string by descending order with all elements which values are even
    // replaced to "KB" symbols
    string transform_input_string(const string &__str);

public:
    msg_handler on_msg;

    explicit Client() = default;
    explicit Client(const string &__raw_IP, const ushort &__port_num);
    virtual ~Client() = default;

    void run();
    void stop();
    void run_in_thread();
    void send_msg(const string &__msg);

    inline bool get_state() const { return m_state.load(); }
    inline void set_state(const bool &__new_state) { m_state.store(__new_state); }
    inline void clear_shared_buffer() { m_shared_buffer.clear(); }

    // Implements 'complex_processing' method in infinity loop
    void input_and_checking_loop();

    // Outputs recieved data to terminal, calculating total sum of all
    // elements which are digits, then sending message to server with this sum
    // which is represents as a string
    void proccessing_shared_buffer();

    // Performs complex checking of input string and then if
    // the string matches the required template - sending message to server
    void complex_processing(const string &__str);
};
