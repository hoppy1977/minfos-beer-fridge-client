# minfos-beer-fridge-client
Arduino code that runs on the MBF fridge box.

## Introduction
The MBF system is intended to be zero maintenance. From time-to-time, however you may need to make a code change. The most likely reason for this is if you re-deploy the [MBF serverless stack](https://github.com/hoppy1977/minfos-beer-fridge), which would cause a new API endpoint to be generated.

The code is pretty simple, Arduino code, but because we are using a Yun shield the deployment and debugging steps are a bit different to a typical Arduino sketch.

## Setting up the hardware
1. Plug the 12V power adapter into the MBF device.<br>
While the Arduino can run off the 5V provided by USB, 12V is required to power the Yun shield. The device will not function correctly if you try to use USB power.
2. Plug an ethernet cable into the device.
WiFi has been disabled on the Yun shield, so you will not be able to deploy your changes to the device until you have wired network connectivity.

## Configuring the Arduino IDE

1. First we need to tell the IDE about the board/shield combo that we want to work upload the code to.<br>
The IDE knows how to work with all the standard Arduino boards, but because the Arduino Mega2560/Yun shield combo is a little unusual we need to configure the IDE to handle it.<br>
    1. Make sure the Arduino IDE is shut down.
    2. Open the following file `C:\Program Files (x86)\Arduino\hardware\arduino\avr\boards.txt`.
    3. Paste the contents of the `ArduinoBoardsUpdate.txt` file to the end of `boards.txt`.
    4. Save the file.
    5. Restart the Arduino IDE.

2. Set the board type.<br>
If you completed the previous step correctly, you should now have a new board type available for use:<br>
<img src="./Resources/board_selection.png" width="475"/>

3. Set the port.<br>
TODO:<br>
<img src="./Resources/port_selection.png" width="535"/>



---
Note:
The Arduino IDE is very unpleasant to use. You may prefer to use Visual Code with the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino). This provides a much more pleasant development experience, but unfortunately you will still need to use the Arduino IDE to upload and debug your code.  

---

## Updating the AWS credentials
If you are deploying the Arduino firmware to the device you will have to at least update the AWS credentials:
1. *API endpoint:* this is given to you when you deploy the [MBF serverless stack](https://github.com/hoppy1977/minfos-beer-fridge).<br>
Use this value to populate the `baseUrl` variable in the code.
2. *API key:* This can be obtained from Systems Manager Parameter Store. The parameter name is:
`/MinfosBeerFridge/ApiKeys/Client`<br>
Use this value to populate the `apiKey` variable in the code. 

## Uploading Code
Once your hardware and IDE are set up, just hit the Upload button (the round button in the toolbar with an arrow pointing to the right).
You will then be asked for the password for the Yun shield (the IDE is using the Yun shield to write the sketch to the Arduino Mega2560 board). The password is 'dragino'.
The upload should take about 30 seconds.

## Debugging
