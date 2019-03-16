#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Console.h>
#include <ArduinoJson.h>

const byte sensorPin = 2;
const byte backlightPin = 13;

#define MAX_TEMPERATURE 50
#define MIN_TEMPERATURE -20

// The addresses of the various sensors we work with
DeviceAddress alphaBeerThermometer = { 0x28, 0xFF, 0x20, 0x44, 0x52, 0x15, 0x01, 0x27 };
DeviceAddress betaBeerThermometer = { 0x28, 0xFF, 0x8F, 0x69, 0x52, 0x15, 0x02, 0x74 };
DeviceAddress roomThermometer = { 0x28, 0xFF, 0x6A, 0xAC, 0x53, 0x15, 0x03, 0x31 };
DeviceAddress fridgeThermometer = { 0x28, 0xFF, 0xA4, 0x6F, 0x52, 0x15, 0x01, 0x17 };
//DeviceAddress testThermometer1 = { 0x28, 0xD7, 0xB5, 0x00, 0x00, 0x00, 0x80, 0x4E };
//DeviceAddress testThermometer2 = { 0x28, 0xCE, 0xB7, 0x00, 0x00, 0x00, 0x80, 0x00 };

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(sensorPin);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
float beerTemperature = -999;
float roomTemperature = -999;
float fridgeTemperature = -999;

// LCD
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// ===============
// AWS Settings
//char awsAddress[] = "mbf.azurewebsites.net";
//const int awsPort = 80;
//const String awsApiKey = "DXkE/XXWAJxFA2Bzf/YacOYz4udZAjYfyJgSAxPOSrHqdMii037q6w==";

void setup()
{
	Bridge.begin();
	Console.begin();

	pinMode(backlightPin, OUTPUT);
	digitalWrite(backlightPin, HIGH);

	Console.println("Setting up the LCD...");
	Console.println();
	lcd.begin(20, 4);

	printStartup();
	delay(2000);

	updateCertificateExtract();
}

void loop()
{
	lcd.clear();
	printTemperatures();

	Console.println("Beginning loop...");

	sensors.begin();
	sensors.setResolution(12);
	//	sensors.setCheckForConversion(false);

	//	lcd.setCursor(0, 2);
	//	lcd.print(padForLcd("Sensor count: " + String(sensors.getDeviceCount())));
	//	delay(500);

	Console.print(sensors.getDeviceCount());
	Console.println(" devices connected to the bus.");
	Console.println();

	// ------------------------------
	lcd.setCursor(0, 2);
	lcd.print(padForLcd("Reading sensors..."));

	// ------------------------------
	// Now process each of the known sensors
	String sensorName;

	// ------------------------------
	// Beer temperature
	//clearBottomSegment();
	sensorName = "'beer'";

	Console.println("Processing " + sensorName + " sensor:");
	//lcd.setCursor(0, 2);
	//lcd.print(padForLcd("Sensor " + sensorName));

	if (sensors.isConnected(alphaBeerThermometer))
	{
		beerTemperature = readTemperatureSensor(alphaBeerThermometer, "ALPHA");
	}
	else if (sensors.isConnected(betaBeerThermometer))
	{
		beerTemperature = readTemperatureSensor(betaBeerThermometer, "BETA");
	}
	else
	{
		//lcd.setCursor(0, 3);
		//lcd.print(padForLcd("No " + sensorName + " sensor!"));
		beerTemperature = -999;
		//delay(500);
	}
	printTemperatures();
	//delay(500);

	// ------------------------------
	// Room temperature
	//clearBottomSegment();
	sensorName = "'room'";

	Console.println("Processing " + sensorName + " sensor:");
	//lcd.setCursor(0, 2);
	//lcd.print(padForLcd("Sensor " + sensorName));

	if (sensors.isConnected(roomThermometer))
	{
		roomTemperature = readTemperatureSensor(roomThermometer, sensorName);
	}
	else
	{
		//lcd.setCursor(0, 3);
		//lcd.print(padForLcd("No " + sensorName + " sensor!"));
		roomTemperature = -999;
		//delay(500);
	}
	printTemperatures();
	//delay(500);

	// ------------------------------
	// Fridge temperature
	//clearBottomSegment();
	sensorName = "'fridge'";

	Console.println("Processing " + sensorName + " sensor:");
	//lcd.setCursor(0, 2);
	//lcd.print(padForLcd("Sensor " + sensorName));

	if (sensors.isConnected(fridgeThermometer))
	{
		fridgeTemperature = readTemperatureSensor(fridgeThermometer, sensorName);
	}
	else
	{
		//lcd.setCursor(0, 3);
		//lcd.print(padForLcd("No " + sensorName + " sensor!"));
		fridgeTemperature = -999;
		//delay(500);
	}
	printTemperatures();
	//delay(500);

	// ------------------------------
	// Upload data
	uploadTemperatures();

	// ------------------------------
	Console.println();
}

float readTemperatureSensor(DeviceAddress deviceAddress, const String& sensorName)
{
	sensors.requestTemperaturesByAddress(deviceAddress);
	float temperature = sensors.getTempC(deviceAddress);

	//lcd.setCursor(0, 3);
	//lcd.print(padForLcd("Temperature read"));

	Console.print("    Temperature: ");
	Console.print(temperature);
	Console.println(" C");

	if (!isTemperatureValid(temperature))
	{
		lcd.setCursor(0, 3);
		lcd.print(padForLcd("Invalid temperature!"));

		Console.print("    WARNING: Invalid temperature (");
		Console.print(temperature);
		Console.println(").");
		return -999;
	}

	return temperature;
}

