// joystick.h

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <string>
#include <vector>
#include <../SDL/include/SDL.h>

namespace pugi {
class xml_node;
}


class Joystick
{
public:
	enum {Aileron = 0, Elevator, Rudder, Throttle};
	enum {Flaps0 = 0, Flaps1, Flaps2, OisLock, OisIR};

	explicit Joystick(const std::string &filename);
	~Joystick();

	void readCfg();
	void writeCfg();
	bool update();
	float getAxis(int axis) const;
	bool getButton(int button) const;
	bool getButtonEvt(int button);

	std::vector<std::string> getList() const;
	int getNumOfAxes() const;
	int getNumOfButtons() const;

	int getDevIndex() const;
	int getAxisIndex(int axis) const;
	bool getAxisInv(int axis) const;
	int getButtonIndex(int button) const;
	int getDeadZone() const;

	void setDevIndex(int index);
	void setName(const std::string &name);
	void setAxisIndex(int axis, int index);
	void setAxisInv(int axis, bool value);
	void setButtonIndex(int button, int index);
	void setDeadZone(int value);

private:
	Joystick();
	float applyDeadZone(float value) const;
	int getAxisByName(const std::string &name) const;
	int getButtonByName(const std::string &name) const;

	const std::string filename_;

	int devIndex_;
	int axesIndex_[4];
	bool axesInv_[4];
	int buttonsIndex_[8];
	float deadZone_;

	SDL_Joystick *joy_;
	float axes_[4];
	bool buttons_[8];
	bool buttonsEvt_[8];
};


#endif
