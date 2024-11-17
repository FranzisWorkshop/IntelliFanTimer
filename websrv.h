/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */

//----------------------------------------------------------
//  INCLUDES

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
//----------------------------------------------------------
//  SYSTEM
#include  <WebSocketsServer.h>
#include  <ESP8266WebServer.h>


//----------------------------------------------------------
//  DEFINES

// #define   WEBSRV_DEBUG         0

//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS
int               websrvState = 0;
ESP8266WebServer  websrvServer( 80 );
WebSocketsServer  websrvSocket( 81 );

//----------------------------------------------------------
//  LOCAL FUNCTIONS

void websrvHandleRoot( void )
{
  String sState = "AUS";
  if( relaisGet() )   sState = "EIN - ";
  if( alarmIsTimerActive() )  sState += "Timer";
  //
  String sPage = "<html>\n";
  sPage += "<head>\n";
  sPage += "<meta name='viewport' content='width=device-width,initial-scale=1'>\n";
  sPage += "<link rel='manifest' href='data:application/manifest+json,{\"name\":\"L&uuml;ftung\",\"short_name\":\"L&uuml;ftung\",\"orientation\":\"portrait\",\"display\":\"standalone\"}' />\n";
  sPage += "<style>\n";
  sPage += ".area{border-radius:1vw;background-color:#eee;margin:1vw 1vw 1vw 1vw;padding:0.5em 1em 2vw 1em;}\n";
  sPage += "button{border-radius:0.5em;background-color:#ddd;width:100%;font-size:1.1em;padding:0.5em;cursor:pointer;margin-bottom:0.1em;}\n";
  sPage += ".ctr{text-align:center;}\n";
  sPage += ".AUS{background-color:green;}\n";
  sPage += ".EIN{background-color:red;}\n";
  sPage += "td{text-align:center;}\n";
  sPage += "</style>\n";
  sPage += "</head>\n";
  sPage += "<body>\n";
  sPage += "<h2 class='ctr "+sState+"' onclick='location.reload();' style='color:white;padding:0.5em;'>L&uuml;ftung - "+sState+"</h2>\n";
  //
  sPage += "<div class='area'>\n";
  sPage += "<p class='ctr'>Wetterdaten-Update am "+meteoGetUpdateDate()+" um "+meteoGetUpdateTime()+"</p>\n";
  sPage += "<table width='100%'>\n";
  sPage += "<tr><th>Uhrzeit</th><th>Temperatur</th><th>Feuchte</th><th>L&uuml;ftung</th></tr>\n";
  for( int i=0; i<24; i++ )
  {
    if( datetimeHour() == i )
      sPage += "<tr style='background-color:orange;'>";
    else if( meteoFanOn[i] > 0 )
      sPage += "<tr style='background-color:lightgreen;'>";
    else
      sPage += "<tr>";    
    sPage += "<td>";
    sPage += i; sPage += ":00</td><td>";
    sPage += meteoTemp[i];  sPage += "&deg;C</td> <td>";
    sPage += meteoHumi[i];  sPage += "%</td> <td>";
    sPage += meteoFanOn[i]; sPage += "</td> </tr>\n";
  }
  sPage += "</table>\n</div>\n<br/>\n";
  //
  sPage += "<div class='area'>\n";
  sPage += "<p class='ctr'>Timer "+alarmGetTimer()+"</p>\n";
  sPage += "<button onclick='timer(15);'>Timer auf 15 Minuten setzen</button>\n";
  sPage += "<button onclick='timer(30);'>Timer auf 30 Minuten setzen</button>\n";
  sPage += "<button onclick='timer(60);'>Timer auf 1 Stunde setzen</button>\n";
  sPage += "<button onclick='timer(0);'>Timer ausschalten</button>\n";
  sPage += "</div>\n";
  sPage += "<br/>\n";
  //
  sPage += "<br/>\n";
  sPage += "<div class='area'>\n";
  sPage += "<table width='100%'>\n";
  sPage += "<tr><td>IP Addresse</td><td>"+wifiGetIP()+"</td></tr>\n";
  sPage += "<tr><td>AP Addresse</td><td>"+wifiGetAPIP()+"</td></tr>\n";
  sPage += "<tr><td>Uhrzeit</td><td>"+datetimeGet()+"</td></tr>\n";
  sPage += "<tr><td>Timer</td><td>"+alarmGetTimer()+"</td></tr>\n";
  sPage += "<tr><td>Relais Zustand</td><td>"+sState+"</td></tr>\n";
  sPage += "</table>\n";
  sPage += "</div>\n";
  sPage += "<small>App v" VERSION " &copy; 2024 Franz Lorenz</small>\n";
  sPage += "<script>\n";
  sPage += "function click(sPar,sVal){ let sLink='/click?'+sPar+'='+sVal; var xhttp=new XMLHttpRequest(); xhttp.open('GET\',sLink,true); xhttp.send(); }\n";
  sPage += "function timer(nVal){ click(\"timer\",nVal ); setTimeout(function(){location.reload();},1000); }\n";
  sPage += "</script>\n";
  sPage += "</body>\n";
  sPage += "</html>\n";
  websrvServer.send( 200, "text/html", sPage );
}

