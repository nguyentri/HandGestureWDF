
#ifndef HAND_TRAINING
#define HAND_TRAINING


/*standard includes */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*OpenCV includes */
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/ml/ml.hpp"

/*Application includes */
#include "HandSegm.h"
#include <HandTranning.h>
#include <HandFeatEx.h>
#include <PointFunctions.h>

FILE* g_trnDBC_pfi;

const char* trDBC_FileName_c = ".\\training_data\\dbc\\dbc.csv";




//const char* d_65cm 	= "d065";
//const char* d_70cm 	= "d700";
//const char* d_100cm = "d100";
const char* fname[DEPTH_NUM] = {"d792"};
const uint16_t  depth_u16[DEPTH_NUM] = {792};
char t_temp_c[3];

void sortArray_V(float* const arr_pc, const uint8_t arrLen_u8)
{
   uint8_t u8_len = arrLen_u8;
   uint8_t i, j;
   float temp;
   for(i=0;i<u8_len;i++)
    {
        for(j=i;j<u8_len;j++)
        {
            if(arr_pc[i] > arr_pc[j])
            {
                temp=arr_pc[i];
                arr_pc[i]=arr_pc[j];
                arr_pc[j]=temp;
            }
        }
    }
}


void sortFingers_V(CvPoint* arr_pc, const uint8_t arrLen_u8)
{
   uint8_t u8_len = arrLen_u8;
   uint8_t i, j;
   CvPoint temp;
   for(i=0;i<u8_len;i++)
    {
        for(j=i;j<u8_len;j++)
        {
            if(arr_pc[i].x > arr_pc[j].x && (arr_pc[j].x != 0 && arr_pc[j].y != 0))
            {
				temp = arr_pc[i];
				arr_pc[i] = arr_pc[j];
				arr_pc[j] = temp;
            }
        }
    }
}


