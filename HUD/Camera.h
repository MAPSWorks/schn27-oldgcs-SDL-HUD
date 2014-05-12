// camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <sdlwrap.h>
#include <escapi.h>

class Camera
{
public:
	Camera(int width, int height);
	~Camera(void);
	int Draw(SDL_Surface *dst, int force);

private:
	int capDevices_;
	struct SimpleCapParams capture_;
	SDL_Surface *cam_;
};

#endif
