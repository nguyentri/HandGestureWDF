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
static char ImgFileName_c[40] = ".\\data\\image\\imgxxx.png";
static char InfImgFName_c[40] = ".\\data\\image\\imgxxx.txt";
extern char t_temp_c[3];

CvFont NoticeFont;

int exmainCpp::mainCpp(ImageTakeNS::ImageTakeCls^ imgTkObj, VideoControlCls^ videoControlObj)
{
	unsigned char key;

	/*Init camera */
	openni::Status rc = openni::STATUS_OK;

	/*
	*Declare objects 
	*/
	/*Hand viewer object */
	HandViewer HandViewerObj("Hand Gesture", "Main Window", "Threshold Image");
	/*Hand segmentation object */
	HandSegm HandSegmObj;

	/* open cv */
	HandViewerObj.InitOpenCV();

	/*Hand processing initialization. */
	init_HandGestureSt(&HandGestureSt);

	/*Init font to display the letter in bitmap */
	NoticeFont = cvFont(2, 1);

	/*Init SVM to recognize gestures */
	handRecognition_Init();

	while(1)
	{
		/*Init kinect openni*/
		rc = HandViewerObj.InitOpenNI();

		/*Check the button event */
		key = videoControlObj->VideoControl;
		HandViewerObj.KeyBoard(key, NULL, NULL);

		if (rc == openni::STATUS_OK)
		{
			/*Infinite loop to process hand */
			for(;;)
			{
				/*Check the button event */
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
							HandGestureSt.dfdisthreshold = 5000/(float)HandSegmObj.handPoint[0].d;
							HandGestureSt.HandPoint = cvPoint(HandSegmObj.handPoint[0].p.x, HandSegmObj.handPoint[0].p.y);
							HandGestureSt.image = HandViewerObj.pDisplayImg;
							HandGestureSt.thr_image = HandSegmObj.pThImg;
							//HandGestureSt.mm_image = this->pMMImg;
							HandGestureSt.handDepth = HandSegmObj.handPoint[0].d;
							HandGestureSt.RectTopHand = HandSegmObj.RectTop;
							HandGestureSt.depthImg_b = HandViewerObj.depthImgFlag_b;
							//Call hand processing
							handProcessing();
							//Display Image
							//HandViewerObj.DisPlayImg(HandSegmObj.pThImg);

							//map to bitmap
							imgTkObj->disImg = HandViewerObj.pDisplayImg;

							//map gesture recognized to c# source
							videoControlObj->videoOutGesture_ub = (unsigned char)HandGestureSt.gesture;

							/*Check whether event to save the sample */
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

								strncpy(ImgFileName_c + 16, (const char*)&t_temp_c, 3);
								strncpy(InfImgFName_c + 16, (const char*)&t_temp_c, 3);
								cvSaveImage(ImgFileName_c,  HandSegmObj.pBinImag);

								//save image information
								FILE*	depth_file = fopen(InfImgFName_c, "w");
								fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.x);
								fprintf(depth_file, "%d,", HandSegmObj.handPoint[0].p.y);
								fprintf(depth_file, "%d", HandSegmObj.handPoint[0].d);

								fclose(depth_file);

								imgidx++;
								/*check maximum sample is reach */
								if(imgidx == 999){

									imgidx = 0;
								}
								else{
								}

							}
						}
						else/* no hand found */
						{
							/*Notify user waving his hand in order to system be able to recognize his hand */
							cvPutText(HandViewerObj.pDisplayImg, "Please wave your hand....", cvPoint(80, 240), &NoticeFont, GREEN);
							imgTkObj->disImg = HandViewerObj.pDisplayImg;
							videoControlObj->videoOutGesture_ub = 0xff;
						}

						imgTkObj->imgIsUpdated_b = true;
					//}
				}
				else if (HandViewerObj.training_flag == TRUE)
				{
					videoControlObj->VideoControl = 0xff;
					HandViewerObj.training_flag = FALSE;
					if(createDBC_s32(NULL) != 0)
					{
						printf("error in training data");
					}
				} 
				else{}

				if(videoControlObj->VideoControl == 200)
				{
					videoControlObj->VideoControl = 0xff;
					if(testing_s32()!= 0)
					{
						printf("error in testing data");
					}
					else{

					}
				}
				else{
				}				
			}
		}
		else if (HandViewerObj.training_flag == TRUE)
		{
			videoControlObj->VideoControl = 0xff;
			HandViewerObj.training_flag = FALSE;
			if(createDBC_s32(NULL) != 0)
			{
				printf("error in training data");
			}
		} 
		else if(videoControlObj->VideoControl == 200)
		{
			videoControlObj->VideoControl = 0xff;
			if(testing_s32()!= 0)
			{
				printf("error in testing data");
			}
			else{

			}
		}
		else{
		}
	}
}



