# espbtc

## Retreive the BTC rate and display it on a LED display using ESP8266 

*&laquo; Although this code works just fine I'm not 100% happy with the structure. Some re-arranging of the code will happen "Real soon now" ^_^ &raquo;* 

This is a small ESP8266 NONOS example project that uses my prevously released driver for the cheap QIFEI 4-digit LED modules.

Read more about that project at [github.com/SmallRoomLabs/4bittube](https://github.com/SmallRoomLabs/4bittube)

I'm making a HTTP-call to the [Coindesk API](http://www.coindesk.com/api/) to retrieve the current BTC exchange rates. The rates are returned in a JSON document which I parse with help from the JSON functions in the ESP8266 SDK.

After getting fully connected to the wifi the software does calls the SDK function for resolving a hostname into a IP address and stored the address for future usage.

It then connects to the IP address, builds a HTTP request for the Coindesk API and sends it as a simple TCP request. 

When the HTTP-reply comes back from the server the payload is extracted and sent to the JSON parser to extract the USD/BTC exchange rate from it.

While the module are connecting and waiting for the reply all four decimal points are lit up on the display as an indicator of it working.

A timer function in the main code module makes the request every 30 seconds. 

![Running project](/images/espbtc-animated.gif?raw=true "Starting up and reteriving BTC data")
