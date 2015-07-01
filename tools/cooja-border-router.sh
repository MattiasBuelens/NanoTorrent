#!/bin/bash
cd ../contiki/tools
sudo ./tunslip6 -a 127.0.0.1 -p 60000 aaaa::1/64
