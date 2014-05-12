// main.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <hudwnd.h>
#include <config.h>
#include <dataxchg.h>
#include <sdlwrap.h>
#include <windows.h>


namespace {
void InitSDL(void);
void InitConfig(void);
bool MessageProcessing(void);

Config *config_;
}


#pragma comment(lib, "../SDL/lib/SDLmain.lib")
#pragma comment(lib, "../SDL/lib/SDL.lib")
#pragma comment(lib, "../SDL/lib/SDL_gfx.lib")


int main(int argc, char* argv[])
{
	InitSDL();
	InitConfig();

	DataXchg *dataXchg = new DataXchg(config_->GetTdsIf());
	HudWnd *hud = new HudWnd();

	while (MessageProcessing())
	{
		UAVDATA uavData;
		dataXchg->GetData(uavData);
		hud->SetUavData(uavData);

		if (!hud->Update())
		{
			std::cerr << "Error!" << std::endl;
			exit(1);
		}

		SDL_Delay(1);
	}

	delete hud;
	delete dataXchg;

	return 0;
}


namespace {

void InitSDL(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
		std::cerr << "Couldn't initialize SDL!" << std::endl;
        exit(1);
    }

	atexit(SDL_Quit); 
}


void InitConfig(void)
{
	char szAppPath[MAX_PATH] = "";
	GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	std::string filename = szAppPath;
	filename = filename.substr(0, filename.rfind("\\"));
	filename += "\\hud.xml";

	config_ = new Config(filename);
	config_->Read();
}


bool MessageProcessing(void)
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		switch (evt.type)
		{
		case SDL_QUIT:	
			return false;

		case SDL_KEYDOWN:
			if (evt.key.keysym.sym == SDLK_ESCAPE)
				return false;
			break;
		}
	}

	return true;
}

}