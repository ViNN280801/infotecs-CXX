#include <iostream>

#include "../../headers/server.hpp"

void Server::start_accept()
{
    m_sock.emplace(m_io_context);

    m_acceptor.async_accept(
        *m_sock,
        [this](const system::error_code &err)
        {
            auto connection{TCP_Connection::create_connection(std::move(*m_sock))};

            if (on_join)
                on_join(connection);

            m_connections.insert(connection);

            if (!err)
            {
                connection->start(
                    [this](const string &msg)
                    {
                if (on_client_msg)
                    on_client_msg(msg); },
                    [&, weak = std::weak_ptr(connection)]
                    {
                        if (auto shared = weak.lock(); shared && m_connections.erase(shared))
                        {
                            if (on_leave)
                                on_leave(shared);
                        }
                    });
            }
            start_accept();
        });
}

int Server::run()
{
    try
    {
        start_accept();
        m_io_context.run();
    }
    catch (const std::exception &ex)
    {
        cerr << "Error occured! Message: "
             << ex.what() << endl;
        return -1;
    }

    return 0;
}

void Server::broadcast(const string &__msg)
{
    for (auto &connection : m_connections)
    {
        connection->send_msg(__msg);
    }
}

string Server::analyze_msg_from_client(const string &__msg) const
{
    strmanip msg_copy{__msg};
    msg_copy.remove_endl();

    if (msg_copy.is_3more_digits() && msg_copy.is_32multiple())
        return green_colour + "Data recieved successfully!\n" + reset_colour + "\n";
    else
        return red_colour + "Error occured:\n" + reset_colour +
               +"Your number is not contains of 3 more digits and is not multiple to 32!\n";
}

int main()
{
    cout << "Listening clients ..." << endl;
    Server server(SERVER_SETTINGS_PORT_NUMBER);

    server.on_join = [](std::shared_ptr<TCP_Connection> server)
    {
        cout << "User has joined to the server: " << server->get_str() << endl;
    };

    server.on_leave = [](std::shared_ptr<TCP_Connection> server)
    {
        cout << "User has left from the server: " << server->get_str() << endl;
    };

    server.on_client_msg = [&server](const string &msg)
    {
        if (msg != "")
        {
            server.broadcast(server.analyze_msg_from_client(msg));
            cout << server.analyze_msg_from_client(msg) << endl;
        }
    };

    server.run();

    return EXIT_SUCCESS;
}