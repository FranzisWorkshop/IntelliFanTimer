/**
 *  Project:    Zeitschaltuhr
 *  Author:     Franz Lorenz
 *  Copyright:  Franz Lorenz, Kelheim
 */

//----------------------------------------------------------
//  SYSTEM

//----------------------------------------------------------
//  DEFINES
#define   ALARM_ON                    1
#define   ALARM_OFF                   0

//----------------------------------------------------------
//  TYPEDEFs
typedef struct 
{ 
  boolean bActive;
  int iOffHour;
  int iOffMin;
}
tTimer;

//----------------------------------------------------------
//  GLOBALS
tTimer  alarmTimer        = { false, 0, 0 };

//----------------------------------------------------------
//  LOCAL FUNCTIONS

//----------------------------------------------------------
//  API FUNCTIONS

/**
 *  This function initialize the timer.
 */
void alarmInit( void )
{
  alarmTimer.bActive = false;                               //deactivate "running" timer
  relaisSet( RELAIS_APP_TIMER, false );                     //reset
}

/**
 *  This function returns the state of timer.
 *  @return boolean true => timer is active, false => timer stopped
 */
boolean alarmIsTimerActive( void )
{
  return alarmTimer.bActive;
}

/**
 *  This function checks the state of alarm.
 *  @return   int     see ALARM_xxx defines
 */
int alarmHandle( int iHour, int iMin )
{
  int iRet = ALARM_OFF;
  if( alarmTimer.bActive )
  {
    iRet = ALARM_ON;
    if( iHour >= alarmTimer.iOffHour )
    {
      if( iMin >= alarmTimer.iOffMin )
      {
        alarmTimer.bActive = false;
        iRet = ALARM_OFF;
      }
    }
  }
  //
  relaisSet( RELAIS_APP_TIMER, ( ALARM_ON == iRet ) );
  //
  return iRet;
}

/**
 *  This function sets a timer.
 *  @param  iMinutes    timer from now to now+iMinutes     
 */
void alarmSetTimer( int iMinutes )
{
  if( iMinutes > 0 )
  {
    alarmTimer.iOffHour = datetimeHour();
    alarmTimer.iOffMin  = datetimeMin() + iMinutes;
    while( alarmTimer.iOffMin > 59 )
    {
      alarmTimer.iOffMin -= 60;
      alarmTimer.iOffHour++;
      if( alarmTimer.iOffHour > 23 )
        alarmTimer.iOffHour = 0;
    }
    alarmTimer.bActive = true;
  }
  else
  {
    alarmTimer.bActive = false;
  }
  relaisSet( RELAIS_APP_TIMER, alarmTimer.bActive );
}

/**
 *  This function returns the current state of the 
 *  timer.
 *  @return   String    gets the current state
 */
String alarmGetTimer( void )
{
  String sRet = "AUS";
  if( alarmTimer.bActive )
  {
    char cBuf[20];
    sprintf( cBuf, "%02d:%02d", alarmTimer.iOffHour, alarmTimer.iOffMin );
    sRet = String( cBuf );
  }
  return sRet;  
}
