#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <map>
#include <string>
#include "schem.h"
using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")

std::map<int, parameterstruct> parametermap;
std::map<int, component> componentmap;
std::map<std::string, int> commandmap;

void initparametermapping(void) {
	 {
		parametermap[LEDLIGHT] = parameterstruct(globalluminosity, "led");
		parametermap[INDUCTOR] = parameterstruct(0, "inductor");
		parametermap[SWITCHCOMP] = parameterstruct(0, "switch");
		parametermap[MEGA] = parameterstruct(0, "mega");
		parametermap[DCMOTOR] = parameterstruct(globaltorque, "dcmotor");
		parametermap[RESISTOR] = parameterstruct(globalres, "resistor");
		parametermap[CAPACITOR] = parameterstruct(globalcapacity, "capacitor");
		parametermap[ULTRASONIC] = parameterstruct(40, "ultrasonic");
		parametermap[INFRARED] = parameterstruct(35, "infrared");
		parametermap[NRF] = parameterstruct(2.4, "nrf");
		parametermap[SERVO] = parameterstruct(9.4, "servo");
		parametermap[MICRO] = parameterstruct(32, "micro");
		parametermap[BUZZER] = parameterstruct(125, "buzzer");
		parametermap[NANO] = parameterstruct(125, "nano");
		parametermap[DHT11] = parameterstruct(11, "dht11");
		parametermap[NODEMCU] = parameterstruct(2.4, "nodemcu");
		parametermap[ACCELEROMETER] = parameterstruct(521, "accelerometer");
	}
}

void initcomponentmapping(void) {
	{
		componentmap[LEDLIGHT] = component(LEDLIGHT, (gridstep / 8), (gridstep * 2.1), 4, 3, ledhz, L"%dLum");
		componentmap[INDUCTOR] = component(INDUCTOR, (gridstep / 8), gridstep, 5.5, 2.1, inductorhz, L"EMF");
		componentmap[SWITCHCOMP] = component(SWITCHCOMP, (gridstep / 8), gridstep, 5.5, 1.7, switchhz, L"0 | 1");
		componentmap[MEGA] = component(MEGA, (gridstep / 8), 0, 40, 20, megahz, L"ATmega2560");
		componentmap[DCMOTOR] = component(DCMOTOR, (gridstep / 2), (gridstep * 1.5), 5, 4, dcmotorhz, L"%dτ");
		componentmap[RESISTOR] = component(RESISTOR, 0, (gridstep / 2), 4, 1, resistorhz, L"%dΩ");
		componentmap[CAPACITOR] = component(CAPACITOR, (gridstep / 8), gridstep, 4, 2, capacitorhz, L"%dμ");
		componentmap[ULTRASONIC] = component(ULTRASONIC, (gridstep / 2), (gridstep / 2), 16, 11, ultrasonichz, L"%dkHz");
		componentmap[INFRARED] = component(INFRARED, (gridstep / 2), (gridstep / 2), 5, 8, infraredhz, L"%dkHz");
		componentmap[NRF] = component(NRF, (gridstep / 2), (gridstep / 2), 6, 11, nrfhz, L"%dGHz");
		componentmap[SERVO] = component(SERVO, (gridstep / 2), (gridstep / 2), 10.8, 9.6, servo, L"%dkg/cm");
		componentmap[MICRO] = component(MICRO, (gridstep / 2), (gridstep / 2), 20, 7.2, micro, L"32u4");
		componentmap[BUZZER] = component(BUZZER, (gridstep / 2), (gridstep / 2), 7, 4.8, buzzer, L"%ddB");
		componentmap[NANO] = component(NANO, (gridstep / 2) + (gridstep / 4), (gridstep / 2), 17.2, 7.2, nano, L"Nano");
		componentmap[DHT11] = component(DHT11, (gridstep / 2) + (gridstep / 4), (gridstep / 2), 7, 11, dht11, L"DHT11");
		componentmap[NODEMCU] = component(NODEMCU, (gridstep / 2), (gridstep / 2), 20, 10, nodemcu, L"NodeMCU");
		componentmap[ACCELEROMETER] = component(ACCELEROMETER, (gridstep / 2), (gridstep / 2), 6, 8, accelerometer, L"GY_521");

		commandmap["dcmotor"] = DCMOTOR;
		commandmap["resistor"] = RESISTOR;
		commandmap["capacitor"] = CAPACITOR;
		commandmap["switch"] = SWITCHCOMP;
		commandmap["led"] = LEDLIGHT;
		commandmap["inductor"] = INDUCTOR;
		commandmap["ultrasonic"] = ULTRASONIC;
		commandmap["nano"] = NANO;
		commandmap["buzzer"] = BUZZER;
		commandmap["micro"] = MICRO;
		commandmap["mega"] = MEGA;
		commandmap["infrared"] = INFRARED;
		commandmap["servo"] = SERVO;
		commandmap["nrf"] = NRF;
		commandmap["micro"] = MICRO;
		commandmap["dht11"] = DHT11;
		commandmap["nodemcu"] = NODEMCU;
		commandmap["accelerometer"] = ACCELEROMETER;
	}
}