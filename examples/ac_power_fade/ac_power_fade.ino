#include <atmega328_16mhz_ac_phase_control.h>

int speed[14]={600,500,480,450,400,380,350,300,250,180,150,80,50,1};//our good stage's values

//or direct valus from 623 to 1 NOTE: MIN_AC_POWER=623 and MAX_AC_POWER=1;

void setup() {
	atmega328_16mhz_ac_phase_control.init();
//atmega328_16mhz_ac_phase_control.uninit();
}

void loop() {
  for(int i=1;i<623;i++)
  {
   atmega328_16mhz_ac_phase_control.set_ac_power(i);//speed[0 to 13] or 623 to 1 
   delay(20);
  }

  for(int i=623;i>=1;i--)
  {
   atmega328_16mhz_ac_phase_control.set_ac_power(i);//speed[0 to 13] or 623 to 1
   delay(20);
  }
  
  /*
   for(int i=0;i<=13;i++)
  {
   atmega328_16mhz_ac_phase_control.set_ac_power(speed[i]);//speed[0 to 13] or 623 to 1 
   delay(1000);
  }

  for(int i=13;i>=0;i--)
  {
   atmega328_16mhz_ac_phase_control.set_ac_power(speed[i]);//speed[0 to 13] or 623 to 1
   delay(1000);
  }
  */
}
