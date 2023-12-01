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

// Function to read GPS data - runs on Core 1
void updateGPS(void* parameter)
{
	for (;;)
	{
		if (xSemaphoreTake(xSemaphore, (TickType_t)10))
		{
			String line;
			while (Serial2.available())
			{
				char c = Serial2.read();
				if (c == '\n')
				{
					if (line.startsWith("$GNGLL"))
					{
						gpsData = line; // Update the global GPS data
						gpsData += '\n';
					}
					line = "";
				}
				else
				{
					line += c;
				}
			}
			xSemaphoreGive(xSemaphore);
			delay(1000); // Read every second
		}
	}
}

// Function to read GPS data - runs on Core 1
void update_GPS(void* parameter)
{
	while (true)
	{
		process_GPS();
	}
}

void process_GPS()
{
	String line;
	while (Serial2.available())
	{
		char c = Serial2.read();
		if (c == '\n')
		{
			if (line.startsWith("$GNGLL"))
			{
				if (xSemaphoreTake(xSemaphore, (TickType_t)10))
				{
					gpsData = line; // Critical section
					xSemaphoreGive(xSemaphore);
				}
			}

			line = "";
		}
		else
			line += c;
	}
}
