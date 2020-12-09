# AEMO Parser
The Australian Energy Market Operator (AEMO) publishes 5 minute settlement data for the National Electricity Market (NEM) in a JSON file located at
https://aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY

This JSON file includes price and generation/demand data useful for making/scripting demand management decisions. 

## Dependencies
This code uses the following libraries
* cURL (https://curl.haxx.se/)
* cJSON (https://github.com/DaveGamble/cJSON)

## Install cURL
```sh
$ wget https://curl.se/download/curl-7.74.0.tar.gz
$ tar -xzf curl-7.74.0.tar.gz
$ cd curl-7.74.0.tar.gz
$ make
$ sudo make install
```
## Install cJSON
```sh
$ wget https://github.com/DaveGamble/cJSON/archive/v1.7.14.tar.gz
$ tar -xzf curl-7.74.0.tar.gz
$ cd curl-7.74.0/
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
$ ldconfig -v
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
and check if https is listed as a protocol.



