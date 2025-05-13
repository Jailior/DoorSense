#include <boost/asio.hpp>
#include <fstream>
#include <iostream>

using namespace boost::asio::ip::tcp;

#define PORT 12345

int main() {
    try {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

    std::cout << "Server is running on port " << PORT << std::endl;

    tcp::socket socket(io_context);
    acceptor.accept(socket);
    std::cout << "Client connected" << std::endl;

    uint32_t img_length;
    boost::asio::read(socket, boost::asio::buffer(&img_length, sizeof(img_length)));

    std::vector<uint8_t> buffer(img_length);
    boost::asio::read(socket, boost::asio::buffer(buffer));


    std::ofstream file("received_image.jpg", std::ios::binary);
    file.write(reinterpret_cast<const char*>(buffer.data()), img_length);
    file.close();

    std::cout << "Image received and saved as received_image.jpg" << std::endl;
    socket.close();
    acceptor.close();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}