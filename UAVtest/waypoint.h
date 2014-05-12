#pragma once

#include <stdint.h>

#define EMPTY_WPT	-32768

struct Waypoint {
	float latitude;
	float longitude;
	int16_t altitude;		// ���� == EMPTY_WPT, ����� ��������� ��������������
};



