

using namespace System;

namespace VideoControlNS
{
	public ref class VideoControlCls
	{		
	public:
		unsigned char VideoControl;
		unsigned char videoOutGesture_ub;
		VideoControlCls(){
						isNewGesture_b = false;
						VideoControl = 0;
						videoOutGesture_ub = 0;};
		bool isNewGesture_b;
	};
};
