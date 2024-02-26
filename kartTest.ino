#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <RTClib.h>

//SPI FLASH https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
#include <Preferences.h>
Preferences preferences;
// critic
// {
//   packet_count: "your_ssid"
//   pass: "your_pass"
// }
int packet_count;


//////RTOS

#include "freertos/FreeRTOS.h" //freeRTOS items to be used
#include "freertos/task.h"

String gpsData = "gps: init\n";		 // Global GPS data string

SemaphoreHandle_t xSemaphore = NULL; // Mutex for synchronization


String xbeeData = "";

// tasks
void readXBeeTask(void* parameter);

/////

// init
bool isBMP280Initialized = false;
bool isBNO055Initialized = false;
bool isRTCInitialized = false;
///

#define SEALEVELPRESSURE_HPA (1013.25)

// ******** PINS ************

//UBLOX UART
int RXPin = 16; // ESP32 pin connected to M8M TX //16
int TXPin = 17; //17

// I2C
Adafruit_BMP280 bmp;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
RTC_DS3231 rtc;

//XBee UART
HardwareSerial XBee(1); 
int XBee_RX = 4;		
int XBee_TX = 2;

//////////////////////////////////////
void setup()
{
	begin_serials();
	init_sensors();

	// rtos
	xSemaphore = xSemaphoreCreateMutex();										// Create a mutex
	xTaskCreatePinnedToCore(update_GPS, "update_GPS", 20000, NULL, 1, NULL, 0); // Task pinned to core 0

  // Create a task for reading XBee data, pinned to Core 0
  xTaskCreatePinnedToCore(readXBeeTask, "readXBeeTask", 10000, NULL, 1, NULL, 0);


  preferences.begin("mission_data", false);
  packet_count = preferences.getUInt("packet_count", 0);
}

void loop()
{
	static unsigned long lastUpdate = 0;
	unsigned long currentMillis = millis();
  //static int i = preferences.getUInt("packet_count", 0);


	// 1 second
	if (currentMillis - lastUpdate >= 1000)
	{
		lastUpdate = currentMillis;

		// Gather data
		String data = "ITER: " + String(packet_count) + "\n";
    packet_count++;
		data += "BMP_280\n" + get_BMP280()  + "\n";
		data += "BNO055\n"  + get_BNO055()  + "\n";
		data += "RTC\n"     + get_RTC()     + "\n";
		data += "GPS\n"     + get_GPS()     + "\n";

		// Send data over XBee
		// XBee.println(data);

    XBee.println(data);
		Serial.println(data);
    preferences.putUInt("packet_count", packet_count);
	}
}
