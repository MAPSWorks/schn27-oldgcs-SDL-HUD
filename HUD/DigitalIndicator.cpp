// digitalindicator.h

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <sdlwrap.h>
#include <DigitalIndicator.h>


DigitalIndicator::DigitalIndicator(SDL_Surface *screen, int x, int y, int w, int h, int precision, StyleEnum style) 
	: Gauge(screen, x, y, w, h)
	, initialized_(false)
	, gauge_(NULL)
	, precision_(precision)
	, style_(style)
	, colorF_(0x00FF00FF)
	, colorB_(0x001000FF)
	, value_(0.0)
{
}

DigitalIndicator::~DigitalIndicator(void)
{
}


void DigitalIndicator::Init(void)
{
	if (initialized_)
		return;

	const SDL_Rect &rect = GetRect();

	gauge_ = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0);
	
	if (style_ == eNoFrame)
		SDL_SetColorKey(gauge_, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	initialized_ = true;
}


void DigitalIndicator::SetValue(double value)
{
	value_ = value;
}


void DigitalIndicator::Draw(void)
{
	Init();

	SDL_Rect rect = GetRect();

	if (style_ == eFrame)
	{
		boxColor(gauge_, 0, 0, rect.w - 1, rect.h - 1, colorB_);
		rectangleColor(gauge_, 0, 0, rect.w - 1, rect.h - 1, colorF_);
	}
	else
	{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = rect.w;
		r.h = rect.h;
		SDL_FillRect(gauge_, &r, 0);
	}
	
	std::ostringstream os;
	os << std::fixed << std::setprecision(precision_) << value_;
	
	if (style_ == eNoFrame)
		stringColor(gauge_, 3, 3, os.str().c_str(), colorB_);

	stringColor(gauge_, 2, 2, os.str().c_str(), colorF_);

	SDL_BlitSurface(gauge_, NULL, GetScreen(), &rect);
}


