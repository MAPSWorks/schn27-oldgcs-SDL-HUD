// ahi.cpp

#include <ahi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <sdlwrap.h>


Ahi::Ahi(SDL_Surface *screen, int x, int y, int w, int h) 
	: Gauge(screen, x, y, w, h)
	, initialized_(false)
	, pitchBar_(NULL)
	, rollTicks_(NULL)
	, colorF_(0x00FF00FF)
	, colorB_(0x001000FF) 
	, bgcolor_(0x0000FF50)
	, fontHeight_(13)
	, pitchFrame_(0.15)
	, pitchTickStart0_(0.01)
	, pitchTickStart_(0.1)
	, pitchScale_(6.0)
	, rollTickR1_(0.28)
	, rollTickR2_(0.35)
	, rollIndR1_(0.05)
	, rollIndR2_(0.2)
	, rollIndR2a_(0.18)
	, roll_(0.0)
	, pitch_(0.0)
{
}

Ahi::~Ahi()
{
}

void Ahi::Init(void)
{
	if (initialized_)
		return;

	CreatePitchBar();
	CreateRollTicks();

	initialized_ = true;
}


void Ahi::SetValue(double roll, double pitch)
{
	roll_ = std::min(std::max(roll, -90.0), 90.0);
	pitch_ = std::min(std::max(pitch, -180.0), 180.0);
}

void Ahi::Draw(void)
{
	Init();

	const SDL_Rect &rect = GetRect();
	SDL_Surface *screen = GetScreen();
	
	SDL_Rect srcRect;
	SDL_Rect dstRect;

	// указатель тангажа

	srcRect.x = 0;
	srcRect.w = rect.w;
	dstRect.x = rect.x;
	dstRect.w = rect.w;
	
	int y = int((90.0 - pitch_) * pitchScale_ - rect.h / 2.0 + 0.5);

	if (y >= 0)
	{
		srcRect.y = y;
		srcRect.h = rect.h;
		dstRect.y = rect.y;
		dstRect.h = rect.h;
	}
	else
	{
		srcRect.y = 0;
		srcRect.h = rect.h + y;
		dstRect.y = -y;
		dstRect.h = rect.h + y;
	}

	SDL_BlitSurface(pitchBar_, &srcRect, screen, &dstRect);

	
	// метки крена

	dstRect.x = rect.x;
	dstRect.y = rect.y + rect.h / 2;
	dstRect.w = rect.w;
	dstRect.h = rect.h / 2;
	SDL_BlitSurface(rollTicks_, NULL, screen, &dstRect);


	// указатель крена
	
	double cosa = cos(-roll_ / 180.0 * M_PI);
	double sina = sin(-roll_ / 180.0 * M_PI);

	int x0 = rect.x + rect.w / 2;
	int y0 = rect.y + rect.h / 2;
	int dx1 = int(rect.w * rollIndR1_ * cosa + 0.5);
	int dy1 = int(rect.w * rollIndR1_ * sina + 0.5);
	int dx2 = int(rect.w * rollIndR2_ * cosa + 0.5);
	int dy2 = int(rect.w * rollIndR2_ * sina + 0.5);
	int dx3 = int(rect.w * rollIndR2a_ * sina + 0.5);
	int dy3 = int(rect.w * rollIndR2a_ * cosa + 0.5);

	lineColor(screen, x0 - dx1, y0 + dy1 + 1, x0 - dx2, y0 + dy2 + 1, colorB_);
	lineColor(screen, x0 + dx1, y0 - dy1 + 1, x0 + dx2, y0 - dy2 + 1, colorB_);
	lineColor(screen, x0 - dy1 + 1, y0 - dx1, x0 - dx3 + 1, y0 - dy3, colorB_);

	lineColor(screen, x0 - dx1, y0 + dy1, x0 - dx2, y0 + dy2, colorF_);
	lineColor(screen, x0 + dx1, y0 - dy1, x0 + dx2, y0 - dy2, colorF_);
	lineColor(screen, x0 - dy1, y0 - dx1, x0 - dx3, y0 - dy3, colorF_);
}


