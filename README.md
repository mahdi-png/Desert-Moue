# Project - Desert Mouse

Created by: xorus
Status: Done
Priority: P 1
Tags: IoT, Microcontollers, Opensource
Summary: The "Desert Mouse" project enables control of an ESP32 circuit via voice commands through a WebSocket connection. It includes a structured folder layout for the code, instructions for running the ESP32 and website, and details on using ngrok for secure access. Key steps involve configuring Wi-Fi credentials, setting up necessary libraries, and ensuring both the ESP32 and website are on the same network for successful communication. Troubleshooting tips are provided for common connection issues.

# Operation: Desert Mouse

## ESP32 Controlled via Voice command

This project allows you to control an ESP32 circuit through WebSocket commands sent via a website. The ESP32 receives commands for turning the LED on/off, blinking, and displaying numbers through voice commands processed by a WebSocket connection.

## Folder Structure:

- **/Main**: Contains the main ESP32 code (`main.cpp`) and configuration (`config.h`).
- **/Website**: Contains the frontend code (HTML, JavaScript, and optional CSS).
    - **/Flask file**:  flask app Folder (`app.py`)
- **/ngrok**: Contains the ngrok configuration file (`ngrok.yml`).
- **/README.md**: Read ME files in different shapes and form.

## How to Run the Project:

### 1. **Running the ESP32 Code**:

- Open `Main/main.cpp` in your Arduino IDE. - Make sure to update the Wi-Fi credentials:
    
    ```cpp
    const char* ssid = "Your_SSID";
    
    const char* password = "Your_PASSWORD";
    ```
    
- Upload the code to your ESP32 board using the Arduino IDE.
    - you need the following Setup first:
        
        To run the given code on your ESP32, you will need the following libraries and drivers:
        
        ### Required Libraries
        
        1. **Wi-Fi**:
            - **Library**: `<WiFi.h>`
            - **Publisher**: Espressif Systems
            - Installed with the ESP32 core package.
        2. **WebSocketsServer**
            - **Library**: `<WebSocketsServer.h>`
            - **Publisher**: Markus Sattler
            - Install via the Arduino Library Manager or manually from the [Arduino WebSockets GitHub repository](https://github.com/Links2004/arduinoWebSockets).
        3. **ESP32 Core for Arduino**
            - Installation: Use the Board Manager in the Arduino IDE and add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` to the "Additional Boards Manager URLs."
        4. **CP210x USB to UART Bridge VCP Drivers** - Required to allow your computer to communicate with the ESP32 via USB.
            - For Windows: [Download from Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
            - For macOS/Linux: Typically included, but manual installation might be needed.
            - If you encounter any further errors with your USB connection and the port which is connected to ESP32 is not being recognized in Arduino IDE, try one of these two drivers depending on your board (you can install both if you're now sure):
                - 1. **CH341SER Driver**
                    - **Purpose**: Required for ESP32 boards that use the CH340 chip for USB-to-serial communication.
                    - **Download**: WCH official website [here](https://chatgpt.com/c/46).
                        - secondary download link [here](https://chatgpt.com/c/47).
                    - **Why You may Need It**: Some ESP32 modules use the CH340 chip for communication with your computer. Without this driver, the board won't be recognized by your computer.
                - 2. **CP210x Universal Windows Driver**
                    - **Purpose**: Required for ESP32 boards that use the CP210x chip for USB-to-serial communication.
                    - **Download**: Silicon Labs official website: [Download from Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
                    - **Why You may Need It**: If your ESP32 module uses the CP210x chip, this driver ensures your computer recognizes and communicates with the board.
                - 3. **Arduino IDE Configuration**
                    - Board: `ESP32 Dev Module` or the appropriate ESP32 variant.
                    - serial monitor Baud rate : `115200`
                    - Upload Speed: `115200` (adjust if needed).
                    - Flash Frequency: `80MHz`. - The ESP32 will connect to Wi-Fi and start a WebSocket server on port 81.

### 2. **Running the Website**:

1. Open windows PowerShell in the `/website/` folder.
2. Type in this command `npx http-server` and hit Enter to run the website on a localhost (your computer).

1. you will then have something like this:

`Available on: http://x.x.x.x:8080`

              `http://x.x.x.x:8080`

`Hit CTRL-C to stop the server`

1. Depending on your website port you need to modify your website address in the `ngrok.yml` file. (in this case its `8080`).

> OR if you don’t have Node.js on you machine you can run the Flask app in `Flask file` folder. use `python app.py` to run it.
> 
- **Ensure that the ESP32 and the computer running the browser are on the same network.**

### 3. **Running Everything on ngrok**:

- Why:
    - if you want to use microphone to send voice commands you need to run the website on https protocol which requires SSL certificate. but for now we use **ngrok** to run the website through its https tunnels.
    - to make the ESP32 WebSocket accessible for the website, we use **ngrok** to tunnel both ESP32 and the website, Because due to security protocols no browser will allow our website to send massages to a `http` or `ws` address.

**Setup ngrok**:

- Download and install [ngrok](https://ngrok.com/download) if you haven't already. - Place the `ngrok.yml` file from the `/ngrok` Folder, (or any folder it doesn't mater).
- **ngrok Configuration (`ngrok.yml`)**:
    - Open the `ngrok.yml` file and configure the `websocket` settings as needed:
        
        ```yaml
        version: "2"
        authtoken: your auth-token
        tunnels:
        website:
        	addr: 8080 #(website port)
        	proto: http
        websocket:
        	addr: x.x.x.x:81 #(ESP32 IP address (it should be printed in serial monitor))
        	proto: http
        ```
        

**Start ngrok**

- open another PowerShell and tab run the following command:
    
    `ngrok start --config="X:\\blabla\\blabla\\.ngrok\\ngrok.yml" --all`
    
- ngrok will provide a public URL (e.g., `https://abc123.ngrok.io`) that can be used to access the WebSocket from "anywhere!", for both the website and ESP32.
    
    > note: you can still use the website without ngrok but you wont be able to use voice command.
    > 

### 4. **Accessing the WebSocket Remotely**:

- If using ngrok, replace the WebSocket server URL in `website/index.html`:
    
    ```jsx
    const socket = new WebSocket('wss://abc123.ngrok.io');
    // Replace with your ngrok URL
    ```
    
- Ensure the frontend (website) points to the correct WebSocket URL either for local or ngrok tunnel connections.

### 5. **Final Notes**:

- **Testing Locally**: When testing locally, make sure the ESP32 and the website are on the same network. - **Voice Command Processing**: The website listens for voice commands and sends them to the ESP32 for processing. The ESP32 executes actions like turning the LED on/off or blinking based on the received commands.

### Troubleshooting:

- If the WebSocket connection is not working, check that the ESP32 is running and connected to the Wi-Fi network (check serial monitor). (also check if your Wi-Fi credentials are currect).
- If ngrok is not tunneling correctly, verify that the correct port (`81`) is exposed in the `ngrok.yml` file and that ngrok is running. (also check your auth-token)
