/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */

//----------------------------------------------------------
//  SYSTEM
#include  <ESP8266WiFi.h>

//----------------------------------------------------------
//  DEFINES

//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS
int               iWifiState = 0;
boolean           bWifiEnable = true;
boolean           bWifiForceEnable = false;

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the WIFI interface.
 */
void wifiInit( void )
{
  WiFi.mode( WIFI_AP_STA );                                 //ESP32 Access point configured
  WiFi.softAP( WIFI_AP_SSID, WIFI_AP_PASSWORD );            //Configuring ESP32 access point SSID and password
  //
  WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
  WiFi.setAutoReconnect( true );
}

/**
 *  This function
 */
void wifiHandle( void )
{
  if( bWifiEnable )
  {
    switch( iWifiState )  
    {
      case 0 :
        if( WiFi.status() == WL_CONNECTED )
          iWifiState++;
        break;
        //
      case 1 :
        if( WiFi.status() != WL_CONNECTED )
          iWifiState++;
        break;
        //
      case 2 :
        WiFi.disconnect();
        WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
        iWifiState = 0;
        break;
        //
      default:
        iWifiState = 0;
        break;
    }
  }
}

/**
 *  This function return "true" if the WIFI network
 *  is connected. Otherwise "false".
 */
boolean wifiIsConnected( void )
{
  return( 1 == iWifiState );
}

/**
 *  This function shutdown the WIFI module.
 */
void wifiShutdown( void )
{
  bWifiEnable = false;
  WiFi.disconnect();
  WiFi.mode( WIFI_OFF );
  iWifiState = 0;
}

/**
 *  This function starts-up the WIFI module.
 */
void wifiStartup( void )
{
  bWifiEnable = true;
  wifiInit();
  iWifiState = 0;
}

/**
 *  This function returns the current state of WIFI.
 *  @return   boolean     true=> wifi enabled, false => wifi disabled
 */
boolean wifiIsEnabled( void )
{
  return bWifiEnable;
}

/**
 *  This function returns the IP address.
 *  @return   String    IP address
 */
String wifiGetIP( void )
{
  char cBuf[34];
  IPAddress ip = WiFi.localIP();
  sprintf( cBuf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );
  String sRet( cBuf );
  return sRet;
}

/**
 *  This function returns the AccessPoint (AP) IP address.
 *  @return   String    AP IP address
 */
String wifiGetAPIP( void )
{
  char cBuf[34];
  IPAddress ip = WiFi.softAPIP();
  sprintf( cBuf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );
  String sRet( cBuf );
  return sRet;
}

/**
 * This function returns the force enable flag.
 */
boolean wifiGetForceEnable( void )
{
  return bWifiForceEnable;
}

/**
 *  This function sets/clears the force enable flag.
 */
void wifiSetForceEnable( boolean bState )
{
  bWifiForceEnable = bState;
}
