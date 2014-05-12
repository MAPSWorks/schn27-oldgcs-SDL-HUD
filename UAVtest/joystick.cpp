// joystick.cpp

#include <map>
#include <joystick.h>
#include <../pugixml/pugixml.hpp>
#include <../pugixml/pugiconfig.hpp>

using namespace pugi;

Joystick::Joystick(const std::string &filename)
	: filename_(filename)
	, joy_(NULL)
	, devIndex_(0)
	, deadZone_(0)
{
	memset(axes_, 0, sizeof(axes_));
	memset(buttons_, 0, sizeof(buttons_));
	memset(axesIndex_, 0, sizeof(axesIndex_));
	memset(axesInv_, 0, sizeof(axesInv_));
	memset(buttonsIndex_, 0, sizeof(buttonsIndex_));
	memset(buttonsEvt_, 0, sizeof(buttonsEvt_));
}

Joystick::~Joystick()
{
	if (joy_ != NULL)
		SDL_JoystickClose(joy_);
}

void Joystick::readCfg()
{
	xml_document doc;

	if (!doc.load_file(filename_.c_str()))
		return;

	xml_node &cfg = doc.child("config");
	
	setDevIndex(cfg.child("device").attribute("index").as_int());

	xml_node &axes = cfg.child("axes");
	deadZone_ = axes.attribute("deadzone").as_int();
	for (xml_node a = axes.first_child(); !a.empty(); a = a.next_sibling())
	{
		int axis = getAxisByName(a.name());
		if (axis == -1)
			continue;
		axesIndex_[axis] = a.attribute("n").as_int();
		axesInv_[axis] = a.attribute("inverted").as_bool();
	}

	xml_node &buttons = cfg.child("buttons");
	for (xml_node b = buttons.first_child(); !b.empty(); b = b.next_sibling())
	{
		int button = getButtonByName(b.name());
		if (button == -1)
			continue;
		buttonsIndex_[button] = b.attribute("n").as_int();
	}
}

int Joystick::getAxisByName(const std::string &name) const
{
	std::map<std::string, int> axis;
	axis["aileron"] = Aileron;
	axis["elevator"] = Elevator;
	axis["rudder"] = Rudder;
	axis["throttle"] = Throttle;
	std::map<std::string, int>::const_iterator it = axis.find(name);
	return (it == axis.end()) ? -1 : it->second;
}

int Joystick::getButtonByName(const std::string &name) const
{
	std::map<std::string, int> btn;
	btn["flaps0"] = Flaps0;
	btn["flaps1"] = Flaps1;
	btn["flaps2"] = Flaps2;
	btn["oislock"] = OisLock;
	btn["oisir"] = OisIR;
	std::map<std::string, int>::const_iterator it = btn.find(name);
	return (it == btn.end()) ? -1 : it->second;
}

void Joystick::writeCfg()
{
	xml_document doc;
	xml_node root = doc.append_child();
	root.set_name("config");

	xml_node node = root.append_child();
	node.set_name("device");
	node.append_attribute("index").set_value(devIndex_);

	xml_node axes = root.append_child();
	axes.set_name("axes");
	axes.append_attribute("deadzone").set_value(deadZone_);

	const std::string axesnames[] = {"aileron", "elevator", "rudder", "throttle"};
	for (int i = 0; i < sizeof(axesnames)/sizeof(axesnames[0]); ++i)
	{
		int axis = getAxisByName(axesnames[i]);
		if (axis == -1)
			continue;
		xml_node a = axes.append_child();
		a.set_name(axesnames[i].c_str());
		a.append_attribute("n").set_value(axesIndex_[axis]);
		a.append_attribute("inverted").set_value(axesInv_[axis]);
	}

	xml_node buttons = root.append_child();
	buttons.set_name("buttons");

	const std::string buttonsnames[] = {"flaps0", "flaps1", "flaps2", "oislock", "oisir"};
	for (int i = 0; i < sizeof(buttonsnames)/sizeof(buttonsnames[0]); ++i)
	{
		int button = getButtonByName(buttonsnames[i]);
		if (button == -1)
			continue;
		xml_node b = buttons.append_child();
		b.set_name(buttonsnames[i].c_str());
		b.append_attribute("n").set_value(buttonsIndex_[button]);
	}

	doc.save_file(filename_.c_str(), "  ");
}

