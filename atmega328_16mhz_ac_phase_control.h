
#ifndef atmega328_16mhz_ac_phase_control_h
#define atmega328_16mhz_ac_phase_control_h

class atmega328_16mhz_ac_phase_controlClass
{
  public:
        	void init();
		void uninit();
		void set_ac_power(int value);
};

extern atmega328_16mhz_ac_phase_controlClass atmega328_16mhz_ac_phase_control;

#endif

