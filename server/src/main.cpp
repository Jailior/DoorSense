#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

const unsigned int PORT = 12345;
const bool DEBUG = true;
const unsigned int OPEN_THRESHOLD = 2000;

bool door_open = false;

#define ifdebug if(DEBUG)

int main() {

    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

        ifdebug std::cout << "Server is running on port: " << PORT << std::endl;

        tcp::socket socket(io_context);
        acceptor.accept(socket);

        ifdebug std::cout << "Client connected.\n";

        for (;;) {
                        
            uint32_t edge_count;
            boost::asio::read(socket, boost::asio::buffer(&edge_count, sizeof(edge_count)));
            edge_count = ntohl(edge_count);

            ifdebug std::cout << "Received edge count: " << edge_count << std::endl;

            if (edge_count > OPEN_THRESHOLD) {
                door_open = true;
            } else {
                door_open = false;
            }

            if (door_open) {
                std::cout << "ALERT: Door is open\n";
            }
        }

        socket.close();
        acceptor.close();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }


    return 0;
}