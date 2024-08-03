#!/bin/bash

#kill the existing process if any
ssh gringos@gringos.local "killall vanassistant.pios"

#copy the binary to the pi /opt folder
scp ./vanassistant.pios gringos@gringos.local:/opt/vanassistant

#kill any existing process and launch the new one
ssh -f gringos@gringos.local "sh -c 'cd /opt/vanassistant; nohup ./vanassistant.pios /dev/ttyAMA0 115200 > /dev/null 2>&1 &'"
