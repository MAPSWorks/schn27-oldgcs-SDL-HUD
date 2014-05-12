// ahi.h

#ifndef AHI_H
#define AHI_H

#include <cstdint>
#include <gauge.h>

class Ahi : public Gauge
{
public:
	Ahi(SDL_Surface *screen, int x, int y, int w, int h);
	virtual ~Ahi();

	virtual void Draw(void);
	void SetValue(double roll, double pitch);

private:
	Ahi(void);

	void Init();
	void CreatePitchBar(void);
	void CreateRollTicks(void);

	bool initialized_;
	SDL_Surface *pitchBar_;
	SDL_Surface *rollTicks_;

	const std::uint32_t colorF_;
	const std::uint32_t colorB_;
	const std::uint32_t bgcolor_;
	const int fontHeight_;

	const double pitchFrame_;
	const double pitchTickStart0_;
	const double pitchTickStart_;
	const double pitchScale_;
	const double rollTickR1_;
	const double rollTickR2_;
	const double rollIndR1_;
	const double rollIndR2_;
	const double rollIndR2a_;

	double roll_;
	double pitch_;
};


#endif