void websrvHandleClick( void )
{
  String sDebug = "";
  for( int i=0; i < websrvServer.args(); i++ )
  {
    sDebug += "Arg no" + ( String )i + " –> ";
    sDebug += websrvServer.argName( i ) + ": ";
    sDebug += websrvServer.arg(i) + "\n";
  } 
  Serial.print( "websrvHandleClick() - " );
  Serial.println( sDebug );
  //
  if( websrvServer.argName( 0 ) == "timer" )
  {
    String sVal = websrvServer.arg( 0 );
    sVal.trim();
    Serial.println( "alarmSetTimer '"+sVal+"'" );
    alarmSetTimer( sVal.toInt() );
  }
  websrvHandleRoot();
}

/**
 *  This function returns all relevant information of the
 *  device.
 *  @return String    string with all informations
 */
String websrvSocketGet( void )
{
  String sRet = datetimeGet();
  if( relaisGet() )
    sRet += "|1";
  else
    sRet += "|0";
  if( alarmIsTimerActive() )
    sRet += "|1";
  else
    sRet += "|0";
  sRet += "|"+alarmGetTimer();
  sRet += "|"+wifiGetIP();
  sRet += "|"+wifiGetAPIP();
  return sRet;
}

/**
 *  This function handles all events form the websocket server.
 */
void websrvSocketEvent( uint8_t u8Num, WStype_t Type, uint8_t *pu8Payload, size_t nLength )
{
  Serial.printf( "webSocketEvent(%d, %d, ...)\r\n", u8Num, Type );
  switch( Type ) 
  {
    case WStype_DISCONNECTED:
      Serial.printf( "[%u] Disconnected!\r\n", u8Num );
      break;
    case WStype_CONNECTED:
      {
        Serial.printf( "[%u] Connected url: %s\r\n", u8Num, pu8Payload );
        // websrvSocket.sendTXT(num, LEDON, strlen(LEDON));
      }
      break;
    case WStype_TEXT:
      Serial.printf( "[%u] get Text: %s\r\n", u8Num, pu8Payload );
      if( 0 == strcmp( ( char* )pu8Payload, "get" ) )
      {
        String sRet = websrvSocketGet();
        websrvSocket.sendTXT( u8Num, sRet );
      }
      break;
    case WStype_BIN:
      Serial.printf( "[%u] get binary length: %u\r\n", u8Num, nLength );
      break;
    default:
      Serial.printf( "Invalid WStype [%d]\r\n", Type );
      break;
  }
}

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the NTP service.
 */
void websrvInit( void )
{
  #if defined METEO_DEBUG
    Serial.println( "websrvInit()" );
  #endif
  //
  websrvState = 0;
  websrvServer.on( "/", websrvHandleRoot );
  websrvServer.on( "/click", websrvHandleClick );
  websrvServer.begin();
  websrvSocket.begin();
  websrvSocket.onEvent( websrvSocketEvent );  
}

/**
 *  This function calls the NTP server.
 */
void websrvHandle( void )
{
  #if defined WEBSRV_DEBUG
    Serial.print( "websrvHandle() : state " ); Serial.println( websrvState );
  #endif
  websrvServer.handleClient();
  websrvSocket.loop();
  switch( websrvState )  
  {
    default:
      websrvState = 0;
      break;
  }
}
