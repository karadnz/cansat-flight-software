void begin_serials()
{
	Serial.begin(115200);
	XBee.begin(9600, SERIAL_8N1, XBee_RX, XBee_TX);
	Serial2.begin(9600, SERIAL_8N1, RXPin, TXPin);
}

void init_sensors()
{
	initBMP280();
	initBNO055();
	initRTC();
}


void initBMP280()
{
	isBMP280Initialized = bmp.begin(0x76);

	if (!isBMP280Initialized)
		Serial.println("Couldn't find BMP280 sensor");
}

void initBNO055()
{
	isBNO055Initialized = bno.begin();

	if (!isBNO055Initialized)
		Serial.println("Couldn't find BNO055 sensor");
	else
		bno.setExtCrystalUse(true);
}

void initRTC()
{
	isRTCInitialized = rtc.begin();

	if (!isRTCInitialized)
		Serial.println("Couldn't find DS3231");
	else if (rtc.lostPower())
	{
		Serial.println("RTC lost power, let's set the time!");
		// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
}