bool Joystick::update()
{
	if (joy_ == NULL)
		return false;

	SDL_JoystickUpdate();

	for (int i = 0; i < sizeof(axes_) / sizeof(axes_[0]); ++i)
	{
		float v = SDL_JoystickGetAxis(joy_, axesIndex_[i]) / 32768.0;
		if (axesInv_[i])
			v = -v;
		if (i != Throttle)
			v = applyDeadZone(v);
		axes_[i] = v;
	}

	for (int i = 0; i < sizeof(buttons_) / sizeof(buttons_[0]); ++i)
	{
		bool value = SDL_JoystickGetButton(joy_, buttonsIndex_[i]) != 0;
		buttonsEvt_[i] |= value && !buttons_[i];
		buttons_[i] = value;
	}

	return true;
}

float Joystick::getAxis(int axis) const
{
	return (axis >= 0 && axis < sizeof(axes_) / sizeof(axes_[0])) ? axes_[axis] : 0;
}

bool Joystick::getButton(int button) const
{
	return (button >= 0 && button < sizeof(buttons_) / sizeof(buttons_[0])) ? buttons_[button] : false;
}

bool Joystick::getButtonEvt(int button)
{
	bool res = false;

	if (button >= 0 && button < sizeof(buttons_) / sizeof(buttons_[0]))
	{
		res = buttonsEvt_[button];
		buttonsEvt_[button] = false;
	}
	
	return res;
}


std::vector<std::string> Joystick::getList() const
{
	std::vector<std::string> list;
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
		list.push_back(SDL_JoystickName(i));
	return list;
}

int Joystick::getNumOfAxes() const
{
	return SDL_JoystickNumAxes(joy_);
}

int Joystick::getNumOfButtons() const
{
	return SDL_JoystickNumButtons(joy_);
}

int Joystick::getDevIndex() const
{
	return devIndex_;
}

int Joystick::getAxisIndex(int axis) const
{
	return (axis >= 0 && axis < sizeof(axesIndex_) / sizeof(axesIndex_[0])) ? axesIndex_[axis] : -1;
}

bool Joystick::getAxisInv(int axis) const
{
	return (axis >= 0 && axis < sizeof(axesInv_) / sizeof(axesInv_[0])) ? axesInv_[axis] : false;
}

int Joystick::getButtonIndex(int button) const
{
	return (button >= 0 && button < sizeof(buttonsIndex_) / sizeof(buttonsIndex_[0])) ? buttonsIndex_[button] : -1;
}

int Joystick::getDeadZone() const
{
	return deadZone_;
}

void Joystick::setDevIndex(int index)
{
	if (joy_ != NULL)
	{
		SDL_JoystickClose(joy_);
		joy_ = NULL;
	}

	devIndex_ = index;

	joy_ = SDL_JoystickOpen(devIndex_);
}

void Joystick::setAxisIndex(int axis, int index)
{
	if ((axis >= 0 && axis < sizeof(axesIndex_) / sizeof(axesIndex_[0])))
		axesIndex_[axis] = index;
}

void Joystick::setAxisInv(int axis, bool value)
{
	if ((axis >= 0 && axis < sizeof(axesInv_) / sizeof(axesInv_[0])))
		axesInv_[axis] = value;
}

void Joystick::setButtonIndex(int button, int index)
{
	if ((button >= 0 && button < sizeof(buttonsIndex_) / sizeof(buttonsIndex_[0])))
		buttonsIndex_[button] = index;
}

void Joystick::setDeadZone(int value)
{
	deadZone_ = value;
}


float Joystick::applyDeadZone(float value) const
{
	const float dz = std::max(std::min(deadZone_ / 32768.0f, 0.1f), 0.0f);

	if (value > -dz && value < dz)
		return 0;

	return (value + ((value > 0) ? -dz : dz)) / (1 - dz);
}


