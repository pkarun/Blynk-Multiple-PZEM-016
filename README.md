# Blynk with 3 PZEM-016 and NodeMCU for Phase Failure Automation
Blynk ESP8266 (NodeMCU) Program for PZEM 016 Power Meter. 

<h2> What it Does?</h2>

1) If voltage is lessthan minimum set value for all 3 Phase, then it turn OFF Relay 1</br>
2) Monitors 3 PZEM 016 device data with one ESP8266 (NodeMCU) device (multiple salve)</br>
3) Auto Mode ON/OFF - With Auto Mode ON, it turn ON Relay 1 if it meets all 3 Phase voltage reaches minimum voltage value. If any those condition don't satisify, then it won't turn on</br>
4) If voltage is lessthan minimum set value for all 3 Phase, then it turn OFF Relay 1</br>
5) Show sum of voltage, current usage, active power, active energy, frequency and power factor</br>
6) You can update firmware using "HTTP Server OTA" method. In other words, through internet you can update firware without having physical access to device or without connecting to same network</br>
7) For other 1 Relays user can connect other devices to control On/Off through internet using blynk app</br>


<h2>Requirements</h2>
1) <a href="http://s.click.aliexpress.com/e/pV6CDdr2">PZEM-016 (3 No)</a></br>
2) <a href="http://s.click.aliexpress.com/e/nlefJ4PI">NodeMCU (1 No) </a></br>
3) <a href="http://s.click.aliexpress.com/e/5D9mJ8JW">MAX485 Module RS-485 TTL to RS485 MAX485CSA Converter Module (1 No) </a></br>
4) <a href="http://s.click.aliexpress.com/e/K59e6yQc">2 Channel Relay Module (1 No)</a> </br>
5) <a href="http://s.click.aliexpress.com/e/e9vv4Wwy">USB to RS485 485 Converter Adapter (optional)</a></br>
6) <a href="https://play.google.com/store/apps/details?id=cc.blynk">Blynk App</a></br>

<h2> Installation </h2>
<ul>
<li>Open <code>secret.h</code> and change Bynk Auth code, Wifi settings, server settings and few other parameters as per your project requirement. </li>
<li>Open <code>settings.h</code> - Usually you don't need to change any values here, but if you need any customization feel free play with it.</li>
</ul>


<h2> Hardware Connection </h2>

<h3>PZEM-016 to NodeMCU</h3>

GND to GND</br>
5v to Vin</br>

<h3>Connect 3 PZEM-016 to RS485</h3>

A to A</br>
B to B</br>

<h3>RS-485 TTL to NodeMCU</h3>

VCC to 3V</br>
GND to GND</br>
DI  to D6/GPIO12</br>
DE  to D1/GPIO5</br>
RE  to D2/GPIO4</br>
RO  to D5/GPIO14</br>

<h3>NodeMCU to 2 channel Relay Module</h3>
   
D4 to Relay Pin 1</br>
D0 to Relay Pin 2</br>
Vin to VCC</br>
GND to GND</br>
 

<h2> Software Setup </h2>

1) Download and install the Blynk Mobile App for iOS or Android.

2) Scan the QR code at the bottom of this page to clone the screenshot below, or create a new project yourself and manually arrange and setup the widgets.

3) Email yourself the Auth code.

4) Download this repo and copy the files in to your sketches directory. Open the sketch in Arduino IDE.

5) Go to the settings.h tab. This is where all the customisable settings are. You should be able to change almost everything from there before compiling.

<h2> Product Images </h2>

<img src="/images/products/peacefair-pzem-016-with-split-ct.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>

<img src="/images/products/max485-rs-485-to-ttl-converter-module.jpg" alt="MAX485 Module RS-485 TTL to RS485 MAX485CSA Converter Module " title="MAX485 Module RS-485 TTL to RS485 MAX485CSA Converter Module " width="350" height=""></br>

<img src="/images/products/usb-to-rs485.jpg" alt="USB to RS485 485 Converter Adapter" title="USB to RS485 485 Converter Adapter" width="350" height=""></br>

<h2> Project Screenshots</h2>

<img src="/images/screenshots/1.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>

<img src="/images/screenshots/2.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>

<img src="/images/screenshots/3.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>

<img src="/images/screenshots/4.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>

<img src="/images/screenshots/5.jpg" alt="PZEM-016" title="PZEM-016" width="350" height=""></br>


<h2> Scan QR Code on Blynk App </h2>

<img src="/images/blynk-scan-qr-code.png" alt="Blynk Project QR code" title="Blynk Project QR code"></br>
