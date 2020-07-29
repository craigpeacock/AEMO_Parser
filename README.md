# AEMO Parser
The Australian Energy Market Operator (AEMO) publishes 5 minute settlement data for the National Electricity Market (NEM) in a JSON file located at
https://aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY

This JSON file includes price and generation/demand data useful for making/scripting demand management decisions. 

## Dependencies
This code uses the following libraries
* cURL (https://curl.haxx.se/)
* cJSON (https://github.com/DaveGamble/cJSON)

