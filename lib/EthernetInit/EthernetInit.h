#include <Ethernet.h>
#include <Dhcp.h>

void initializeEthernetDHCP(uint8_t *mac)
{
    Ethernet.init(17);

    while (Ethernet.begin(mac) == 0)
    {
#ifdef DEBUG
        Serial.println(F("Failed to configure Ethernet using DHCP"));
#endif
        // Check for Ethernet hardware present

        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
#ifdef DEBUG
            Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
#endif
            while (true)
            {
                delay(1); // do nothing, no point running without Ethernet hardware
            }
        }

        if (Ethernet.linkStatus() == LinkOFF)
        {
#ifdef DEBUG
            Serial.println(F("Ethernet cable is not connected."));
#endif
        }
    }

#ifdef DEBUG
    Serial.print(F("My IP address: "));
    Serial.println(Ethernet.localIP());
#endif
}


void ethernetMaintain()
{
    int rc = Ethernet.maintain();

    switch (rc)
    {
    case DHCP_CHECK_REBIND_OK:
    case DHCP_CHECK_RENEW_OK:
#ifdef DEBUG
        Serial.print(F("My IP address: "));
        Serial.println(Ethernet.localIP());
#endif
        break;
    }
}
