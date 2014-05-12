// hudwnd.h

#ifndef HUDWND_H
#define HUDWND_H

#include <cstdint>
#include <vector>
#include <sdlwrap.h>
#include <escapi.h>
#include <AHI.h>
#include <DigitalIndicator.h>
#include <gauge.h>
#include <uavdata.h>
#include <Camera.h>

class HudWnd
{
public:
	HudWnd(void);
	~HudWnd(void);

	void SetUavData(UAVDATA &data);
	bool Update(void);

private:
	bool Init(void);

	const int scrWidth_;
	const int scrHeight_;

	bool initialized_;

	SDL_Surface *screen_;
	std::uint8_t *fontdata_;
	Camera *camera_;

	Ahi *ahi_;
	DigitalIndicator *speed_;
	DigitalIndicator *course_;
	DigitalIndicator *altitude_;
	DigitalIndicator *vspeed_;
	DigitalIndicator *speedGps_;
	DigitalIndicator *courseGps_;
	DigitalIndicator *altitudeGps_;

	typedef std::vector<Gauge *> gauges_t;
	gauges_t gauges_;
};

#endif
