
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
		BitmapSource^ getImage();
		bool imgIsUpdated_b;
		bool ImgValid(){return (disImg != nullptr);};
		BitmapSource^ ImageTakeCls::Ipl2BitmapSource();
		BitmapSource^ bitmapSrc;
	};
};
