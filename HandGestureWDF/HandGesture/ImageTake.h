
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

//using namespace ImageTakeNS;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Media;
using namespace System;

namespace ImageTakeNS
{
	public ref class ImageTakeCls
	{		
	public:
		const IplImage* disImg;
		const IplImage* thImg;
		BitmapSource^ getImage();
		BitmapSource^ getThImage();
		bool thimgIsUpdated_b;
		bool imgIsUpdated_b;
		bool ImgValid(){return (disImg != nullptr && disImg->imageData != nullptr);};

		bool thImgValid(){return (thImg != nullptr && thImg->imageData != nullptr);};

		BitmapSource^ ImageTakeCls::Ipl2BitmapSource(const IplImage* img);
		BitmapSource^ bitmapSrc;
		ImageTakeCls(){thimgIsUpdated_b = false; imgIsUpdated_b = false;};
	};
};
