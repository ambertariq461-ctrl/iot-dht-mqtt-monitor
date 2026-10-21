# IoT Temperature & Humidity Monitor with Alarm

ESP32 + DHT22 + buzzer simulated in Wokwi, publishing sensor data over MQTT
to a Google Colab notebook for storage, visualization and analysis.

## How it works
1. The ESP32 reads temperature and humidity from a DHT22 sensor.
2. If temperature > 35 °C or humidity > 70 %, the buzzer sounds and `alarm = 1`.
3. Readings are published as JSON to a public MQTT broker (HiveMQ).
4. A Colab notebook subscribes to the topic, saves the readings to CSV,
   plots them against the alarm limits and computes statistics.

## Components
- ESP32, DHT22 sensor, buzzer (Wokwi simulation)
- MQTT broker: `broker.hivemq.com`, topic `iot-DHT-4827/sensor`
- Python: paho-mqtt, pandas, matplotlib

## Circuit
![Circuit](Clean%20Wokwi%20circuit.jpeg)
![Circuit with serial monitor](circuit%20with%20serial%20monitor.jpeg)

## Results
![Live MQTT data](Live%20MQTT%20data%20arriving%20in%20Colab.jpeg)
![Plot](Final%20plot%20with%20alarm%20limits.jpeg)
![Statistics](Summary%20statistics.jpeg)

- 36 readings collected
- Temperature: 17.1 to 54.5 °C, humidity: 32 to 75 %
- Alarm active in 44.4 % of readings (16 of 36)

## Files
- `sketch.ino`, `diagram.json`, `libraries.txt`, `wokwi-project.txt`: Wokwi project
- `Untitled0.ipynb`: Colab notebook (data collection, plot, statistics)

## How to run
1. Open the project in Wokwi (add your Share link here) and press Play.
2. Open the notebook in Google Colab.
3. Run the install cell, then the data collection cell while Wokwi is running.
4. Run the plot and statistics cells.

## Future work
Live dashboard, anomaly detection, real ESP32 hardware.
