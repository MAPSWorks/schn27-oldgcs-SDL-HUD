// config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <../pugixml/pugixml.hpp>
#include <../SerialPort/serialport.h>

class Config
{
public:
	explicit Config(const std::string &filename);
	~Config();

	void Read(void);
	
	SerialPort *GetTdsIf(void) const;

private:
	Config(void); //disabled
	SerialPort *CreateInterface(pugi::xml_node &node);

	std::string filename_;
	SerialPort *tds_;
};


#endif
