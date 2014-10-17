
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "ImageTake.h"
using namespace ImageTakeNS;

BitmapSource^ ImageTakeCls::getImage()
{
	return Ipl2BitmapSource();
}

BitmapSource^ ImageTakeCls::Ipl2BitmapSource()
{
	int nStride = ( disImg->width * 24 + 7) / 8; //Add 7 and divide by 8 to correctly round to enough bytes (10 bits will need 2 bytes)
	return BitmapSource::Create(disImg->width, disImg->height, 96, 96, PixelFormats::Bgr24, nullptr, (IntPtr)(disImg->imageData), disImg->width*disImg->height*3, nStride);
}


