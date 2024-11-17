/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 *
 *  Uses the data of the https://api.open-meteo.com
 *  "temperature_2m":16.5
 *  "relative_humidity_2m":68
 */

//----------------------------------------------------------
//  SYSTEM
// also included #include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include <WiFiClientSecure.h>

//----------------------------------------------------------
//  DEFINES
#define   METEO_DEBUG             0

// index for minimum and maximum values
#define   MAX                       0
#define   MIN                       1

//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS
WiFiClientSecure  meteoWifiClient;
HTTPClient        meteoHttpClient;
int               meteoState = 0;
boolean           meteoUpdate = true;
//
String            meteoSrvRequest = "https://api.open-meteo.com/v1/forecast?latitude=" METEO_LOCATION_LATITUDE "&longitude=" METEO_LOCATION_LONGITUDE "&current=temperature_2m,relative_humidity_2m&hourly=temperature_2m,relative_humidity_2m&forecast_days=1";
String            meteoSrvGet = "";
String            &meteoData = meteoSrvGet;
String            meteoUpdateTime = "--:--";
String            meteoUpdateDate = "";
String            meteoMsg = "";
//
int               meteoTempMaxMin[2] = { 0, 100 };
int               meteoHumiMaxMin[2] = { 0, 100 };
int               meteoTemp[24];
int               meteoHumi[24];
int               meteoFanOn[24];

//----------------------------------------------------------
//  LOCAL FUNCTIONS

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the meteo service.
 */
void meteoInit( void )
{
  #if defined METEO_DEBUG
    Serial.println( "meteoInit()" );
  #endif
  meteoState = 0;
  meteoWifiClient.setInsecure();
  //
  meteoTempMaxMin[MIN] = 100;
  meteoTempMaxMin[MAX] = -100;
  meteoHumiMaxMin[MIN] = 100;
  meteoHumiMaxMin[MAX] = 0;
}

/**
 *  This function triggers a new update of the meteo data every
 *  hour (minute=0, sec=0). When the function is called, with
 *  iHour+iMin+iSec < 0, then the relais will be updated immidiately.
 *  Every 10 minutes the state of the relais is updated.
 *  It will be triggered each hour.
 *  @param  iHour   hour
 *  @param  iMin    minute
 *  @param  iSec    second
 */
void meteoTrigger( int iHour, int iMin, int iSec )
{
  //
  if( ( 0 == iMin ) && ( 0 == iSec ) )
  {
    #if defined METEO_DEBUG
      Serial.println( "meteoTrigger() - meteoState=0" );
    #endif
    meteoState = 0;
  }
  if( ( 0 == ( iMin % 10 ) ) && ( 0 == iSec ) )
  {
    #if defined METEO_DEBUG
      Serial.println( "meteoTrigger() - relaisSet()" );
    #endif
    relaisSet( RELAIS_APP_METEO, 0 != meteoFanOn[iHour] );
  }
  else if( iHour+iMin+iSec < 0 )
  {
    #if defined METEO_DEBUG
      Serial.println( "meteoTrigger() - force relaisSet()" );
    #endif
    relaisSet( RELAIS_APP_METEO, 0 != meteoFanOn[iHour] );
  }
}

/**
 *  This function returns the time, when the last 
 *  table update was done.
 *  @return   String    time
 */
String meteoGetUpdateTime( void )
{
  return meteoUpdateTime;
}

/**
 *  This function returns the date, when the last 
 *  table update was done.
 *  @return   String    date (YYYY-MM-DD)
 */
String meteoGetUpdateDate( void )
{
  return meteoUpdateDate;
}

/**
 *  This function returns the number of hours where
 *  the temperature is above the minimum temperature
 *  limit.
 *  See METEO_TEMP_LIMIT_MIN, METEO_TEMP_LIMIT_MAX
 *  in config.h!
 *  @return   int     number of hours
 */
int meteoGetHoursOfValidTemp( void )
{
  int iHours = 0;
  int iTemp = 0;
  for( int n=METEO_HOUR_START; n<= METEO_HOUR_STOP; n++ )
  {
    iTemp = meteoTemp[n];
    if( ( iTemp >= METEO_TEMP_LIMIT_MIN ) && ( iTemp <= METEO_TEMP_LIMIT_MAX ) )
      iHours++;
  }
  return iHours;
}

