// camera.cpp

#include <math.h>
#include <sdlwrap.h>
#include <Camera.h>



Camera::Camera(int width, int height)
{
	capDevices_ = setupESCAPI();

	if (capDevices_)
	{
		capture_.mWidth = width;
		capture_.mHeight = height;
		capture_.mTargetBuf = new int[width * height];
		initCapture(0, &capture_);
	
		// для картинки с камеры
		cam_ = SDL_CreateRGBSurfaceFrom(capture_.mTargetBuf, width, height, 32, width * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
	
		doCapture(0);
	}
}

Camera::~Camera(void)
{
	if (capDevices_)
	{
		for (int i = 0; i < 10 && !isCaptureDone(0); ++i)
			SDL_Delay(100);

		deinitCapture(0);

		SDL_FreeSurface(cam_);
		delete[] capture_.mTargetBuf;
	}
}

int Camera::Draw(SDL_Surface *dst, int /*force*/)
{
	if (!capDevices_)
	{
		// синий фон если нет камеры
		SDL_FillRect(dst, NULL, 0x00000080);
		return 1;
	}

	if (isCaptureDone(0))
	{
		// blit current frame
		if (dst->w != capture_.mWidth || dst->h != capture_.mHeight)
		{
			double zoomX = static_cast<double>(dst->w) / static_cast<double>(capture_.mWidth);
			double zoomY = static_cast<double>(dst->h) / static_cast<double>(capture_.mHeight);

			SDL_Surface *camZoom = zoomSurface(cam_, zoomX, zoomY, SMOOTHING_OFF);
			SDL_BlitSurface(camZoom, NULL, dst, NULL);
			SDL_FreeSurface(camZoom);
		}
		else
		{
			SDL_BlitSurface(cam_, NULL, dst, NULL);
		}
	
		// Ask for another frame
		doCapture(0);
	}

	return 1;
}




