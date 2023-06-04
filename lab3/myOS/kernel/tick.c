#include "wallClock.h"
int system_ticks;
int HH,MM,SS,MS;
void tick(void){
	system_ticks++;
	//维护时钟，待开发
	MS++;
	getWallClock(HH,MM,SS);
	if(MS>=200){
		MS = 0;
		SS++;
		if(SS>=60){
			SS = 0;
			MM++;
			if(MM>=60){
				MM = 0;
				HH++;
				if(HH>=24)
					HH=0;
			}
		}
	}
	else MS++;
	setWallClock(HH,MM,SS);
	return;
}