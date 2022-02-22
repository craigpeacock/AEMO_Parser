# AEMO <-> MQTT Parser/Connector
The Australian Energy Market Operator (AEMO) publishes 5 minute settlement data for the National Electricity Market (NEM) in a JSON file located at
https://visualisations.aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY

This JSON file includes price and generation/demand data useful for making/scripting demand management decisions. 

## Usage
```
./aemo -?
AEMO <-> MQTT Connector
Usage: aemo region [options]

Regions: NSW1, QLD1, SA1, TAS1, VIC1

Options:
	-l <filename>    Log to file
	-m <broker URI>  Log to MQTT Broker
	-t <topic>       MQTT topic
	-u <username>    Username for MQTT Broker
	-p <password>    Password for MQTT Broker
```

## Log File Format

The log will have the following format:

[local time],[number of tries],[settlement time],[price],[total demand],[net interchange],[scheduled generation],[semi scheduled generation]

e.g. 2022-02-21 17:20:22,1,2022-02-22 11:25:00,-38.47,604.96,59.80,80.00,580.52

Local Time: 2022-02-21 17:20:22<BR>
Number of Tries: 1<BR>
Settlement Time: 2022-02-22 11:25:00<BR>
Price: -38.47 (Dollars)<BR>
Total Demand: 604.96 (MW)<BR>
Net Interchange: 59.80 (MW) - Power being imported/exported from region via interconnector(s)<BR>
Scheduled Generation: 80.00 (MW) - Typically baseload fossil fuel<BR>
Semi Scheduled Generation: 580.52 (MW) - Typically variable renewable energy<BR>

## Dependencies
This code uses the following libraries
* cURL (https://curl.haxx.se/)
  * Requires OpenSSL for https support (https://www.openssl.org/)
* cJSON (https://github.com/DaveGamble/cJSON)
* Eclipse paho MQTT library (https://www.eclipse.org/paho/)
  * Requires OpenSSL

Install using package manager:
```
sudo apt-get install libpaho-mqtt-dev libcurlpp-dev libcjson-dev
```
Or build from code:

### Install OpenSSL
```sh
$ wget https://www.openssl.org/source/openssl-1.1.1m.tar.gz
$ tar -xzf openssl-1.1.1m.tar.gz
$ cd openssl-1.1.1m/
$ ./config
$ make
$ sudo make install
```

### Install cURL
```sh
$ wget https://curl.se/download/curl-7.81.0.tar.gz
$ tar -xzf curl-7.81.0.tar.gz
$ cd curl-7.81.0
$ ./configure --with-openssl
$ make
$ sudo make install
```
### Install cJSON
```sh
$ wget https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.15.tar.gz
$ tar -xzf v1.7.15.tar.gz
$ cd cJSON-1.7.15
$ make
$ sudo make install
```

### Install paho libraries
```
$ wget https://github.com/eclipse/paho.mqtt.c/archive/refs/tags/v1.3.9.tar.gz
$ tar -xzf v1.3.9.tar.gz
$ cd paho.mqtt.c-1.3.9
$ make
$ sudo make install
```

## Potential errors
### Error loading shared libraries
If you get the following error message 
```
error while loading shared libraries: libcjson.so.1: cannot open shared object file: No such file or directory
```
execute
```
$ sudo ldconfig -v
```
### Unsupported protocol
If you get this message
```
curl_easy_perform() failed: Unsupported protocol
```
It may be because curl hasn't been compiled with SSL. Run
```
$ curl -V
```
and check if https is listed as a protocol. If not, you may not have OpenSSL installed correctly.



