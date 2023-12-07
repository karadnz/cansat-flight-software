#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <RTClib.h>

//////rtos

#include "freertos/FreeRTOS.h" //freeRTOS items to be used
#include "freertos/task.h"

String gpsData = "gps: init\n";		 // Global GPS data string
SemaphoreHandle_t xSemaphore = NULL; // Mutex for synchronization


/////

// init
bool isBMP280Initialized = false;
bool isBNO055Initialized = false;
bool isRTCInitialized = false;
///

int RXPin = 16; // ESP32 pin connected to M8M TX
int TXPin = 17;

#define SEALEVELPRESSURE_HPA (1013.25)

// I2C
Adafruit_BMP280 bmp;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
RTC_DS3231 rtc;

// XBee setup
HardwareSerial XBee(1); // Using the second hardware serial port for XBee
int XBee_RX = 4;		// Replace with your XBee RX pin
int XBee_TX = 2;		// Replace with your XBee TX pin

void setup()
{
	begin_serials();
	init_sensors();

	// rtos
	xSemaphore = xSemaphoreCreateMutex();										// Create a mutex
	xTaskCreatePinnedToCore(update_GPS, "update_GPS", 20000, NULL, 1, NULL, 0); // Task pinned to core 1
}

void loop()
{
	static unsigned long lastUpdate = 0;
	unsigned long currentMillis = millis();
  static int i = 0;

	// 1 second
	if (currentMillis - lastUpdate >= 1000)
	{
		lastUpdate = currentMillis;

		// Gather data
		String data = "ITER: " + String(i) + "\n";
    i++;
		data += "BMP_280\n" + get_BMP280()  + "\n";
		data += "BNO055\n"  + get_BNO055()  + "\n";
		data += "RTC\n"     + get_RTC()     + "\n";
		data += "GPS\n"     + get_GPS()     + "\n";

		// Send data over XBee
		// XBee.println(data);

		Serial.println(data);
	}
}
