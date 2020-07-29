# AEMO_Parser

The Australian Energy Market Operator (AEMO) publishes 5 minute settlement data for the National Electricity Market (NEM) in a JSON file at
https://aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY

This file includes price and generation/demand data useful for making/scripting demand management decisions. 

This code uses 
* cURL (https://curl.haxx.se/)
* cJSON (https://github.com/DaveGamble/cJSON)

cURL is compiled as a library and linked at build time (using -lcurl)
