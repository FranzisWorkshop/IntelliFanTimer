/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */


#define   VERSION         "3.2"                  

//----------------------------------------------------------
//  INCLUDES
#include "config.h"                     //configuration for all following modules
#include "datetime.h"
#include "relais.h"
#include "wifi.h"
#include "alarm.h"
#include "meteo.h"
#include "websrv.h"
#include "led.h" 
#include "button.h"

//----------------------------------------------------------
//  DEFINES

//----------------------------------------------------------
//  LOCALS
int               iSec = -1;

//----------------------------------------------------------
//  ARDUINO FRAMEWORK

/**
 *  This function is called, when the system starts.
 */
void setup()
{
  ledInit();
  relaisInit();
  buttonInit();
  datetimeInit();
  //
  Serial.begin( 115200 );
  while( !Serial );
  delay( 300 );
  //
  alarmInit();
  wifiInit();
  websrvInit();
  meteoInit();
} //void setup()

/**
 *  This function is called, when the system starts.
 */
void loop()
{
  if( Serial.available() > 0 ) 
  {
    String sDebug = Serial.readString();
    int   iHour = 0;
    int   iMin = 0;
    sDebug.trim();
    if( sDebug == "timestop" )
      datetimeSetUpdate( false );
    else if( sDebug == "timestart" )
      datetimeSetUpdate( true );
    else if( sDebug.startsWith( "time" ) )
    {
      datetimeSetUpdate( false );
      sscanf( sDebug.c_str(), "time%d:%d", &iHour, &iMin ) ;
      datetimeSetTime( iHour, iMin, 50 );
    }
  }  
  //
  ledHandle();
  //
  // if( DateTime.iSec != iSec )
  if( 1 )
  {
    if( datetimeHour() == WIFI_AP_SWITCHON_HOUR )
    {
      wifiSetForceEnable( false );
    }
    if( ( ( datetimeHour() >= WIFI_AP_SWITCHON_HOUR ) && ( datetimeHour() < WIFI_AP_SWITCHOFF_HOUR ) ) || ( true == wifiGetForceEnable() ) )
    {
      if( wifiIsEnabled() )
      {
        wifiHandle();
        websrvHandle();
      }
      else
      {
        wifiStartup();
      }
    }
    else 
    {
      if( wifiIsEnabled() )
        wifiShutdown();
    }
  }
  //
  if( datetimeSec() != iSec )
  {
    meteoHandle();
    meteoTrigger( datetimeHour(), datetimeMin(), datetimeSec() );
    alarmHandle( datetimeHour(), datetimeMin() );
    datetimeHandle();
    buttonHandle();
    //
    Serial.printf( "%s %02d:%02d:%02d - ", datetimeGetDayOfWeek(), datetimeHour(), datetimeMin(), datetimeSec() );
    Serial.printf( "Relais:%d - ", ( int )relaisGet() );
    Serial.printf( "WifiEnable:%d - ", ( int )wifiIsEnabled() );
    Serial.println( "" );
    //
    iSec = datetimeSec();
  }
} //void loop()