// метки тангажа
void Ahi::CreatePitchBar(void)
{
	const SDL_Rect &rect = GetRect();

	int w = int(rect.w * pitchFrame_ + 0.5);
	int h = int(180 * pitchScale_ + 0.5) + 2;
	
	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0);

	for (int i = 0; i <= 36; ++i)
	{
		int y = int(i * pitchScale_ * 5 + 0.5);

		if ((i & 1) == 0)
		{
			// длинная горизонтальная метка (10, 20, 30, ...)

			int dy = 0;
			if (i != 18)
			{
				dy = 4;
				if (i > 18)
					dy = -4;
			}

			int x = int(rect.w * pitchTickStart0_ + 0.5);
			lineColor(tmp, x + 1, y, x + 1, y + dy, colorB_);
			lineColor(tmp, x, y + 1, w, y + 1, colorB_);
			lineColor(tmp, x, y, x, y + dy, colorF_);
			lineColor(tmp, x, y, w, y, colorF_);

			// цифровая метка тангажа
			std::ostringstream os;
			os.width(4);
			os << 90 - i * 5;
			stringColor(tmp, 1, y - fontHeight_ + 1, os.str().c_str(), colorB_);
			stringColor(tmp, 0, y - fontHeight_, os.str().c_str(), colorF_);
		}
		else
		{
			// короткая горизонтальная метка (5, 15, 25, ...)
			int x = int(rect.w * pitchTickStart_ + 0.5);
			lineColor(tmp, x, y + 1, w, y + 1, colorB_);
			lineColor(tmp, x, y, w, y, colorF_);
		}
	}

	// пунктир нулевого тангажа

	int y = h / 2 - 1;
	int x = int(rect.w * pitchFrame_ + 0.5);

	while (x < rect.w - 8)
	{
		lineColor(tmp, x, y + 1, x + 4, y + 1, colorB_);
		lineColor(tmp, x, y, x + 4, y, colorF_);
		x += 8;
	}

	// вертикальная линия
	lineColor(tmp, w + 1, 0, w + 1, h, colorB_);
	lineColor(tmp, w, 0, w, h, colorF_);

	pitchBar_ = SDL_DisplayFormat(tmp);
	SDL_SetColorKey(pitchBar_, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	SDL_FreeSurface(tmp);
}


// метки кренов
void Ahi::CreateRollTicks(void)
{
	const SDL_Rect &rect = GetRect();

	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, rect.w, rect.h / 2, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0);

	int x0 = int(rect.w * 0.5 + 0.5);
	int y0 = 0;

	double r0 = rect.w * rollTickR1_;
	double r1 = rect.w * rollTickR2_;

	double a = 0.0;
	while (a <= 60.0)
	{
		double cosa = cos(a / 180.0 * M_PI);
		double sina = sin(a / 180.0 * M_PI);

		int x1 = x0 - int(r0 * cosa + 0.5);
		int y1 = y0 + int(r0 * sina + 0.5);
		int x2 = x0 - int(r1 * cosa + 0.5);
		int y2 = y0 + int(r1 * sina + 0.5);

		lineColor(tmp, x1, y1 + 1, x2, y2 + 1, colorB_);
		lineColor(tmp, x1, y1, x2, y2, colorF_);
 
		x1 = x0 + int(r0 * cosa + 0.5);
		x2 = x0 + int(r1 * cosa + 0.5);

		lineColor(tmp, x1, y1 + 1, x2, y2 + 1, colorB_);
		lineColor(tmp, x1, y1, x2, y2, colorF_);

		a += 15.0;
	}

	rollTicks_ = SDL_DisplayFormat(tmp);
	SDL_SetColorKey(rollTicks_, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);

	SDL_FreeSurface(tmp);
}
