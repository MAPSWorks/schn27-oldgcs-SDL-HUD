// hudwnd.cpp

#include <cstdio>
#include <cassert>
#include <hudwnd.h>
#include <sdlwrap.h>

HudWnd::HudWnd(void) 
	: scrWidth_(640)
	, scrHeight_(480)
	, initialized_(false)
	, screen_(NULL)
	, fontdata_(NULL)
	, camera_(NULL)
	, ahi_(NULL)
	, speed_(NULL)
	, course_(NULL)
	, altitude_(NULL)
	, vspeed_(NULL)
	, speedGps_(NULL)
	, courseGps_(NULL)
	, altitudeGps_(NULL)
{
}


HudWnd::~HudWnd(void)
{
// use smart pointers, stupid!!

	if (camera_ != NULL)
		delete camera_;

	if (fontdata_ != NULL)
		delete fontdata_;

	if (ahi_ != NULL)
		delete ahi_;

	if (speed_ != NULL)
		delete speed_;
	
	if (speedGps_ != NULL)
		delete speedGps_;

	if (course_ != NULL)
		delete course_;
	
	if (courseGps_ != NULL)
		delete courseGps_;

	if (altitude_ != NULL)
		delete altitude_;
	
	if (altitudeGps_ != NULL)
		delete altitudeGps_;

	if (vspeed_ != NULL)
		delete vspeed_;
}


bool HudWnd::Init(void)
{
	if (initialized_)
		return true;

	camera_ = new Camera(640, 480);

	screen_ = SDL_SetVideoMode(scrWidth_, scrHeight_, 32, SDL_HWSURFACE/* | SDL_DOUBLEBUF*/ /*| SDL_FULLSCREEN*/);
	
	if (screen_ == NULL)
		return false;

	// заголовок окна
	SDL_WM_SetCaption("HUD", NULL);

	// шрифт
	const int fontHeight = 13;
	FILE *f = NULL;
	fopen_s(&f, "8x13b.fnt", "r+b");

	if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		long len = ftell(f);

		fontdata_ = new uint8_t[len];

		fseek(f, 0, SEEK_SET);
		fread(fontdata_, 1, len, f);
		fclose(f);

		gfxPrimitivesSetFont(fontdata_, 8, 13);
	}


	// приборы
	int x0 = scrWidth_ / 2;
	int y0 = scrHeight_ / 2;
	int w = 256;
	int h = 256;

	ahi_ = new Ahi(screen_, x0 - w / 2, y0 - h / 2, w, h);
	gauges_.push_back(ahi_);
	speed_ = new DigitalIndicator(screen_, x0 - w / 2 - 48, y0 - h / 2 - 20, 48, fontHeight + 4, 0, DigitalIndicator::eFrame);
	gauges_.push_back(speed_);
	speedGps_ = new DigitalIndicator(screen_, x0 - w / 2 - 48, y0 + h / 2 + 20, 48, fontHeight + 4, 0, DigitalIndicator::eNoFrame);
	gauges_.push_back(speedGps_);
	course_ = new DigitalIndicator(screen_, x0 - 48 / 2, y0 - h / 2 - 20 , 48, fontHeight + 4, 0, DigitalIndicator::eNoFrame);
	gauges_.push_back(course_);
	courseGps_ = new DigitalIndicator(screen_, x0 - 48 / 2, y0 + h / 2 + 20, 48, fontHeight + 4, 0, DigitalIndicator::eNoFrame);
	gauges_.push_back(courseGps_);
	altitude_ = new DigitalIndicator(screen_, x0 + w / 2 + 16, y0 - h / 2 - 20, 48, fontHeight + 4, 0, DigitalIndicator::eFrame);
	gauges_.push_back(altitude_);
	altitudeGps_ = new DigitalIndicator(screen_, x0 + w / 2 + 16, y0 + h / 2 + 20, 48, fontHeight + 4, 0, DigitalIndicator::eNoFrame);
	gauges_.push_back(altitudeGps_);
	vspeed_ = new DigitalIndicator(screen_, x0 + w / 2 + 16, y0 - 8, 48, fontHeight + 4, 1, DigitalIndicator::eFrame);
	gauges_.push_back(vspeed_);

	initialized_ = true;

	return true;
}


void HudWnd::SetUavData(UAVDATA &data)
{
	if (!initialized_)
		return;

	ahi_->SetValue(data.Ins.roll, data.Ins.pitch);
	speed_->SetValue(data.Ads.ind_speed * 3.6);
	speedGps_->SetValue(data.Gps.speed * 3.6);
	course_->SetValue(data.Ins.heading);
	courseGps_->SetValue(data.Gps.course);
	altitude_->SetValue(data.Ads.altitude);
	altitudeGps_->SetValue(data.Gps.altitude);
	vspeed_->SetValue(data.Ads.v_speed);
	
}


bool HudWnd::Update(void)
{
	if (!Init())
		return false;

	static uint32_t ticks = 0;

	uint32_t curticks = SDL_GetTicks();
	uint32_t delta = curticks - ticks;

	// вывод изображения с камеры 
	// если прошло более 40 мс, будет выведен предыдущий кадр
	if (camera_->Draw(screen_, delta > 40))
	{
		ticks = curticks;

		// отрисовка приборов
		for (gauges_t::const_iterator it = gauges_.begin(), end = gauges_.end(); it != end; ++it)
			(*it)->Draw();

		//SDL_Flip(screen_);
		SDL_UpdateRect(screen_, 0, 0, 0, 0);
	}
		
	return true;
}


