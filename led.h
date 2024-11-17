/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */

//----------------------------------------------------------
//  SYSTEM

//----------------------------------------------------------
//  DEFINES
#if ! defined LED_GPIO
#define   LED_GPIO                2
#endif

//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS
int ledCount = 0;


//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the LED module.
 */
void ledInit( void )
{
  pinMode( LED_GPIO, OUTPUT );
  digitalWrite( LED_GPIO, LOW );
}

/**
 *  This function sets the relais state.
 *  @param  bOn     on
 */
void ledSet( boolean bOn )
{
  if( bOn )
    digitalWrite( LED_GPIO, LOW );
  else
    digitalWrite( LED_GPIO, HIGH );
}

/**
 *  This function flashes the LED for a short time ON.
 */
void ledHandle( void )
{
  int iState = ledCount / 100;
  int iTime  = ledCount % 100;
  delay( 10 );
  //
  if( 0 == iState )
  {
    if( ( relaisGet() ) && ( iTime < LED_ON_TIME_10MS ) )
      ledSet( true );
    else
      ledSet( ( boolean )( iTime < LED_OFF_TIME_10MS ) );
  }
  else if( 1 == iState )
  {
    if( ( wifiIsConnected() ) && ( iTime < LED_ON_TIME_10MS ) )
      ledSet( true );
    else
      ledSet( ( boolean )( iTime < LED_OFF_TIME_10MS ) );
  }
  else if( 2 == iState )
  {

  }
  else if( 9 == iState )
  {
    ledCount = 0 ;
  }
  //
  ledCount++;
}


