#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "RTC.h"
#include <config.h>
#include <arduino_secrets.h>
// Include the NTP library
#include <NTPClient.h>

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS;

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.println(DBAR);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println(DBAR);
} // printWifiStatus() {

/**
 * Connect to wifi
 */
void connectToWiFi()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    return;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
} // connectToWiFi()

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to NTP server...");

  timeClient.begin();
  timeClient.update();

  Serial.println(__DATE__);
  Serial.println(__TIME__);
  /**
   * Get the current date and time from an NTP server and convert it to UTC +TIME_ZONE_OFFSET by passing the time zone offset in hours.
   * You may change the time zone offset to your local one.
  */
  auto timeZoneOffsetHours = TIME_ZONE_OFFSET;
  auto unixTime = timeClient.getEpochTime();
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime);
  Serial.println("The RTC was just set to: " + String(currentTime));
} // end setup

void loop()
{
  RTCTime currenttime;

  // Get current time from RTC
  RTC.getTime(currenttime);

  // Print out date (DD/MM//YYYY)
  Serial.print(currenttime.getYear());
  Serial.print("-");
  Serial.print(Month2int(currenttime.getMonth()));
  Serial.print("-");

  Serial.print(currenttime.getDayOfMonth());
  Serial.print("-");
  // Print time (HH/MM/SS)
  Serial.print(currenttime.getHour());
  Serial.print(":");
  Serial.print(currenttime.getMinutes());
  Serial.print(":");
  Serial.println(currenttime.getSeconds());

  delay(1000);
}
