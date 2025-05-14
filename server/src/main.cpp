#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>

#include "protocol.hpp"

using boost::asio::ip::tcp;

const unsigned int PORT = 12345;
const int HEIGHT = 240;
const int WIDTH = 240;
const bool DEBUG = true;

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
            {
                // uint32_t img_length;
                // boost::asio::read(socket, boost::asio::buffer(&img_length, sizeof(img_length)));
                // img_length = ntohl(img_length); // Conver to host byte order

                // ifdebug std::cout << "Got image length: " << img_length << std::endl;

                // std::vector<uint8_t> buffer(img_length);
                // boost::asio::read(socket, boost::asio::buffer(buffer, buffer.size()));

                // ifdebug std::cout << "Read Image successfully.\n";

                // // Convert image into jpeg and save
                // cv::Mat img(HEIGHT, WIDTH, CV_8UC1, buffer.data());
                // std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
                // cv::imwrite("received_img.jpg", img, params);
                // ifdebug std::cout << "Image received and saved as received_image.jpg" << std::endl;
            }
            uint8_t bytes;
            boost::asio::read(socket, boost::asio::buffer(&bytes, sizeof(bytes)));
            bytes = ntohl(bytes);

            ifdebug std::cout << "Received door status bytes\n";
            
            DoorState state = (DoorState) bytes;
            std::string stateString = DoorState::to_string(state);

            std::cout << "Door Status: " << stateString << std::endl;
        }

        socket.close();
        acceptor.close();
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
