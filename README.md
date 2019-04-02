# minfos-beer-fridge-client
Arduino code that runs on the MBF fridge box.

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
