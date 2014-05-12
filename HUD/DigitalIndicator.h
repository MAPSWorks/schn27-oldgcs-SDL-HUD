// digitalindicator.h

#ifndef DIGITALINDICATOR_H
#define DIGITALINDICATOR_H

#include <cstdint>
#include <gauge.h>


class DigitalIndicator : public Gauge
{
public:
	typedef enum {eNoFrame, eFrame} StyleEnum;

	DigitalIndicator(SDL_Surface *screen, int x, int y, int w, int h, int precision = 0, StyleEnum style = eNoFrame);
	virtual ~DigitalIndicator(void);

	virtual void Draw(void);
	void SetValue(double value);

private:
	DigitalIndicator(void);
	
	void Init(void);

	bool initialized_;
	SDL_Surface *gauge_;

	const StyleEnum style_;
	const int precision_;
	const std::uint32_t colorF_;
	const std::uint32_t colorB_;

	double value_;
};

#endif
