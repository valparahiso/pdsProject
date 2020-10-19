#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::steady_timer;
using boost::asio::ip::tcp;

class tcp_client
{
public:

    tcp_client(boost::asio::io_context& io_context, std::string username, std::string password)
            : stopped_(false),
              socket_(io_context),
              deadline_(io_context),
              heartbeat_timer_(io_context),
              username_(username),
              password_(password)
    {
    }

    // Called by the user of the user class to initiate the connection process.
    // The endpoints will have been obtained using a tcp::resolver.
    void start(tcp::resolver::results_type endpoints)
    {
        // Start the connect actor.
        endpoints_ = endpoints;
        start_connect(endpoints_.begin());

        // Start the deadline actor. You will note that we're not setting any
        // particular deadline here. Instead, the connect and input actors will
        // update the deadline prior to each asynchronous operation.
        deadline_.async_wait(boost::bind(&tcp_client::check_deadline, this));
    }

    // This function terminates all the actors to shut down the connection. It
    // may be called by the user of the user class, or by the class itself in
    // response to graceful termination or an unrecoverable error.
    void stop()
    {
        stopped_ = true;
        boost::system::error_code ignored_ec;
        socket_.close(ignored_ec);
        deadline_.cancel();
        heartbeat_timer_.cancel();
    }

private:
    void start_connect(tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (endpoint_iter != endpoints_.end())
        {
            std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

            // Set a deadline for the connect operation.
            deadline_.expires_after(boost::asio::chrono::seconds(60));

            // Start the asynchronous connect operation.
            socket_.async_connect(endpoint_iter->endpoint(),
                                  boost::bind(&tcp_client::handle_connect,
                                              this, _1, endpoint_iter));

        }
        else
        {
            // There are no more endpoints to try. Shut down the user.
            stop();
        }
    }

    void handle_connect(const boost::system::error_code& ec,
                        tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (stopped_)
            return;

        // The async_connect() function automatically opens the socket at the start
        // of the asynchronous operation. If the socket is closed at this time then
        // the timeout handler must have run first.
        if (!socket_.is_open())
        {
            std::cout << "Connect timed out\n";

            // Try the next available endpoint.
            start_connect(++endpoint_iter);
        }

            // Check if the connect operation failed before the deadline expired.
        else if (ec)
        {
            std::cout << "Connect error: " << ec.message() << "\n";

            // We need to close the socket used in the previous connection attempt
            // before starting a new one.
            socket_.close();

            // Try the next available endpoint.
            start_connect(++endpoint_iter);
        }

            // Otherwise we have successfully established a connection.
        else
        {
            std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

            start_write(this->username_ + "-" + this->password_ + "\n");
            // Start the input actor.
            //start_read();
            //std::cout<<"Lettura"<<std::endl;

            // Start the heartbeat actor.
            //start_write();
            //std::cout<<"Scrittura"<<std::endl;

        }
    }

    void start_read()
    {
        std::cout<<"In start read - user"<<std::endl;

        // Set a deadline for the read operation.
        deadline_.expires_after(boost::asio::chrono::seconds(30));


        // Start an asynchronous operation to read a newline-delimited message.
        boost::asio::async_read_until(socket_,
                                      boost::asio::dynamic_buffer(input_buffer_), '\n',
                                      boost::bind(&tcp_client::handle_read, this, _1, _2));


    }

    void handle_read(const boost::system::error_code& ec, std::size_t n)
    {
        std::cout<<"In handle read - user"<<std::endl;
        if (stopped_)
            return;

        std::cout<<"EC: "<<ec<<std::endl;


        if (!ec)
        {
            // Extract the newline-delimited message from the buffer.
            std::string line(input_buffer_.substr(0, n - 1));
            input_buffer_.erase(0, n);

            // Empty messages are heartbeats and so ignored.
            if (!line.empty())
            {
                std::cout << "Received: " << line << "\n";
            }

           //stop(); //aggiunto io ma non sono sicuro perchè sotto c'è questa start_read che mi ripete la lettura e poi
            //mi andrà a fare l'else e ci sarà errore

            start_write(" ");
        }
        else
        {
            std::cout << "Error on receive: " << ec.message() << "\n";

            stop();
        }
    }

    void start_write(std::string data)
    {
        std::cout<<"In start write - user"<<std::endl;
        if (stopped_)
            return;

        std::cout<<"MESSAGGIO: "<<data<<std::endl;
        // Start an asynchronous operation to send a heartbeat message.
        boost::asio::async_write(socket_, boost::asio::buffer(data),
                                 boost::bind(&tcp_client::handle_write, this, _1));
    }

    void handle_write(const boost::system::error_code& ec)
    {
        std::cout<<"In handle write - user"<<std::endl;
        if (stopped_)
            return;

        if (!ec)
        {
            // Wait 10 seconds before sending the next heartbeat.
            heartbeat_timer_.expires_after(boost::asio::chrono::seconds(10));
            heartbeat_timer_.async_wait(boost::bind(&tcp_client::start_read, this));
        }
        else
        {
            std::cout << "Error on heartbeat: " << ec.message() << "\n";

            stop();
        }
    }

    void check_deadline()
    {
        if (stopped_)
            return;

        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (deadline_.expiry() <= steady_timer::clock_type::now())
        {
            // The deadline has passed. The socket is closed so that any outstanding
            // asynchronous operations are cancelled.
            socket_.close();

            // There is no longer an active deadline. The expiry is set to the
            // maximum time point so that the actor takes no action until a new
            // deadline is set.
            deadline_.expires_at(steady_timer::time_point::max());
        }

        // Put the actor back to sleep.
        deadline_.async_wait(boost::bind(&tcp_client::check_deadline, this));
    }

private:
    bool stopped_;
    tcp::resolver::results_type endpoints_;
    tcp::socket socket_;
    std::string input_buffer_;
    steady_timer deadline_;
    steady_timer heartbeat_timer_;
    std::string username_;
    std::string password_;
};