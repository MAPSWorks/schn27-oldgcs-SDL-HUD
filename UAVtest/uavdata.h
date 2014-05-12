#pragma once

#include <stdint.h>


struct UavData
{
	char info[65];
	
	uint32_t nGoodCnt;
	uint32_t nErrCnt;

	int state;

	struct {
		float roll;			// крен (градусы)
		float pitch;		// тангаж (градусы)
		float heading;		// курс (градусы)
	} orientation;

	struct {
		float latitude;		// широта
		float longitude;	// долгота
		float altitude;		// высота
		float course;		// путевой угол
		float speed;		// путевая скорость
		uint8_t nsat;			// количество обозреваемых спутников
		int16_t snlevels[12];	// уровни сигналов обозреваемых спутников
	} position;

	struct {
		float speed;		// воздушная скорость (м/c)
		float altitude;		// барометрическая высота (м)
		float v_speed;		// вертикальная скорость (м/c)
		char resetalt;
	} air_data;

	struct {
		float altitude;
	} sonar;

	struct {
		uint16_t rpm;
		int16_t temp1;
		int16_t temp2;
		uint8_t fuel_level;
	} engine;

	struct {
		float distance;
		float error;
		int activeWpt;
	} track;

	struct {
		float aileron;
		float elevator;
		float rudder;
		float throttle;
		uint8_t flaps;
	} control;

	struct {
		uint32_t failures;
		float servoU;
		float servoI;
	} hardwarestate;

	struct {
		float azimuth;
		float elevation;
		float zoom;
	} oisstate;


	// данные системы электроснабжения
	enum {MAX_POWERMODULES = 7};
	struct powermodule_t {
		bool ok;
		uint8_t flags;
		float uin;
		float uout;
		float ibat;
		float iout;
		float tmod;
		float tbat;
	}
	powermodules[MAX_POWERMODULES];
};

