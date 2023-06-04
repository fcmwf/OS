char  *ad_HH = (char*)0xB8F90;
char  *ad_MM = (char*)0xB8F96;
char  *ad_SS = (char*)0xB8F9C;

void setWallClock(int HH,int MM,int SS){
	int color;
	*ad_HH = ((color<<8)&0xFF00)|((HH - HH%10)/10 + 48);
	*(ad_HH+2) = ((color<<8)&0xFF00)|(HH%10 + 48);

	*(ad_HH+4) = ((color<<8)&0xFF00)|':';

	*ad_MM = ((color<<8)&0xFF00)|((MM - MM%10)/10 + 48);
	*(ad_MM+2) = ((color<<8)&0xFF00)|(MM%10 + 48);

	*(ad_MM+4) = ((color<<8)&0xFF00)|':';

	*ad_SS = ((color<<8)&0xFF00)|((SS - SS%10)/10 + 48);
	*(ad_SS+2) = ((color<<8)&0xFF00)|(SS%10 + 48);
}

void getWallClock(int HH,int MM,int SS){
	HH = *ad_HH;
	MM = *ad_MM;
	SS = *ad_SS;
}