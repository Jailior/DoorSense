#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

using boost::asio::ip::tcp;

const unsigned int PORT = 12345;
const bool DEBUG = false;
const unsigned int MAX_BUFFER_SIZE = 200;

std::deque<uint32_t> count_buffer;
std::mutex data_mutex; // protects count_buffer
std::atomic<bool> door_open{false};
std::atomic<uint32_t> current_edge_count{0};
std::atomic<int> threshold{2000};

#define ifdebug if(DEBUG)

void network_thread() {
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
            current_edge_count = edge_count;

            ifdebug std::cout << "Received edge count: " << edge_count << std::endl;

            {
                std::lock_guard<std::mutex> lock(data_mutex);
                count_buffer.push_back(edge_count);
                if (count_buffer.size() > MAX_BUFFER_SIZE) {
                    count_buffer.pop_front();
                }
            }

            door_open = (edge_count > static_cast<uint32_t>(threshold.load()));

            if (DEBUG && door_open.load()) {
                std::cout << "ALERT: Door is open\n";
            }
        }

        socket.close();
        acceptor.close();
    } catch (std::exception& e) {
        std::cerr << "Network Error: " << e.what() << std::endl;
    }
}

int main() {
    std::thread net_thread(network_thread);

    ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize glfw3\n";
        return 1;
    }

    // GLFW Config: OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Don't draw glfw window
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Anomaly Detection Monitor", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        net_thread.join();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.75f; // scale all fonts x1.25

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImVec2 window_size((float)display_w, (float)display_h);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Anomaly Detection Monitor", nullptr, window_flags);

        ImGui::Text("Current Edge Count: %u", current_edge_count.load());
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (door_open) {
            ImGui::TextColored(red, "Door State: OPEN");
        } else {
            ImGui::TextColored(green, "Door State: CLOSED");
        }
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        int current_threshold = threshold.load();
        if (ImGui::SliderInt("Open Threshold", &current_threshold, 0, 10000)) {
            threshold = current_threshold;
        }

        ImGui::Dummy(ImVec2(0.0f, 60.0f));

        // Plot Count History
        std::vector<float> history;
        {
            std::lock_guard<std::mutex> lock(data_mutex);
            for (auto count : count_buffer) {
                history.push_back(static_cast<float>(count));
            }
        }
        if (!history.empty()) {
            ImGui::PlotLines("Edge Count History", history.data(), history.size(), 0, nullptr, 0.0f, *std::max_element(history.begin(), history.end()), ImVec2(0, 100));
        }


        ImGui::End();


        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    net_thread.join();
    return 0;
}