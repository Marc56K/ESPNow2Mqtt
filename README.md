Motivation
----------

if you need to create a battery powered device with esp32, then mqtt looks great but connecting to the WiFi could take 2 seconds of high power consumption, add to that the fact that polling for a subscription can last other 2 seconds adn the result is your battery drained doing connection stuff.

With this library you can comunicate with the mqtt server within 100 ms. 

You can build both, the device part and the gateway. Some examples of both.

Architecture
------------

todo:


Client
-------

as a client you need to add this dependence to you program:

```c++
#include <EspNow2MqttClient.hpp>
```

follow the examples on the examples folder

you can send 4 types of messages:

1. ping : testing message, does not reach to mqtt, only tests espnow
2. send : deliveres a message to the queue of your choice
3. subscribe: gives the content of a queue when it changes. this message creates a subscription in the gateway, when the gateway gets the message stores it and delivers to you on the next 'subscribe' message with the same queue
4. multiple: any combination of up to 10 atomic messages (ping, send or subscribe). the response will be an array with the same order

remember anycase that you cannot exced 200 bytes as an esp-now limitation

Server
------

You can build gateway server just taking the example as is. 

You need to share the key, channel and mac with your clients in order to allow only YOUR clients to connect.

### note on dependencies

sometimes the dependencies are not resolved by itself add this manually if happens to you:

```
Crypto@^0.2.0
```

### To compile the libary itself

+ [regenerate protobuf](documentation/protobuf.md)
+ use platform.io and select the profile you need
