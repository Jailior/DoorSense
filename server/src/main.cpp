#include <boost/asio.hpp>
#include <fstream>
#include <iostream>

using boost::asio::ip::tcp;

const unsigned int PORT = 12345;

std::string handler() {
    return "hello";
}

int main() {

    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

        std::cout << "Server is running on port: " << PORT << std::endl;

        // for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Client connected.\n";

            std::string message = handler();

            boost::system::error_code ignored_error;
            uint32_t img_length;
            boost::asio::read(socket, boost::asio::buffer(&img_length, sizeof(img_length)));

            std::cout << "Got image length: " << img_length << std::endl;

            std::vector<uint8_t> buffer(img_length);
            boost::asio::read(socket, boost::asio::buffer(buffer, buffer.size()));

            std::cout << "Read Image successfully.\n";

            std::ofstream file("received_img.jpg", std::ios::binary);
            file.write(reinterpret_cast<const char*>(buffer.data()), img_length);

            std::cout << "Image received and saved as received_image.jpg" << std::endl;

            file.close();
            socket.close();
            acceptor.close();

        // }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }


    return 0;
}




 // try {
    // boost::asio::io_context io_context;
    // tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));

    // std::cout << "Server is running on port " << PORT << std::endl;

    // tcp::socket socket(io_context);
    // acceptor.accept(socket);
    // std::cout << "Client connected" << std::endl;

    // uint32_t img_length;
    // boost::asio::read(socket, boost::asio::buffer(&img_length, sizeof(img_length)));

    // std::vector<uint8_t> buffer(img_length);
    // boost::asio::read(socket, boost::asio::buffer(buffer));


    // std::ofstream file("received_image.jpg", std::ios::binary);
    // file.write(reinterpret_cast<const char*>(buffer.data()), img_length);
    // file.close();

    // std::cout << "Image received and saved as received_image.jpg" << std::endl;
    // socket.close();
    // acceptor.close();
    // } catch (std::exception& e) {
    //     std::cerr << "Exception: " << e.what() << std::endl;
    // }
