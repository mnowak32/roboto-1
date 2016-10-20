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

## Operation

After boot, the ESP connects to the given WiFi network (in STA mode, an IP address is obtained with DHCP, it is printed then on a serial console) or creates an access point (in AP mode, the IP address is 192.168.4.1). In either mode a simple web server is started serving a single web page with robot controls. Note, the page uses Touch API for HTML 5, so **only touch devices** are able to steer the rover.

