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
	xTaskCreatePinnedToCore(update_GPS, "update_GPS", 10000, NULL, 1, NULL, 1); // Task pinned to core 1
}

void loop()
{
	// Gather data
	String data = "";
	data += get_BMP280();
	data += get_BNO055();
	data += get_RTC();
	data += get_GPS();

	// Send data over XBee
	// Xbee.println(data);

	Serial.println(data);

	// todo: change to ms
	delay(1000); // Wait for a second
}
