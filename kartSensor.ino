String get_BMP280()
{
	if (!isBMP280Initialized)
		return "BMP280 not initialized\n";

	float temperature = bmp.readTemperature();
	float pressure = bmp.readPressure();
	String output = "";

	output += "Temperature = " + String(temperature) + " *C\n";
	output += "Pressure = " + String(pressure / 101325.0) + " ATM\n";
	output += "Approx altitude = " + String(bmp.readAltitude(SEALEVELPRESSURE_HPA)) + " m\n";

	return output;
}

String get_BNO055()
{
	if (!isBNO055Initialized)
		return "BNO055 not initialized\n";

	imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
	String output = "";

	output += "Orientation: X = " + String(euler.x()) + ", Y = " + String(euler.y()) + ", Z = " + String(euler.z()) + "\n";

	return output;
}

String get_RTC()
{
	if (!isRTCInitialized)
		return "RTC not initialized\n";

	DateTime now = rtc.now();
	String output = "";

	output += "Date/Time: " + String(now.year(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.day(), DEC);
	output += " " + String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC) + "\n";

	return output;
}

String get_GPS()
{
	String output = "";
	// RTOS
	if (xSemaphoreTake(xSemaphore, (TickType_t)10))
	{
		output += gpsData + "\n";
		xSemaphoreGive(xSemaphore);
	}
	return output;
}

// Function to read GPS data - runs on Core 0
void update_GPS(void* parameter)
{
	while (true)
	{
		if (Serial2.available() > 0)
		{
			process_GPS();
		}

		// Yield control back to the system
		vTaskDelay(10 / portTICK_PERIOD_MS); // Adjust delay as needed

		// Optionally, feed the watchdog here if necessary
		// esp_task_wdt_reset();
	}
}

void process_GPS()
{
	String line;
	while (Serial2.available())
	{
		char c = Serial2.read();
		// Serial.print(c);
		if (c == '\n')
		{
			if (line.startsWith("$GNGLL"))
			{
				// Serial.print(line);
				if (xSemaphoreTake(xSemaphore, (TickType_t)10))
				{
					gpsData = line; // Critical section
          gpsData += " "; //to delete
          gpsData += packet_count;
					xSemaphoreGive(xSemaphore);
				}
			}

			line = "";
		}
		else
			line += c;

		// Yield control in each iteration
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

void readXBeeTask(void* parameter)
{
	while (true)
	{
		if (XBee.available() > 0)
		{
			// Read incoming data from XBee
			xbeeData = XBee.readString();
			// Print data to Serial (or process it as needed)
			Serial.print("XBee Data: ");
			Serial.println(xbeeData);
		}

		// Delay to yield time to other tasks
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
