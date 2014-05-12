// config.cpp

#include "stdafx.h"
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <../pugixml/pugixml.hpp>
#include <../pugixml/pugiconfig.hpp>
#include <../SerialPort/Rs232.h>
#include <../SerialPort/UdpTunnel.h>
#include <../SerialPort/TcpTunnel.h>
#include "config.h"


using namespace pugi;


Config::Config(const std::string &filename)
	: filename_(filename)
	, fcs_(NULL)
	, tds_(NULL)
	, datalinkmod_(false)
	, logName_()
{
}

Config::~Config()
{
	if (fcs_ != NULL)
		delete fcs_;

	if (tds_ != NULL)
		delete tds_;
}


SerialPort *Config::GetFcsIf(void)
{
	SerialPort *res = fcs_;
	fcs_ = NULL;
 	return res;
}

SerialPort *Config::GetTdsIf(void)
{
	SerialPort *res = tds_;
	tds_ = NULL;
	return res;
}

bool Config::GetDatalinkMod(void) const
{
	return datalinkmod_;
}

std::string Config::getLogName() const
{
	return logName_;
}


void Config::Read(void)
{
	xml_document doc;

	if (!doc.load_file(filename_.c_str()))
		return;

	xml_node cfg = doc.child("config");

	fcs_ = CreateInterface(cfg.child("fcs"));
	tds_ = CreateInterface(cfg.child("tds"));

	xml_node mod = cfg.child("datalinkmod");

	if (!mod.empty())
		datalinkmod_ = mod.attribute("value").as_bool();

	xml_node log = cfg.child("log");

	if (!log.empty())
		logName_ = log.attribute("name").value();
}


SerialPort *Config::CreateInterface(pugi::xml_node &node)
{
	std::string name(node.attribute("iface").value());

	for (xml_node iface = node.child("iface"); !iface.empty(); iface = iface.next_sibling())
	{
		if (name != iface.attribute("name").value())
			continue;

		std::string type(iface.attribute("type").value());
		
		if (type == "serial")
		{
			return new Rs232(
				iface.attribute("port").value(), iface.attribute("baudrate").as_int(),
				100);	// timeout
		}
		else if (type == "udp")
		{
			return new UdpTunnel(
				iface.attribute("self").value(), iface.attribute("port-in").as_int(),
				iface.attribute("addr").value(), iface.attribute("port-out").as_int());
		}
		else if (type == "tcp")
		{
			return new TcpTunnel(
				iface.attribute("addr").value(), iface.attribute("port").as_int(), 
				iface.attribute("handshake").value(), TcpTunnel::CLIENT);
		}
		else if (type == "tcpserver")
		{
			return new TcpTunnel(
				iface.attribute("addr").value(), iface.attribute("port").as_int(), 
				iface.attribute("handshake").value(), TcpTunnel::SERVER);
		}
	}

	return NULL;
}
