Wifi config on row 29&30 #define EXAMPLE_ESP_WIFI_SSID "YourSSID"
Wifi only connects to non 5G right now???

. $HOME/esp/esp-idf/export.sh

// For config
idf.py menuconfig

idf.py build

idf.py -p /dev/ttuUSB0 flash monitor
