#pragma once

#include <cstdint>

// данные —¬—
typedef struct tag_ADS
{
	double ind_speed;	// приборна€ скорость (м/c)
	double true_speed;	// истинна€ скорость (м/c)
	double altitude;	// барометрическа€ высота (м)
	double pressure;	// давление у земли (мм.рт.ст) задаЄтс€ вручную?
	double v_speed;		// вертикальна€ скорость (м/c)
	char resetalt;
} ADS;


// данные компаса
typedef struct tag_MAG
{
	double heading;		// магнитный курс (градусы)
	double roll;		// крен от компаса (градусы)
	double pitch;		// тангаж от компаса (градусы)
} MAG;


// данные инерциальной системы
typedef struct tag_INS
{
	double roll;		// крен (градусы)
	double pitch;		// тангаж (градусы)
	double heading;		// курс (градусы)
	double roll_speed;	// скорость изменени€ крена (градусы/c)
	double pitch_speed;	// скорость изменени€ тангажа (градусы/c)
	double heading_speed; // скорость изменени€ курса (градусы/c)
} INS;


typedef struct _TIME
{
	std::uint16_t year;
	std::uint16_t month;
	std::uint16_t day;
	std::uint16_t hours;
	std::uint16_t minutes;
	std::uint16_t seconds;
} TIME;


// данные навигационной системы
typedef struct tag_GPS
{
	double latitude;	// широта
	double longitude;	// долгота
	double altitude;	// высота
	TIME   utctime;		// врем€ со спутника
	double course;		// вычисл€емый курс
	double speed;		// вычисл€ема€ путева€ скорость

	double trackS;
	double trackZ;
	double trackCrs;

	int activeWpt;
} GPS;


// параметры силовой установки
typedef struct tag_ENG
{
	std::uint16_t rpm;
	std::int16_t temp1;
	std::int16_t temp2;
} ENG;


// управл€ющие воздействи€ от автопилота (не совсем датчик :))
typedef struct tag_AUTOCTRL
{
	float elevator;
	float aileron_l;
	float aileron_r;
	float rudder;
	float throttle;
} AUTOCTRL;


typedef struct
{
	char info[17];
	
	std::uint32_t nGoodCnt;
	std::uint32_t nErrCnt;

	double timeStamp;
	std::uint16_t flags;

	ADS Ads;		// —¬—
	MAG Mag;		// компас
	INS Ins;		// инерциальна€ система
	GPS Gps;		// спутникова€ навигаци€
	ENG Eng;		// параметры работы силовой установки

	AUTOCTRL AutoCtrl;

} UAVDATA;	
	




