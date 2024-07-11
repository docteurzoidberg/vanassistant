#!/bin/bash

ssh gringos@gringos.local "killall vanassistant.pi"

#copy the binary to the pi /opt folder
scp ./vanassistant.pi gringos@gringos.local:/opt/vanassistant

#kill any existing process and launch the new one
ssh -f gringos@gringos.local "sh -c 'cd /opt/vanassistant; nohup ./vanassistant.pi > /dev/null 2>&1 &'"
