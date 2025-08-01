#!/bin/bash

#kill the existing process if any
ssh pizerodev "killall vanassistant.pi_fb"

#copy the binary to the pi /opt folder
scp ./vanassistant.pi_fb pizerodev:/opt/vanassistant

#kill any existing process and launch the new one
ssh -f pizerodev "sh -c 'cd /opt/vanassistant; nohup ./vanassistant.pi_fb /dev/ttyAMA0 115200 > /dev/null 2>&1 &'"