void uploadTemperatures()
{
	Console.println("Uploading to AWS...");

	clearBottomSegment();
	lcd.setCursor(0, 2);
	lcd.print(padForLcd("Uploading to AWS"));
	lcd.setCursor(0, 3);
	lcd.print(padForLcd("Posting..."));

	Process p;
	p.begin("curl");
	p.addParameter("-X");
	p.addParameter("POST");

  p.addParameter("-k");
	//p.addParameter("--cacert");
	//p.addParameter("cacert.pem");

	p.addParameter("-w \"%{http_code}\"");
	//	p.addParameter("-s");
	p.addParameter("-o");
	p.addParameter("/dev/null");

	p.addParameter("-H");
	p.addParameter("User-Agent: arduino-yun");

	p.addParameter("-H");
	p.addParameter("Content-Type: application/json");

	p.addParameter("-H");
	p.addParameter("x-api-key: zCHviXD15Z28vfdc3T06o25Rh16XrUSb9JE6bnYi");

	p.addParameter("--data");
	p.addParameter(getData());

	p.addParameter("https://i8ai84tyo4.execute-api.ap-southeast-2.amazonaws.com/v1/temperature");
	p.run();

	lcd.setCursor(0, 3);
	lcd.print(padForLcd("Reading response..."));

	Console.println();
	if (p.exitValue() == 0)
	{
		String httpStatus;
		while (p.available() > 0)
		{
			char c = p.read();
			httpStatus += c;
		}
		httpStatus.trim();

		Console.println("HTTP Status: " + httpStatus);
		lcd.setCursor(0, 3);
		lcd.print(padForLcd("HTTP Status: " + httpStatus));
	}
	else
	{
		Console.println("exitValue: " + String(p.exitValue()));
		lcd.setCursor(0, 3);
		lcd.print(padForLcd("exitValue: " + String(p.exitValue())));
	}
	delay(500);

	Console.println();
	Console.println();

	// Ensure the last bit of data is sent.
	Console.flush();
}

void updateCertificateExtract()
{
	Console.println("Updating CA certificate extract...");

	lcd.setCursor(0, 3);
	lcd.print(padForLcd("Updating certs..."));

	//curl -k --remote-name --time-cond cacert.pem https://curl.haxx.se/ca/cacert.pem
	Process p;
	p.begin("curl");
	p.addParameter("-k");
	p.addParameter("--remote-name");
	p.addParameter("--time-cond");
	p.addParameter("cacert.pem");
	p.addParameter("https://curl.haxx.se/ca/cacert.pem");
	p.run();

	lcd.setCursor(0, 3);
	lcd.print(padForLcd("Reading response..."));

	Console.println();
	if (p.exitValue() == 0)
	{
		Console.println("Success");
		lcd.setCursor(0, 3);
		lcd.print(padForLcd("Success"));
	}
	else
	{
		Console.println("exitValue: " + String(p.exitValue()));
		lcd.setCursor(0, 3);
		lcd.print(padForLcd("exitValue: " + String(p.exitValue())));
	}
	delay(500);

	Console.println();
	Console.println();

	// Ensure the last bit of data is sent.
	Console.flush();
}

void printStartup()
{
	lcd.setCursor(0, 1);
	lcd.print(padForLcd(" Beer Telemetry By"));
	lcd.setCursor(0, 2);
	lcd.print(padForLcd("  Hopatech Systems"));
}

void printTemperatures()
{
	lcd.setCursor(0, 0);
	lcd.print(padForLcd("Beer:" + formatTemperatureForLcd(beerTemperature) + "  Room:" + formatTemperatureForLcd(roomTemperature)));
	lcd.setCursor(0, 1);
	lcd.print(padForLcd("Frge:" + formatTemperatureForLcd(fridgeTemperature)));
}

void clearBottomSegment()
{
	lcd.setCursor(0, 2);
	lcd.print(padForLcd(""));
	lcd.setCursor(0, 3);
	lcd.print(padForLcd(""));
}

String convertTemperatureToString(float temperature)
{
	char temperatureBuffer[6] = {};
	dtostrf(temperature, 5, 2, temperatureBuffer);
	return String(temperatureBuffer);
}

String formatTemperatureForLcd(float temperature)
{
	if (isTemperatureValid(temperature))
	{
		char temperatureBuffer[5] = {};
		dtostrf(temperature, 4, 1, temperatureBuffer);
		return String(temperatureBuffer);
	}

	return "----";
}

bool isTemperatureValid(float temperature)
{
	if (temperature < MIN_TEMPERATURE
		|| temperature > MAX_TEMPERATURE)
	{
		return false;
	}

	return true;
}

String padForLcd(String text)
{
	while (text.length() < 20)
	{
		text.concat(' ');
	}

	return text;
}

String getData()
{
	StaticJsonBuffer<200> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();

	if (isTemperatureValid(beerTemperature))
	{
		root["beerTemperature"] = convertTemperatureToString(beerTemperature);
	}

	if (isTemperatureValid(roomTemperature))
	{
		root["roomTemperature"] = convertTemperatureToString(roomTemperature);
	}

	if (isTemperatureValid(fridgeTemperature))
	{
		root["fridgeTemperature"] = convertTemperatureToString(fridgeTemperature);
	}

	String dataAsString;
	root.printTo(dataAsString);
	Console.println(dataAsString);
	return dataAsString;
}
