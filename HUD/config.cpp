// config.cpp

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <../pugixml/pugixml.hpp>
#include <../pugixml/pugiconfig.hpp>
#include <../SerialPort/UdpTunnel.h>
#include <config.h>


using namespace pugi;


Config::Config(const std::string &filename)
	: filename_(filename)
	, tds_(NULL)
{
}

Config::~Config()
{
	if (tds_ != NULL)
		delete tds_;
}


SerialPort *Config::GetTdsIf(void) const
{
	return tds_;
}


void Config::Read(void)
{
	xml_document doc;

	if (!doc.load_file(filename_.c_str()))
		return;

	xml_node cfg = doc.child("config");

	tds_ = CreateInterface(cfg.child("tds"));
}


SerialPort *Config::CreateInterface(pugi::xml_node &node)
{
	std::string type(node.attribute("interface").value());

	xml_node iface = node.child(type.c_str());

	if (iface.empty())
		return NULL;
	else if (type == "udp")
	{
		return new UdpTunnel(
			iface.attribute("self").value(), iface.attribute("port-in").as_int(),
			iface.attribute("addr").value(), iface.attribute("port-out").as_int());
	}

	return NULL;
}
