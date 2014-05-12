// gauge.h

#ifndef GAUGE_H
#define GAUGE_H

#include <sdlwrap.h>

class Gauge
{
public:
	Gauge(SDL_Surface *screen, int x, int y, int w, int h);
	virtual ~Gauge(void);

	virtual void Draw(void) = 0;

protected:
	SDL_Surface * GetScreen(void) const;
	const SDL_Rect &GetRect(void) const;

private:
	Gauge(void);

	SDL_Surface *screen_;
	SDL_Rect rect_;
};

#endif
