// config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class SerialPort;

namespace pugi {
class xml_node;
}

class Config
{
public:
	explicit Config(const std::string &filename);
	~Config();

	void Read(void);
	
	SerialPort *GetFcsIf(void);
	SerialPort *GetTdsIf(void);
	bool GetDatalinkMod(void) const;
	std::string getLogName() const;

private:
	Config(void); //disabled

	SerialPort *CreateInterface(pugi::xml_node &node);

	std::string filename_;

	SerialPort *fcs_;
	SerialPort *tds_;

	bool datalinkmod_;

	std::string logName_;
};


#endif
