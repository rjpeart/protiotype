#!/bin/bash
#wpa_supplicant -iwlan0 -c/etc/wpa_supplicant.conf -B

if pgrep -x "kismet_server" > /dev/null
then
    echo "kismet_server already Running"
else
    echo "starting kismet_server"
    kismet_server -c wlan1 -n -s &
fi

node scanner.js
