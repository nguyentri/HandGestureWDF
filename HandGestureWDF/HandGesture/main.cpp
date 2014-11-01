/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - Hand HandViewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include "main.h"
#include "HandViewer.h"
#include "HandSegm.h"
#include "HandFeatEx.h"
#include "HandTranning.h"
#include "HandGestRcg.h"
//#include "VideoControl.h"

//#define GETSAMPLE

using namespace maincpp;
using namespace ImageTakeNS;
using namespace VideoControlNS;
using namespace System::Threading;


int captureImage(const IplImage* IplRawImg);

static uint16_t imgidx = 0;
static char ImgFileName_c[40] = ".\\training_data\\image\\imgxxx.png";
static char InfImgFName_c[40] = ".\\training_data\\image\\imgxxx.txt";
extern char t_temp_c[3];

int exmainCpp::mainCpp(ImageTakeNS::ImageTakeCls^ imgTkObj, VideoControlCls^ videoControlObj)
{
	unsigned char key;

	/*Init camera */
	openni::Status rc = openni::STATUS_OK;
	/*Declare objects */

	HandViewer HandViewerObj("Hand Gesture", "Main Window", "Threshold Image");
	HandSegm HandSegmObj;

	/*Init kinect openni/ open cv */
	rc = HandViewerObj.Init();
	/* Check if init is ok? */
	if (rc != openni::STATUS_OK)
	{
		return 1;
	}
	
	/*Hand processing initialization. */
//	init_recording(&HandGestureSt);
	//init_windows();
	init_HandGestureSt(&HandGestureSt);

	handRecognition_Init();

	/*Infinite loop to process hand */
	for(;;)
	{
		key = videoControlObj->VideoControl;
		HandViewerObj.KeyBoard(key, NULL, NULL);

		if (HandViewerObj.training_flag == FALSE)
		{
			/*ReadImages Image */
			HandViewerObj.ReadImages();

			/*Detect hand */
			HandSegmObj.HandSegmInit(HandViewerObj.pBiDepthImg, 
										HandViewerObj.pDisplayImg, 
										HandViewerObj.m_pHandTracker,
										&HandViewerObj.handFrame);

			/*Hand detection */
			HandSegmObj.HandDetection();

			//processing hands if hands are found
			//for (int idx = 0; idx <= HandSegmObj.GetNumHand(); idx++) {
				if(HandSegmObj.handFound[0])
				{
					//hand segmentation
					HandSegmObj.HandSegmentation(0);
					//HandSegm::~HandSegm();
					//hand feature extraction
					/*Map to hand gesture struct to process hand */
					HandGestureSt.dfdisthreshold = 5000/HandSegmObj.handPoint[0].d;
					HandGestureSt.HandPoint = cvPoint(HandSegmObj.handPoint[0].p.x, HandSegmObj.handPoint[0].p.y);
					HandGestureSt.image = HandViewerObj.pDisplayImg;
					HandGestureSt.thr_image = HandSegmObj.pThImg;
					//HandGestureSt.mm_image = this->pMMImg;
					HandGestureSt.handDepth = HandSegmObj.handPoint[0].d;
					HandGestureSt.RectTopHand = HandSegmObj.RectTop;
					//Call hand processing
					handProcessing();
					//Display Image
					//HandViewerObj.DisPlayImg(HandSegmObj.pThImg);

					//map to bitmap
					imgTkObj->disImg = HandViewerObj.pDisplayImg;
					//map gesture recognized to c# source
					videoControlObj->videoOutGesture_ub = (unsigned char)HandGestureSt.number;

				    if (videoControlObj->VideoControl == 111)
					{
						videoControlObj->VideoControl = 0xff;

						sprintf(&t_temp_c[0],"%3d", imgidx);
						for(int idx = 0; idx<3; idx++){
							if(t_temp_c[idx] == 32)
							{
								t_temp_c[idx] = 48;
							} 
						};

						strncpy(ImgFileName_c + 25, (const char*)&t_temp_c, 3);
						strncpy(InfImgFName_c + 25, (const char*)&t_temp_c, 3);
						cvSaveImage(ImgFileName_c,  HandSegmObj.pBinImag);

						//save image information
						FILE*	depth_file = fopen(InfImgFName_c, "w");

						//fseek(depth_file, ftell(depth_file), SEEK_CUR);

						fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.x);
						fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.y);
						fprintf(depth_file, "%d", HandSegmObj.handPoint[0].d);

						fclose(depth_file);

						imgidx++;
						if(imgidx == 999){

							imgidx = 0;
						}
						else{}
					}

					#ifdef GETSAMPLE
					{
						HandViewerObj.DisPlayImg(HandSegmObj.pThImg);
						cvWriteFrame(HandGestureSt.writer, HandSegmObj.img_t);//Write frame


						FILE*	depth_file = fopen("depthhand.txt", "w+" );

						fseek(depth_file, ftell(depth_file), SEEK_CUR);

						fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.x);
						fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.y);
						fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].d);

						fclose(depth_file);
					}
					#else
					{  /*
						if(videoControlObj->VideoControl == 144)
						{
							cvWriteFrame(HandGestureSt.writer, HandViewerObj.pDisplayImg);//Write frame
						} */
					}
					#endif
				}
				else// no hand found
				{
					//cvSaveImage("img.png", HandViewerObj.pDisplayImg);

					imgTkObj->disImg = HandViewerObj.pDisplayImg;

					//HandViewerObj.DisPlayImg();
				}

				imgTkObj->imgIsUpdated_b = true;
			//}
			//HandSegmObj.ReleaseImg();
			 //Sleep(10);

		}
		else
		{
			videoControlObj->VideoControl = 0xff;
			HandViewerObj.training_flag = FALSE;
			if(createDBC_s32(NULL) != 0)
			{
				printf("error in training data");
			}
		}
	}
}



