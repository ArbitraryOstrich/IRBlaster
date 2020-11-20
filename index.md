## Basic idea outline
Setup a IR transmitter and linked it up with mqtt. Figured out and saved main functions for TV and satellite, leaned heavily on [this code by crankyoldgit][crankyoldgitIRremoteESP8266] to build out my less cool but more specific version. Specifically take a look at [examples/IRMQTTServer], it's advanced code but fairly useful. Also included a DHT22 to match with the fireplace temps and get a good idea of how the fireplace effects living room temps.

## Ongoing notes
 - Nov 19 2020
   - Im seeing a sawtooth noise on the graph that happens over hours when there is not much actual change in temp or humidity. This may be something to do with the soldering job i did and some cross talk going on.  

### Build
- **VS1838B Infrared receiver**

- **940nm IR infrared Emission Tube LED**
   - ForwardVoltage: 0.0-1.3V
   - ReverseVoltage: 5V
   - Forward DC: 30mA
   - Launch Angle: 30
- NPN transistor
- ~73ohm resistors
- DHT22

## References

- **Information**
  - github
    - [SensorsIot/Definitive-Guide-to-IR]
  - youtube
    - [Andreas Spiess #171]
  - e-tinkers
    - [build-an-esp32-web-server-and-ir-remote]
- **IR libraries**
  - [ukw100/IRMP][ukw100IRMP]
    - Deals w/ samsung well but no shaw, i also don't know if its going to do some raw format or not.

  - [ESP8266-HTTP-IR-Blaster][esp8266HTTPIRBlaster]

  - [crankyoldgit/IRremoteESP8266][crankyoldgitIRremoteESP8266]
    - This is apperently built specifically for the esp8266, but supports esp32
    - Looks frequently updated
    - [IRremoteESP8266/SupportedProtocols.md][irremoteesp8266SupportedProtocols.md]
    - Was able to capture a raw recording of the sat power button and replay it.

  - [bengtmartensson/Infrared4Arduino][bengtmartenssonInfrared4Arduino]
      - Recently updated
      - Looks extensive, has ability to feed to an external analyzer

  - [mdhiggins/ESP8266-HTTP-IR-Blaster][mdhigginsESP8266HTTPIRBlaster]

  - [ExploreEmbedded/Arduino-IRremote][exploreembeddedArduinoIRremote]






[build-an-esp32-web-server-and-ir-remote]: https://www.e-tinkers.com/2019/11/build-an-esp32-web-server-and-ir-remote/

[irledsAndReceiver]: https://www.amazon.ca/dp/B07FFQ9B9H
[examples/IRMQTTServer]: https://github.com/crankyoldgit/IRremoteESP8266/tree/master/examples/IRMQTTServer

[Andreas Spiess #171]: https://www.youtube.com/watch?v=gADIb1Xw8PE

[SensorsIot/Definitive-Guide-to-IR]: https://github.com/SensorsIot/Definitive-Guide-to-IR/

[mdhigginsESP8266HTTPIRBlaster]: https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster/
[z3t0ArduinoIRremote]: https://github.com/z3t0/Arduino-IRremote
[sensorsiotESP32IRremote]: https://github.com/SensorsIot/Definitive-Guide-to-IR/tree/master/ESP32-IRremote
[sensorsiotIRMQTT_ESP32]: https://github.com/SensorsIot/Definitive-Guide-to-IR/tree/master/IRMQTT_ESP32
[crankyoldgitIRremoteESP8266]: https://github.com/crankyoldgit/IRremoteESP8266

[bengtmartenssonInfrared4Arduino]: https://github.com/bengtmartensson/Infrared4Arduino

[irremoteesp8266SupportedProtocols.md]: https://github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md

[exploreembeddedArduinoIRremote]: https://github.com/ExploreEmbedded/Arduino-IRremote

[esp8266HTTPIRBlaster]: https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster/

[ukw100IRMP]: https://github.com/ukw100/IRMP
