# ROBOTO-1

A simple remote controlled rover with an ESP8622 (WeMos D1 mini) as a controller. An Arduino sketch.

## Wifi credentials

In order to get the sketch to work, you have to provide a file with Wi-Fi connection credentials (not included in the repository):

```c_cpp
#define WIFI_STA_SSID "SSID"
#define WIFI_STA_PASS "passwd"

#define WIFI_AP_SSID "ROBOTO-1"
#define WIFI_AP_PASS "robotopass"

```

The file has to be named `WifiCredentials.h`.

