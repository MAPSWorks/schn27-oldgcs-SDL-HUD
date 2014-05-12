// gauge.cpp

#include <Gauge.h>

Gauge::Gauge(SDL_Surface *screen, int x, int y, int w, int h)
	: screen_(screen)
{
	rect_.x = x;
	rect_.y = y;
	rect_.w = w;
	rect_.h = h;
}

Gauge::~Gauge(void)
{
}

SDL_Surface * Gauge::GetScreen(void) const
{
	return screen_;
}

const SDL_Rect & Gauge::GetRect(void) const
{
	return rect_;
}