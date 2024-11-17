/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 *
 */

//----------------------------------------------------------
//  INCLUDES

//----------------------------------------------------------
//  DEFINES
#define   BUTTON_DEBUG              0

//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS

//----------------------------------------------------------
//  LOCAL FUNCTIONS

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the note service.
 */
void buttonInit( void )
{
  #if defined BUTTON_DEBUG
    Serial.println( "buttonInit()" );
  #endif
  pinMode( BUTTON_GPIO, INPUT_PULLUP );
}

/**
 *  This function calls the NTP server.
 */
void buttonHandle( void )
{
  int nState = digitalRead( BUTTON_GPIO );
  if( 0 == nState )
  {
    wifiSetForceEnable( true );
  }
}

