/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */

//----------------------------------------------------------
//  SYSTEM

//----------------------------------------------------------
//  DEFINES

#define   RELAIS_APP_METEO            0
#define   RELAIS_APP_TIMER            1
#define   RELAIS_APP_NUMS             2


//----------------------------------------------------------
//  TYPEDEFs

//----------------------------------------------------------
//  GLOBALS
boolean   relaisOn[RELAIS_APP_NUMS];

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the relais interface.
 */
void relaisInit( void )
{
  pinMode( RELAIS_GPIO, OUTPUT );
  digitalWrite( RELAIS_GPIO, LOW );
  for( int n=0; n<RELAIS_APP_NUMS; n++ )
    relaisOn[n] = RELAIS_STATE_DEFAULT;
}

/**
 *  This function sets the relais state.
 *  @param  bOn     on
 */
void relaisSet( int iApp, boolean bOn )
{
  boolean bRelaisOn = false;
  //
  if( ( iApp >= 0 ) && ( iApp < RELAIS_APP_NUMS ) )
    relaisOn[iApp] = bOn;
  //
  for( int n=0; n<RELAIS_APP_NUMS; n++ )
  {
    if( relaisOn[n] )
    {
      bRelaisOn = true;
      break;
    }
  }
  //
  if( bRelaisOn )
    digitalWrite( RELAIS_GPIO, HIGH );
  else
    digitalWrite( RELAIS_GPIO, LOW );
}

/**
 *  This function returns the current state.
 *  @return   boolean   state
 */
boolean relaisGet( void )
{
  boolean bRelaisOn = false;
  for( int n=0; n<RELAIS_APP_NUMS; n++ )
  {
    if( relaisOn[n] )
    {
      bRelaisOn = true;
      break;
    }
  }
  return bRelaisOn;
}
