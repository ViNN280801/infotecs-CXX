#include <iostream>

#include "../../headers/client.hpp"

void Client::check_error(system::error_code __err)
{
    if (__err)
    {
        stop();
        return;
    }
}

void Client::async_read()
{
    async_read_until(
        m_sock,
        m_buf,
        "\n",
        [this](system::error_code err, size_t bytes_transfered)
        { on_read(err, bytes_transfered); });
}

void Client::on_read(system::error_code __err, size_t __bytes_transfered)
{
    check_error(__err);

    stringstream msg;
    msg << istream{&m_buf}.rdbuf();
    m_buf.consume(__bytes_transfered);

    on_msg(msg.str());

    async_read();
}

void Client::async_write()
{
    m_mutex.lock();

    asio::async_write(
        m_sock,
        buffer(m_outgoing_msgs.front()),
        [this](system::error_code err, size_t bytes_transfered)
        {
            // To avoid "maybe unused variable"
            (void)bytes_transfered;

            on_write(err);
        });

    m_mutex.unlock();
}

void Client::on_write(system::error_code __err)
{
    check_error(__err);

    m_outgoing_msgs.pop();

    if (!m_outgoing_msgs.empty())
        async_write();
}

string Client::processing_input_string(const string &__str)
{
    m_mutex.lock();

    m_shared_buffer = __str;
    cout << m_shared_buffer << endl;
    uint sum{m_shared_buffer.sum_of_digits()};
    clear_shared_buffer();

    m_mutex.unlock();

    return to_string(sum);
}

bool Client::is_checked_input_string(const string &__str)
{
    m_shared_buffer = __str;
    m_shared_buffer.remove_endl();

    return (m_shared_buffer.is_digits() && m_shared_buffer.is_less_or_64symbols()) ? true : false;
}

string Client::transform_input_string(const string &__str)
{
    m_mutex.lock();

    m_shared_buffer = __str;

    m_shared_buffer.sort_descending();
    m_shared_buffer.even_to_KB();

    m_mutex.unlock();

    return m_shared_buffer.get_str();
}

void Client::complex_processing(const string &__str)
{
    string copy{__str};

    // Checks if string contains only of digits and doesn't exceed 64 symbols
    if (is_checked_input_string(copy))
    {
        // Sorting by descending order and replacing all even digits to latin 'KB' symbols
        copy = transform_input_string(copy);

        // 'copy' string fits into the shared buffer
        m_shared_buffer = copy;
    }
    else
        cout << "The string does not match the template."
             << "String must contains only of digits and doesn't exceed 64 symbols."
             << endl;
}

Client::Client(const string &__raw_IP, const ushort &__port_num)
    : m_sock(m_io_context), m_state(true)
{
    ip::tcp::resolver resolver{m_io_context};
    m_endpoints = resolver.resolve(__raw_IP, to_string(__port_num));
}

void Client::run()
{
    async_connect(
        m_sock, m_endpoints,
        [this](system::error_code err, tcp::endpoint endpoint)
        {
            // To avoid "maybe unused variable"
            (void)endpoint;

            if (!err)
                async_read();
        });

    m_io_context.run();
}

void Client::stop()
{
    system::error_code err;
    m_sock.close(err);
}

void Client::run_in_thread()
{
    thread{[this]()
           {
               run();
           }}
        .detach();
}

void Client::send_msg(const string &__msg)
{
    bool is_idle{m_outgoing_msgs.empty()};
    m_outgoing_msgs.push(__msg);

    if (is_idle)
        async_write();
}

void Client::input_and_checking_loop()
{
    while (true && get_state())
    {
        string msg("");
        cout << "Enter string: ";
        getline(cin, msg);

        if (msg == "q" || msg == "quit" || msg == "exit")
            break;
        msg += "\n";

        complex_processing(msg);
        set_state(false);
    }
}

void Client::proccessing_shared_buffer()
{
    if (m_shared_buffer.get_str() != "" && !get_state())
    {
        m_mutex.lock();

        strmanip shared_buffer_copy(m_shared_buffer);
        clear_shared_buffer();

        cout << shared_buffer_copy << endl;

        uint total_sum(shared_buffer_copy.sum_of_digits());

        m_mutex.unlock();

        send_msg(to_string(total_sum));
        set_state(true);
    }
}

// Test string: 1253861794201345684590134913469308656340523450982453247592391237
int main()
{
    Client client(SERVER_SETTINGS_IP, SERVER_SETTINGS_PORT_NUMBER);

    client.on_msg = [](const string &msg)
    {
        cout << msg;
    };

    client.run_in_thread();

    thread loop_thread([&client]()
                       { client.input_and_checking_loop(); });
    thread processing_thread([&client]
                             { client.proccessing_shared_buffer(); });

    // while (true && client.get_state())
    // {
    //     string msg("");
    //     cout << "Enter string: ";
    //     getline(cin, msg);

    //     if (msg == "q" || msg == "quit" || msg == "exit")
    //         break;
    //     msg += "\n";

    //     client.complex_processing(msg);
    //     client.set_state(false);

    //     client.proccessing_shared_buffer();
    // }

    client.stop();
    loop_thread.join();
    processing_thread.join();

    return EXIT_SUCCESS;
}
