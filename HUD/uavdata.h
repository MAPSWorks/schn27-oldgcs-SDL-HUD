#pragma once

#include <cstdint>

// ������ ���
typedef struct tag_ADS
{
	double ind_speed;	// ��������� �������� (�/c)
	double true_speed;	// �������� �������� (�/c)
	double altitude;	// ��������������� ������ (�)
	double pressure;	// �������� � ����� (��.��.��) ������� �������?
	double v_speed;		// ������������ �������� (�/c)
	char resetalt;
} ADS;


// ������ �������
typedef struct tag_MAG
{
	double heading;		// ��������� ���� (�������)
	double roll;		// ���� �� ������� (�������)
	double pitch;		// ������ �� ������� (�������)
} MAG;


// ������ ������������ �������
typedef struct tag_INS
{
	double roll;		// ���� (�������)
	double pitch;		// ������ (�������)
	double heading;		// ���� (�������)
	double roll_speed;	// �������� ��������� ����� (�������/c)
	double pitch_speed;	// �������� ��������� ������� (�������/c)
	double heading_speed; // �������� ��������� ����� (�������/c)
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


// ������ ������������� �������
typedef struct tag_GPS
{
	double latitude;	// ������
	double longitude;	// �������
	double altitude;	// ������
	TIME   utctime;		// ����� �� ��������
	double course;		// ����������� ����
	double speed;		// ����������� ������� ��������

	double trackS;
	double trackZ;
	double trackCrs;

	int activeWpt;
} GPS;


// ��������� ������� ���������
typedef struct tag_ENG
{
	std::uint16_t rpm;
	std::int16_t temp1;
	std::int16_t temp2;
} ENG;


// ����������� ����������� �� ���������� (�� ������ ������ :))
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

	ADS Ads;		// ���
	MAG Mag;		// ������
	INS Ins;		// ������������ �������
	GPS Gps;		// ����������� ���������
	ENG Eng;		// ��������� ������ ������� ���������

	AUTOCTRL AutoCtrl;

} UAVDATA;	
	




