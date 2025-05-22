# DoorSense - Door Monitoring System

This project uses an **ESP32-CAM** module to monitor the state of an image frame, specifically a door. It performs **edge detection** on the ESP32 and streams edge count readings via **TCP** to a **C++ server**. The server evaluates the state of the door, either open or closed, and displays real-time data using a GUI built with **ImGui**.

## Features

- Real-time edge detection on ESP32-CAM.
- TCP transmission of edge detection counts to a Boost-based server.
- Tunable threshold-based logic for door open/closed detection.
- Live visualization with ImGui:
    - Edge count plotted over time.
    - Adjustable threshold slider.
    - Door state indicator.
- Robust connection allows for a restart of both ESP32 and server with automatic reconnection.



## System Requirements

### ESP32-CAM:
- Arduino IDE or PlatformIO
- Libraries:
    - `esp32-camera`
    - WiFi


### Server (C++):
- C++17 or later
- Boost.asio
- OpenGL + GLFW
- [Dear ImGui](https://github.com/ocornut/imgui)


## How it works

1. The ESP32 captures an image every ~85ms and applies a Sobel Filter to count edge pixels.
2. It sends the edge pixel count as an unsigned 32-bit integer over a TCP socket.
3. The C++ server receives the edge pixel count, compares it against a user-tunable threshold value, and updates the door state.
4. A GUI windows shows:
    - A plot of recent edge pixel counts.
    - The current edge pixel count.
    - The current threshold value, slider.
    - An indicator for the door state.



**Example of GUI**

![Example of GUI](/img/gui_example.png)


---

## How to Run

### On ESP32-CAM:

1. Load the TCPClient folder as an Arduino IDE or PlatformIO project.
2. Build and upload to the ESP32.
3. Make sure it connects to WiFi and begins sending data to the server IP and port. (Check through serial monitor)

Ensure the ESP32 is configured with the correct WiFi and server IP credentials. Before compiling add a file called `config.h` into the `include` folder.

Define the credentials as so:
```c
#pragma once
#define WIFI_SSID "YOUR_WIFI_SSID_HERE"
#define WIFI_PASS "YOUR_WIFI_PASSWORD_HERE"
#define SERVER_IP "YOUR_SERVER_IP_HERE"
```

The server IP is the local IP address of the machine running the C++ server.

Additionally, add the files `camera_pins.h` and `camera_index.h` to the include folder for your ESP32 model. For this project the XIAO ESP32-S3 Sense was used
with available header files on the *[SeeedStudio GitHub](https://github.com/limengdu/SeeedStudio-XIAO-ESP32S3-Sense-camera/tree/main/take_photos)*.

### On Server (Linux/Windows):

```bash
mkdir build && cd build
cmake ..
make
./server
```

---

## Customization

- **Threshold**: Adjust through the slider in the GUI.
- **ESP32 Sampling Interval**: To send updates faster or slower, adjust `interval` in the `main.cpp` file for the ESP32.
- **Edge Detection Logic**: Can be updated in `detectEdges()` on the ESP32.

---

## License

MIT License, use freely for educational and personal purposes.
