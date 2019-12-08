# minfos-beer-fridge-client
Arduino code that runs on the MBF fridge box.

## Introduction
The MBF system is intended to be zero maintenance. From time-to-time, however you may need to make a code change. The most likely reason for this is if you re-deploy the [MBF serverless stack](https://github.com/hoppy1977/minfos-beer-fridge), which would cause a new API endpoint to be generated.

## Setting up the hardware

## Setting up Visual Code

## Configuring the Arduino IDE

## Uploading Code

## Debugging



Requirments:
* Visual Code
* Visual Studio Code extension for Arduino

Instructions:
* Obtain the API key from Systems Manager Parameter Store. The parameter name is:
/MinfosBeerFridge/ApiKeys/Client
Insert this in the code where it says `<TODO: REDACTED>`
* Set the board to be "Arduino/Genuino Mega or Mega 2560" 

Requires the following libraries:
* ArduinoJson (version 5x): http://blog.benoitblanchon.fr/
* DallasTemperature: https://github.com/milesburton/Arduino-Temperature-Control-Library
* NewliquidCrystal: https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
* OneWire: https://www.pjrc.com/teensy/td_libs_OneWire.html