/**
 *  This function calls the NTP server.
 */
void meteoHandle( void )
{
  int     iTemp;
  int     iHumi;
  int     iFanOnHours;
  int     iPos1;
  int     iPos2;
  int     n;
  //
  #if defined METEO_DEBUG
    Serial.print( "meteoHandle() : state " ); Serial.println( meteoState );
  #endif
  //
  switch( meteoState )  
  {
    case 0 :
      if( wifiIsConnected() )
        meteoState++;
      break;
      //
    case 1 :
      #if defined METEO_DEBUG
        Serial.println( meteoSrvRequest );
      #endif
      meteoHttpClient.begin( meteoWifiClient, meteoSrvRequest );
      meteoState++;
      break;
      //
    case 2 :
      if( wifiIsConnected() )
      {
        if( meteoHttpClient.GET() > 0 )
          meteoState++;
      }
      break;
      //
    case 3 :
      meteoData = meteoHttpClient.getString();
      #if defined METEO_DEBUG
        Serial.println( meteoData );
      #endif
      meteoState++;
      break;
      //
    case 4 :
      meteoHttpClient.end();
      //
      meteoTempMaxMin[MIN] = 100;
      meteoTempMaxMin[MAX] = -100;
      meteoHumiMaxMin[MIN] = 100;
      meteoHumiMaxMin[MAX] = 0;
      //
      meteoState++;
      break;
      //
    case 5 :
      iPos1 = meteoData.indexOf( "time\":[" );
      if( iPos1 > 0 )
      {
        iPos1 += 8;
        iPos2 = meteoData.indexOf( "T00:00", iPos1 );
        meteoMsg = meteoData.substring( iPos1, iPos2 );
        #if defined METEO_DEBUG
          Serial.print( "MeteoDate read " );
          Serial.println( meteoMsg );
        #endif
        meteoUpdateTime = String( datetimeHour(), DEC )+":"+String( datetimeMin(), DEC );
        if( meteoMsg != meteoUpdateDate )
        {
          meteoUpdateDate = meteoMsg;
          meteoState++;
        }
        else
        {
          meteoState = 12;
        }
      }
      break;
      //
    case 6 :
      n = 0;
      iPos1 = meteoData.indexOf( "temperature_2m\":[" );
      if( iPos1 > 0 )
      {
        iPos1 += 17;
        iPos2 = meteoData.indexOf( "]", iPos1 );
        while( ( iPos1 < iPos2 ) && ( n < 24 ) )
        {
          iTemp = meteoData.substring( iPos1, iPos1+4 ).toInt();
          if( ( n >= METEO_HOUR_START ) && ( n <= METEO_HOUR_STOP ) )
          {
            if( iTemp > meteoTempMaxMin[MAX] )
              meteoTempMaxMin[MAX] = iTemp;
            if( iTemp < meteoTempMaxMin[MIN] )
              meteoTempMaxMin[MIN] = iTemp;
          }
          meteoTemp[n] = iTemp;
          n++;
          iPos1 = meteoData.indexOf( ",", iPos1+1 )+1;
        } //while()
      } //if()
      meteoState++;
      //
      #if defined METEO_DEBUG
        Serial.print( "MeteoTemp Max=" );
        Serial.print( meteoTempMaxMin[MAX] );
        Serial.print( " Min=" );
        Serial.println( meteoTempMaxMin[MIN] );
      #endif
      //
      break;
      //
    case 7 :
      // calc the minimum and maximum 
      // temperature and humidity
      n = 0;
      iPos1 = meteoData.indexOf( "relative_humidity_2m\":[" );
      if( iPos1 > 0 )
      {
        iPos1 += 23;
        iPos2 = meteoData.indexOf( "]", iPos1 );
        while( ( iPos1 < iPos2 ) && ( n < 24 ) )
        {
          iHumi = meteoData.substring( iPos1, iPos1+3 ).toInt();
          if( ( n >= METEO_HOUR_START ) && ( n <= METEO_HOUR_STOP ) )
          {
            if( iHumi > meteoHumiMaxMin[MAX] )
              meteoHumiMaxMin[MAX] = iHumi;
            if( iHumi < meteoHumiMaxMin[MIN] )
              meteoHumiMaxMin[MIN] = iHumi;
          }
          meteoHumi[n] = iHumi;
          n++;
          iPos1 = meteoData.indexOf( ",", iPos1+1 )+1;
          if( iPos1 < 0 )
            iPos1 = iPos2;
        } //while()
      } //if()
      meteoState++;
      //
      // reset fan-on array
      for( n=0; n<24; n++ )
      { meteoFanOn[n] = 0;  }
      //
      // debug output
      #if defined METEO_DEBUG
        Serial.print( "MeteoHumi Max=" );
        Serial.print( meteoHumiMaxMin[MAX] );
        Serial.print( " Min=" );
        Serial.println( meteoHumiMaxMin[MIN] );
      #endif
      //
      break;
      //
    case 8 :
      //
      if( meteoGetHoursOfValidTemp() >= METEO_FANON_HOURS_MIN )
      {
        iFanOnHours = METEO_FANON_HOURS_MIN;
        iTemp = meteoTempMaxMin[MAX];
        while( ( iFanOnHours >= 0 ) && ( iTemp >= METEO_TEMP_LIMIT_MIN ) )
        {
          for( n=METEO_HOUR_START; n<=METEO_HOUR_STOP; n++ )
          {
            if( ( meteoTemp[n] >= iTemp ) && ( 0 == meteoFanOn[n] ) )
            {
              meteoFanOn[n] = 1;
              iFanOnHours--;
            }
          } //for
          iTemp--;
        } //while( ( iFanOnHours >= 0 ) && ( iTemp >= 8 ) )
      } //if( meteoGetHoursOfValidTemp() >= METEO_FANON_HOURS_MIN )
      meteoState++;
      break;
      //
    case 9 :
      iFanOnHours = 0;
      for( n=0; n<24; n++ )
      { iFanOnHours += meteoFanOn[n]; } //for()
      if( 0 == iFanOnHours )
      {
        iFanOnHours = METEO_FANON_HOURS_MIN;
        iHumi = meteoHumiMaxMin[MIN];
        while( iFanOnHours > 0 )
        {
          for( n=METEO_HOUR_START; n<=METEO_HOUR_STOP; n++ )
          {
            if( ( meteoHumi[n]==iHumi ) && ( 0==meteoFanOn[n] ) )
            {
              meteoFanOn[n] = 2;
              if( iFanOnHours > 0 )
                iFanOnHours--;
            }
          } //for()
          iHumi++;
        } //while()
      } //if( 0 == iFanOnHours )
      meteoState++;
      break;
      //
    case 10 :
      #if defined METEO_DEBUG
        Serial.print( "MeteoFanOn " );
        for( int n=0; n<24; n++ )
        {
          Serial.print( n );
          Serial.print( ":" );
          Serial.print( meteoFanOn[n] );
          Serial.print( " " );
        }
        Serial.println( "" );
      #endif
      meteoState++;
      break;
      //
    case 11 :
      if( datetimeIsSet() )
        meteoTrigger( -1, -1, -1 );     //force update of the relais!
      meteoState++;
      break;
      //
    case 12 :
      #if defined METEO_DEBUG
        Serial.println( "save request" );
      #endif
      meteoMsg = meteoGetUpdateTime();
      meteoMsg += "|relais";
      meteoMsg += relaisGet();
      for( n=0; n<24; n++ )
      {
        meteoMsg += "|";
        meteoMsg += n;
        meteoMsg += "-";
        meteoMsg += meteoFanOn[n];
      }
      meteoHttpClient.begin( meteoWifiClient, "https://franzweb.lima-city.de/note/index.php?dat="+meteoGetUpdateDate()+"&msg="+meteoMsg+"|" );
      meteoState++;
      break;
      //
    case 13 :
      if( wifiIsConnected() )
      {
        if( meteoHttpClient.GET() > 0 )
          meteoState++;
      }
      break;
      //
    case 14 :
      #if defined METEO_DEBUG
        Serial.println( meteoHttpClient.getString() );
      #endif
      meteoState++;
      break;
      //
    case 15 :
      meteoHttpClient.end();
      meteoState++;
      break;
      //
    case 16:
      // this state is only for debugging and should not be left!
      break;
      //
    default:
      meteoState = 0;
      break;
  }
}