int createDBC_s32(const IplImage*	input_image)
{
	//uint8_t t_depth_u8 = 0;
	//uint8_t t_gest_name_u8 = 0;
	uint8_t t_smp_idx_u8 = 1;
	uint8_t	t_depth_idx_u8 = 0;
	uint8_t t_gest_idx_u8 = 1;

	//char t_depth_c[2];
	//char t_gest_c[2];
	char ImgFileName_c[40]	= ".\\training_data\\dxxx\\numx\\imgxx.png";
	char ImgFileName0_c[40] = ".\\training_data\\dxxx\\numx\\imgx.png";

	char DepthFileName_c[40] = ".\\training_data\\dxxx\\numx\\imgxx.txt";
	char DepthFileName0_c[40] = ".\\training_data\\dxxx\\numx\\imgx.txt";

	HandSegm HandSegObj;

	IplImage* t_imgSamp_pImg;

	cvDPoint hand_st;

	float t_dist_f = 0;
	
	g_trnDBC_pfi = fopen(trDBC_FileName_c, "w");
	//number zero database
	fprintf(g_trnDBC_pfi, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	fprintf(g_trnDBC_pfi, "\n");	
	
	if(g_trnDBC_pfi == NULL)
	{
		return (-1); //Cannot open database file. May be there is  incorrect the name of database file.
	}else{
		//fclose(g_trnDBC_pfi);	
	}

	
	while(t_depth_idx_u8 < DEPTH_NUM)
	{
		/* Get file name */
		/* Replace dxxx to folder name of depth image */
		strncpy(ImgFileName_c + 16, fname[t_depth_idx_u8], 4);
		strncpy(ImgFileName0_c + 16, fname[t_depth_idx_u8], 4);
		strncpy(DepthFileName_c + 16, fname[t_depth_idx_u8], 4);
		strncpy(DepthFileName0_c + 16, fname[t_depth_idx_u8], 4);
		while(t_gest_idx_u8 < GEST_NUM)
		{
			if(t_gest_idx_u8 < 10)
			{
				sprintf(&t_temp_c[0],"%d", t_gest_idx_u8);	
				strncpy(ImgFileName_c + 24, (const char*)&t_temp_c, 1);
				strncpy(ImgFileName0_c + 24, (const char*)&t_temp_c, 1);
				strncpy(DepthFileName_c + 24, (const char*)&t_temp_c, 1);
				strncpy(DepthFileName0_c + 24, (const char*)&t_temp_c, 1);
			}
			else
			{
				sprintf(&t_temp_c[0],"%d", t_gest_idx_u8);	
				strncpy(ImgFileName_c + 23, (const char*)&t_temp_c, 2);
				strncpy(ImgFileName0_c + 23, (const char*)&t_temp_c, 2);
				strncpy(DepthFileName_c + 23, (const char*)&t_temp_c, 2);
				strncpy(DepthFileName0_c + 23, (const char*)&t_temp_c, 2);
			}

			t_smp_idx_u8 = 0;
			while (t_smp_idx_u8 <= SAMPLE_NUM_MAX)
			{
				//g_trnDBC_pfi = fopen(trDBC_FileName_c, "a+");

				memset(&trainingData_st, 0 , sizeof(TRAININGDATA_ST));
				if(t_smp_idx_u8 < 10)
				{
					sprintf(t_temp_c,"%d", t_smp_idx_u8);
					strncpy(ImgFileName0_c + 29, (const char*)&t_temp_c, 1);
					strncpy(DepthFileName0_c + 29, (const char*)&t_temp_c, 1);

					/*open image sample */
					t_imgSamp_pImg = cvLoadImage(ImgFileName0_c, CV_LOAD_IMAGE_GRAYSCALE);
					if(t_imgSamp_pImg != NULL)
					{	
						//Get depth of sample
						FILE* depth_F = fopen(DepthFileName0_c, "r");
						if(depth_F != NULL){
							//Get the depth of hand point
							fscanf(depth_F, "%d,", &hand_st.p.x);
							fscanf(depth_F, "%d,", &hand_st.p.y);
							fscanf(depth_F, "%d,", &hand_st.d);	
						}
						fclose(depth_F);

						/*extract feature */
						HandSegObj.handPoint[0].d = hand_st.d;
						HandSegObj.handPoint[0].p = cvPoint(hand_st.p.x, hand_st.p.y);
						HandSegObj.pBinImag = t_imgSamp_pImg;
						HandSegObj.HandSegmentation(0);

						/*Map to hand gesture struct to process the hand */
						HandGestureSt.dfdisthreshold = 5000/HandSegObj.handPoint[0].d;
						HandGestureSt.HandPoint = cvPoint(HandSegObj.handPoint[0].p.x, HandSegObj.handPoint[0].p.y);
						HandGestureSt.thr_image = HandSegObj.pThImg;
						//HandGestureSt.mm_image = this->pMMImg;
						HandGestureSt.handDepth = HandSegObj.handPoint[0].d;
						HandGestureSt.RectTopHand = HandSegObj.RectTop;

						//hand processing
						handTrainingProcessing();

						//sortFingers_V(HandGestureSt.fingers, FINGER_NUM);


						//map to training data
						trainingData_st.finger_num_u8 = HandGestureSt.num_fingers;	
						/*Write to training data*/
						fprintf(g_trnDBC_pfi, "%d;", trainingData_st.finger_num_u8);
						for (int idx = 0; idx < trainingData_st.finger_num_u8; idx++)
						{
							trainingData_st.angle_f[idx] = angleToCOG(HandGestureSt.fingers[idx], HandGestureSt.hand_center_mm, HandGestureSt.contourAxisAngle);
							trainingData_st.dis_f[idx] = distanceP2P((const CvPoint*)&HandGestureSt.hand_center, (const CvPoint*)&HandGestureSt.fingers[idx]) - HandGestureSt.hand_radius;
							trainingData_st.dis_f[idx] = trainingData_st.dis_f[idx]/HandGestureSt.dfdisthreshold;
						}

						//sortArray_V((float* const)&trainingData_st.dis_f[0], (const uint8_t)FINGER_NUM);
						//sortArray_V((float* const)&trainingData_st.angle_f[0], (const uint8_t)FINGER_NUM);

						for (int idx = 0; idx < FINGER_NUM; idx++)
						{
							fprintf(g_trnDBC_pfi, "%0.3f;", trainingData_st.angle_f[idx]);
						}

						for (int idx = 0; idx < FINGER_NUM; idx++)
						{
							fprintf(g_trnDBC_pfi, "%0.3f;", trainingData_st.dis_f[idx]);
						}

						fprintf(g_trnDBC_pfi, "%d", t_gest_idx_u8);	
						fprintf(g_trnDBC_pfi, "\n");

						//fclose(g_trnDBC_pfi);

						t_smp_idx_u8++;
					}
					else
					{
						//fclose(g_trnDBC_pfi);
						t_smp_idx_u8 = 1;
						break;
					}

				}
				else //number of samples is greater than or equal to 10
				{
					sprintf(t_temp_c,"%d", t_smp_idx_u8);
					strncpy(ImgFileName_c + 29, (const char*)&t_temp_c, 2);
					strncpy(DepthFileName_c + 29, (const char*)&t_temp_c, 2);

					/*open image sample */
					t_imgSamp_pImg = cvLoadImage(ImgFileName_c, CV_LOAD_IMAGE_GRAYSCALE);
					if(t_imgSamp_pImg != NULL)
					{	
						//Get depth of sample
						FILE* depth_F = fopen(DepthFileName_c, "r");
						if(depth_F != NULL){
							//Get the depth of hand point
							fscanf(depth_F, "%d,", &hand_st.p.x);
							fscanf(depth_F, "%d,", &hand_st.p.y);
							fscanf(depth_F, "%d,", &hand_st.d);	
						}
						fclose(depth_F);

						/*extract feature */
						HandSegObj.handPoint[0].d = hand_st.d;
						HandSegObj.handPoint[0].p = cvPoint(hand_st.p.x, hand_st.p.y);
						HandSegObj.HandSegmentation(0);

						/*Map to hand gesture struct to process the hand */
						HandGestureSt.dfdisthreshold = 5000/HandSegObj.handPoint[0].d;
						HandGestureSt.HandPoint = cvPoint(HandSegObj.handPoint[0].p.x, HandSegObj.handPoint[0].p.y);
						HandGestureSt.thr_image = HandSegObj.pThImg;
						//HandGestureSt.mm_image = this->pMMImg;
						HandGestureSt.handDepth = HandSegObj.handPoint[0].d;
						HandGestureSt.RectTopHand = HandSegObj.RectTop;

						//hand processing
						handTrainingProcessing();

						//sortFingers_V(HandGestureSt.fingers, FINGER_NUM);


						//map to training data
						trainingData_st.finger_num_u8 = HandGestureSt.num_fingers;	
						/*Write to training data*/
						fprintf(g_trnDBC_pfi, "%d;", trainingData_st.finger_num_u8);
						for (int idx = 0; idx < trainingData_st.finger_num_u8; idx++)
						{
							trainingData_st.angle_f[idx] = angleToCOG(HandGestureSt.fingers[idx], HandGestureSt.hand_center_mm, HandGestureSt.contourAxisAngle);
							trainingData_st.dis_f[idx] = distanceP2P((const CvPoint*)&HandGestureSt.hand_center, (const CvPoint*)&HandGestureSt.fingers[idx]) - HandGestureSt.hand_radius;
							trainingData_st.dis_f[idx] = trainingData_st.dis_f[idx]/HandGestureSt.dfdisthreshold;
						}

						//sortArray_V((float* const)&trainingData_st.dis_f[0], (const uint8_t)FINGER_NUM);
						//sortArray_V((float* const)&trainingData_st.angle_f[0], (const uint8_t)FINGER_NUM);

						for (int idx = 0; idx < FINGER_NUM; idx++)
						{
							fprintf(g_trnDBC_pfi, "%0.3f;", trainingData_st.angle_f[idx]);
						}

						for (int idx = 0; idx < FINGER_NUM; idx++)
						{
							fprintf(g_trnDBC_pfi, "%0.3f;", trainingData_st.dis_f[idx]);
						}

						fprintf(g_trnDBC_pfi, "%d", t_gest_idx_u8);	
						fprintf(g_trnDBC_pfi, "\n");	
						t_smp_idx_u8++;
					}
					else
					{
						t_smp_idx_u8 = 1;
						break;
					}
				}
						
			}
			t_gest_idx_u8++;
		}
		t_depth_idx_u8++;
	}

	if (fclose(g_trnDBC_pfi) == 0)
	{
		return 0;
	}
	else
	{
		return (-1);
	}
}




#endif