#pragma once

#include <stdint.h>


struct UavData
{
	char info[65];
	
	uint32_t nGoodCnt;
	uint32_t nErrCnt;

	int state;

	struct {
		float roll;			// ���� (�������)
		float pitch;		// ������ (�������)
		float heading;		// ���� (�������)
	} orientation;

	struct {
		float latitude;		// ������
		float longitude;	// �������
		float altitude;		// ������
		float course;		// ������� ����
		float speed;		// ������� ��������
		uint8_t nsat;			// ���������� ������������ ���������
		int16_t snlevels[12];	// ������ �������� ������������ ���������
	} position;

	struct {
		float speed;		// ��������� �������� (�/c)
		float altitude;		// ��������������� ������ (�)
		float v_speed;		// ������������ �������� (�/c)
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


	// ������ ������� ����������������
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

