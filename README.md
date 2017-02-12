# protiotype
Protiotype development sketches

## System Architecture
The protiotype electronics hardware current consists of the following components:
* Bosch XDK: sensor box, containing temperature, humidity, noise, accelerator, gyro, air pressure and magnetic sensors
* Raspberry Pi: acting as a WiFi access point to collect the data from the XDK and possibly other sensors (could also be Bluetooth LE) and forwards the data to the protiotype PC
* Protiotype PC: backend PC running the database, UI and ACAengine

![System Architecture](./images/architecture.png)

## Code in Sub-Directories

### romantic lift
A basic sketch switching between different videos depending on how many people are in the lift.

### xdk-udp
XDK firmware which reads all the sensors every 10s (for now), samples noise at 1kHz and broadcasts the data in a UDP packet in the local network to port 6666

### xdk-bridge
Node.js server running on the Raspberry Pi to listen on UDP port 6666 for XDK data, forwarding the same as JSON to the /environment endpoint of the consolidator using a POST request. The JSON is of the following format:

    {"accel":{"x":11,"y":83,"z":1035},"gyro":{"x":12,"y":-40,"z":-132},"lux":5760,"noise":55,"mag":{"x":40,"y":-36,"z":-35},"resistance":6627,"pressure":1011.87,"temperature":32.45,"humidity":55}

The current data in the same format can be read from the same endpoint using the GET method.

### kismet-bridge
Node.js kismet client that pushes the following two pieces of information to the consolidator:

1. List of currently present clients (/clients endpoint)

    [{"mac":"C8:F7:33:12:0F:CA","firsttime":1486910495917,"signal":"-86","manufacturer":"IntelCor","lasttime":1486910503392},
     {"mac":"FC:F1:36:8D:8E:51","firsttime":1486910495918,"signal":"-87","manufacturer":"Unknown","lasttime":1486910495918},
     {"mac":"CC:08:8D:0D:54:A8","firsttime":1486910495919,"signal":"-86","manufacturer":"Apple","lasttime":1486910495919},
     {"mac":"FC:F1:36:8C:E0:27","firsttime":1486910495920,"signal":"-87","manufacturer":"Unknown","lasttime":1486910503394},
     {"mac":"B4:75:0E:1D:65:DA","firsttime":1486910497919,"signal":"-88","manufacturer":"BelkinIn","lasttime":1486910497919},
     {"mac":"48:02:2A:0B:D7:FA","firsttime":1486910501135,"signal":"-89","manufacturer":"B-LinkEl","lasttime":1486910501135},
     {"mac":"90:18:7C:DF:34:E9","firsttime":1486910501136,"signal":"-48","manufacturer":"SamsungE","lasttime":1486910506726},
     {"mac":"44:04:44:CB:77:1D","firsttime":1486910502307,"signal":"-91","manufacturer":"Guangdon","lasttime":1486910502307},
     {"mac":"E0:8E:3C:03:0F:EF","firsttime":1486910502310,"signal":"-90","manufacturer":"AztechEl","lasttime":1486910502310},
     {"mac":"DC:0C:5C:80:D3:11","firsttime":1486910503391,"signal":"-88","manufacturer":"Apple","lasttime":1486910503391},
     {"mac":"48:88:CA:14:2F:17","firsttime":1486910503392,"signal":"-87","manufacturer":"Motorola","lasttime":1486910503392},
     {"mac":"FC:F1:36:8D:8E:55","firsttime":1486910503393,"signal":"-88","manufacturer":"Unknown","lasttime":1486910511025},
     {"mac":"14:BB:6E:D5:2C:18","firsttime":1486910503394,"signal":"-87","manufacturer":"SamsungE","lasttime":1486910503394},
     {"mac":"E8:2A:EA:AA:38:E5","firsttime":1486910507836,"signal":"-39","manufacturer":"IntelCor","lasttime":1486910509926},
     {"mac":"48:1D:70:EA:DF:1F","firsttime":1486910511023,"signal":"-84","manufacturer":"CiscoSpv","lasttime":1486910511023},
     {"mac":"2A:6C:07:32:53:FF","firsttime":1486910514125,"signal":"-83","manufacturer":"Unknown","lasttime":1486910514125}]


2. Events when a new client appears or disappears (/clients/events endpoint)

    Format TBD

### consolidator
Node.js server which currently collects the data from `xdk-bridge` and `kismet-bridge` and presents the data through the following endpoints

1. `/environment`: data captured by the XDK
2. `/clients`: data from kismet on the Raspberry Pi
3. `/clients/events`: events from kismet indicating the appearence or disappearence of a new client

### nginx configuration
nginx is used as a web server serving up the front-end pages. The webserver is setup to forward requests for the `/data` directory to the consolidator removing the data portion of the url.

    location /data {
        rewrite /data/(.*) /$1  break;
        proxy_pass http://localhost:8000;
        proxy_set_header Host      $host;
        proxy_set_header X-Real-IP $remote_addr;
    }


