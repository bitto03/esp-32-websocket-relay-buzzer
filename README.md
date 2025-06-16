# esp-32-websocket-relay-buzzer
ESP32 Relay and Buzzer Control with WebSocket Dashboard

This project allows you to control a relay and buzzer connected to an ESP32 through a real-time web dashboard using WebSockets.

Features

* Real-time control of relay and buzzer via WebSocket.
* Simple single-page dashboard hosted on the ESP32.
* Buzzer beeps continuously in 500ms intervals when relay is ON.
* Relay and buzzer state instantly reflected on the web interface.

Hardware Setup

* ESP32 development board.
* Relay module connected to GPIO 25.
* Buzzer connected to GPIO 26.
* Power and ground connected appropriately.

Software Setup

1. Install the following Arduino libraries: ESPAsyncWebServer, AsyncTCP, ArduinoJson.
2. Update the WiFi credentials in the source code (SSID and password).
3. Upload the code to your ESP32.

Usage

1. Open the Serial Monitor to find the IP address assigned to the ESP32.
2. Open a web browser and go to that IP address.
3. Use the web interface button to toggle the relay and buzzer.
4. The buzzer will beep in 500ms intervals while the relay is ON.
5. The web page updates in real-time using WebSocket communication.

Notes

* You can change the GPIO pins by modifying relayPin and buzzerPin in the code.
* Ensure your relay module and buzzer are compatible with 3.3V or 5V logic levels depending on your ESP32 board.

License
This project is open-source and free to use for personal and educational purposes.

