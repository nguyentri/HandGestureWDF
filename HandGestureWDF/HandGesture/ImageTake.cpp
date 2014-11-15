
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "ImageTake.h"
using namespace ImageTakeNS;

BitmapSource^ ImageTakeCls::getImage()
{
	//return Ipl2BitmapSource(this->disImg);
	if (ImgValid())
	{
		int nStride = ( disImg->width * 24 + 7) / 8; //Add 7 and divide by 8 to correctly round to enough bytes (10 bits will need 2 bytes)
		return BitmapSource::Create(disImg->width, disImg->height, 96, 96, PixelFormats::Bgr24, nullptr, (IntPtr)(disImg->imageData), disImg->width*disImg->height*3, nStride);
	}
	else{return nullptr;}
}

BitmapSource^ ImageTakeCls::Ipl2BitmapSource(const IplImage* img)
{
	if (ImgValid())
	{
		int nStride = ( img->width * 24 + 7) / 8; //Add 7 and divide by 8 to correctly round to enough bytes (10 bits will need 2 bytes)
		return BitmapSource::Create(img->width, img->height, 96, 96, PixelFormats::Bgr24, nullptr, (IntPtr)(img->imageData), img->width*img->height*3, nStride);
	}
	else{return nullptr;}
}

BitmapSource^ ImageTakeCls::getThImage()
{
	//return Ipl2BitmapSource(this->thImg);
	if (thImgValid())
	{
		int nStride = ( thImg->width * 8 + 7) / 8; //Add 7 and divide by 8 to correctly round to enough bytes (10 bits will need 2 bytes)
		return BitmapSource::Create(thImg->width, thImg->height, 96, 96, PixelFormats::Gray8, nullptr, (IntPtr)(thImg->imageData), thImg->width*thImg->height, nStride);
	}
	else{return nullptr;}
}


