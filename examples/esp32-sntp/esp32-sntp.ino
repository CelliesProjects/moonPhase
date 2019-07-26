#include<WiFi.h>
#include <moonPhase.h>

const char * wifissid = "networkname";
const char * wifipsk = "password";

moonPhase moonPhase; // include a MoonPhase instance

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print( "Connecting to " );
  Serial.println( wifissid );
  WiFi.begin( wifissid, wifipsk );
  while ( !WiFi.isConnected() )
    delay(100);

  Serial.println( " Connected. Getting time..." );

  configTzTime( "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", "0.pool.ntp.org" ); //Amsterdam, Netherlands

  struct tm timeinfo = {0};

  while ( !getLocalTime( &timeinfo, 0 ) )
    vTaskDelay( 10 / portTICK_PERIOD_MS );
}

void loop() {
  struct tm timeinfo = {0};
  getLocalTime( &timeinfo );
  Serial.print( asctime( &timeinfo ) );

  moonData_t moon; // variable to receive the data
  moon = moonPhase.getPhase();

  Serial.print( "Moon phase angle: " );
  Serial.print( moon.angle );                       // angle is a integer between 0-360
  Serial.print( " degrees. Moon surface lit: " );
  Serial.printf( "%f%%\n", moon.percentLit * 100 ); // percentLit is a real between 0-1
  Serial.println();
  delay(1000);
}
