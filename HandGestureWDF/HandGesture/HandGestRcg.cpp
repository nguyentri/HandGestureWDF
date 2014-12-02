
#ifndef HAND_RECOGNITION
#define HAND_RECOGNITION



/*standard includes */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*OpenCV includes */
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"

/*Application includes */
#include "HandTranning.h"
#include "HandGestRcg.h"
#include "HandFeatEx.h"
#include "HandSegm.h"

using namespace cv;

/*Definations  */
#define NUMBER_OF_TRAINING_SAMPLES 787
#define ATTRIBUTES_PER_SAMPLE 15
#define NUMBER_OF_TESTING_SAMPLES 1
#define NUMBER_OF_CLASSES 1

#define GEST_CNT_MAX  3
#define USER_NUM 4

Mat data = Mat(NUMBER_OF_TRAINING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
Mat training_classifications = Mat(NUMBER_OF_TRAINING_SAMPLES, 1, CV_32FC1);	
Mat testing_data = Mat(NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
Mat testing_classifications = Mat(NUMBER_OF_TESTING_SAMPLES, 1, CV_32FC1);

TRAININGDATA_ST feature_vector_st;

FILE* g_recg_pfi;

int  previous_answer = 0xffff;
int  count_answer = 0;
int  count_notanswer = 0;
bool gest_int_flag = true;

/* for testing purpose */
static const char* TestResultFName_c = ".\\data\\test\\test_res.csv";
static const char* testingUser[USER_NUM] = {"user0", "user1", "user2", "user4"};

static char TestImgFileName_c[41];	//= ".\\data\\test\\userx\\numx\\imgxx.png";
static char TestImgFileName0_c[40]; //= ".\\data\\test\\userx\\numx\\imgx.png";
static char TestDepthFileName_c[40]; //= ".\\data\\test\\userx\\numx\\imgxx.txt";
static char TestDepthFileName0_c[40]; //= ".\\data\\test\\userx\\numx\\imgx.txt";

extern char t_temp_c[3];

CvSVM* svm;

static int read_data_from_dbc(const char* filename, Mat data, Mat classes, int n_samples );

static void GRecgCopyFvToTestingMat_V(void);


/***************** loads the sample database from file*******************/
//Function description: This function has responsibility for reading data from training database file and store to mat class

int read_data_from_dbc(const char* filename, Mat data, Mat classes, int n_samples )
{
	float tmp;
	// if we can't read the input file then return 0
	FILE* f = fopen( filename, "r" );
	if(f == nullptr)
	{
		//MessageBox::Show("ERROR: cannot read data training");
		return (-1);
	}
	// for each sample in the file
	for(int line = 0; line < n_samples; line++)
	{
		// for each attribute on the line in the file
		for(int attribute = 0; attribute < (ATTRIBUTES_PER_SAMPLE + 1); attribute++)
		{
			if (attribute < ATTRIBUTES_PER_SAMPLE)
			{
				// first elements in each line are the attributes
				fscanf(f, "%f;", &tmp);
				data.at<float>(line, attribute) = tmp;
			}
			else if (attribute == ATTRIBUTES_PER_SAMPLE)// is the attribute for sample
			{
				fscanf(f, "%f;", &tmp);
				classes.at<float>(line, 0) = tmp;
			}
		}
	}

	if (fclose(f) == 0)
	{		
		return 0;
	}
	else
	{
		return (-1);
	}

}


int handRecognition_Init(void)
{
	if(read_data_from_dbc(trDBC_FileName_c, data, training_classifications, NUMBER_OF_TRAINING_SAMPLES) != (-1) )
	{
		/*Init SVM parameters*/
		struct CvSVMParams params = CvSVMParams(
				CvSVM::C_SVC, // Type of SVM, here N classes (see manual)
				CvSVM::LINEAR,  // kernel type (see manual)
				0.0,		// kernel parameter (degree) for poly kernel only
				0.0,	// kernel parameter (gamma) for poly/rbf kernel only
				0.0,	// kernel parameter (coef0) for poly/sigmoid kernel only
				10,				// SVM optimization parameter C
				0,// SVM optimization parameter nu (not used for N classe SVM)
				0,// SVM optimization parameter p (not used for N classe SVM)
				NULL,			// class wieghts (or priors)
				cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 0.000001));
				// train SVM classifier (using training data)			
		fflush(NULL);
		svm = new CvSVM;
		svm->train(data, training_classifications, Mat(), Mat(), params);
		return 0;
	}
	else
	{
		return (-1);
	}
}


int handRecognition(void)
{	
	/*g_recg_pfi = fopen(recg_FileName_c, "w");
	if(g_recg_pfi == NULL)
	{
		return (-1);
	}*/	
	GRecgCopyFvToTestingMat_V();
	//fclose(g_recg_pfi);
			
	//if (read_data_from_dbc(recg_FileName_c, testing_data, testing_classifications, NUMBER_OF_TESTING_SAMPLES) != (-1))
	{
		Mat test_sample;
		int cur_answer;
		test_sample = testing_data.row(0);
		cur_answer = svm->predict(test_sample);

		if (gest_int_flag == true)
		{
			gest_int_flag = false;
			count_answer = 0;
			count_notanswer = 0;
			previous_answer = cur_answer;
		}

		if(previous_answer == cur_answer)
		{
			count_answer++;
		}else{
			count_notanswer++;
		}

		if (count_answer > GEST_CNT_MAX)
		{
			gest_int_flag = true;
			HandGestureSt.gesture = (uint8_t)cur_answer;
			sprintf(HandGestureSt.number, "%d", cur_answer);		
		}
		else if(count_notanswer >= GEST_CNT_MAX){
			gest_int_flag = true;
		}
		else{
			/*do nothing */
		}

		return (0);		
	}
}

int testing_s32(void)
{
	//uint8_t t_depth_u8 = 0;
	//uint8_t t_gest_name_u8 = 0;
	uint8_t t_smp_idx_u8 = 1;
	uint8_t	t_testUser_num = 0;
	uint8_t t_gest_idx_u8 = 1;

	HandSegm HandSegObj;

	IplImage* t_imgSamp_pImg;

	cvDPoint hand_st;

	float t_dist_f = 0;
	
	FILE* g_testing_pfi = fopen(TestResultFName_c, "w");
	//write header
	fprintf(g_testing_pfi, "%s;%s\n", "Expected Result", "Obtained Result");
	
	if(g_testing_pfi == NULL)
	{
		return (-1); //Cannot open database file. May be there is  incorrect the name of database file.
	}else{
		//fclose(g_testing_pfi);	
	}

	
	while(t_testUser_num < USER_NUM)
	{
		//Initalize name of files
		strcpy (TestImgFileName_c, ".\\data\\test\\userx\\numx\\imgxx.png");
		strcpy (TestImgFileName0_c,".\\data\\test\\userx\\numx\\imgx.png");
		strcpy (TestDepthFileName_c, ".\\data\\test\\userx\\numx\\imgxx.txt");
		strcpy (TestDepthFileName0_c, ".\\data\\test\\userx\\numx\\imgx.txt");
		/* Get file name */
		/* Replace dxxx to folder name of depth image */
		strncpy(TestImgFileName_c + 12, testingUser[t_testUser_num], 5);
		strncpy(TestImgFileName0_c + 12, testingUser[t_testUser_num], 5);
		strncpy(TestDepthFileName_c + 12, testingUser[t_testUser_num], 5);
		strncpy(TestDepthFileName0_c + 12, testingUser[t_testUser_num], 5);

		t_gest_idx_u8 = 1;
		while(t_gest_idx_u8 < GEST_NUM)
		{
			if(t_gest_idx_u8 < 10)
			{
				sprintf(&t_temp_c[0],"%d", t_gest_idx_u8);	
				strncpy(TestImgFileName_c + 21, (const char*)&t_temp_c, 1);
				strncpy(TestImgFileName0_c + 21, (const char*)&t_temp_c, 1);
				strncpy(TestDepthFileName_c + 21, (const char*)&t_temp_c, 1);
				strncpy(TestDepthFileName0_c + 21, (const char*)&t_temp_c, 1);
			}
			else
			{
				sprintf(&t_temp_c[0],"%d", t_gest_idx_u8);	
				strncpy(TestImgFileName_c + 20, (const char*)&t_temp_c, 2);
				strncpy(TestImgFileName0_c + 20, (const char*)&t_temp_c, 2);
				strncpy(TestDepthFileName_c + 20, (const char*)&t_temp_c, 2);
				strncpy(TestDepthFileName0_c + 20, (const char*)&t_temp_c, 2);
			}

 			t_smp_idx_u8 = 1;
			while (t_smp_idx_u8 < SAMPLE_NUM_MAX)
			{
				//g_testing_pfi = fopen(trDBC_FileName_c, "a+");

				memset(&trainingData_st, 0 , sizeof(TRAININGDATA_ST));
				if(t_smp_idx_u8 < 10)
				{
					sprintf(t_temp_c,"%d", t_smp_idx_u8);
					strncpy(TestImgFileName0_c + 26, (const char*)&t_temp_c, 1);
					strncpy(TestDepthFileName0_c + 26, (const char*)&t_temp_c, 1);

					/*open image sample */
					t_imgSamp_pImg = cvLoadImage(TestImgFileName0_c, CV_LOAD_IMAGE_GRAYSCALE);
					FILE* depth_F = fopen(TestDepthFileName0_c, "r");
					if((t_imgSamp_pImg != NULL) && (depth_F != NULL))
					{	
						//Get the depth of hand point
						fscanf(depth_F, "%d,", &hand_st.p.x);
						fscanf(depth_F, "%d,", &hand_st.p.y);
						fscanf(depth_F, "%d,", &hand_st.d);
						fclose(depth_F);
				
						/*extract feature */
						HandSegObj.handPoint[0].d = hand_st.d;
						HandSegObj.handPoint[0].p = cvPoint(hand_st.p.x, hand_st.p.y);
						HandSegObj.pBinImag = t_imgSamp_pImg;
						HandSegObj.HandSegmentation(0);

						/*Map to hand gesture struct to process the hand */
						HandGestureSt.dfdisthreshold = 5000/HandSegObj.handPoint[0].d;
						HandGestureSt.HandPoint = cvPoint(HandSegObj.handPoint[0].p.x, HandSegObj.handPoint[0].p.y);

						//HandGestureSt.thr_image = HandSegObj.pThImg;
						HandGestureSt.thr_image = cvCreateImage(cvGetSize(HandSegObj.pThImg), HandSegObj.pThImg->depth,  HandSegObj.pThImg->nChannels);
						cvCopy(HandSegObj.pThImg, HandGestureSt.thr_image, NULL);

						//HandGestureSt.mm_image = this->pMMImg;
						HandGestureSt.handDepth = HandSegObj.handPoint[0].d;
						HandGestureSt.RectTopHand = HandSegObj.RectTop;

						//hand processing
						handTrainingProcessing();
						
						//map to testing matrix
						GRecgCopyFvToTestingMat_V();
						
						//find test result
						Mat tsample;
						int answer;
						tsample = testing_data.row(0);
						answer = svm->predict(tsample);
						
						/*Write expected result */
						fprintf(g_testing_pfi, "%d;", t_gest_idx_u8);
						/*Write obtained result */						
						fprintf(g_testing_pfi, "%d", answer);	
						fprintf(g_testing_pfi, "\n");
								
						//release image;
						HandSegObj.ReleaseImg();
						cvReleaseImage(&HandGestureSt.thr_image);
						/*testing sample increased */						
						t_smp_idx_u8++;
					}
					else
					{
						//fclose(g_testing_pfi);
						t_smp_idx_u8 = 1;
						break;
					}

				}
				else //number of samples is greater than or equal to 10
				{
					sprintf(t_temp_c,"%d", t_smp_idx_u8);
					strncpy(TestImgFileName_c + 26, (const char*)&t_temp_c, 2);
					strncpy(TestDepthFileName_c + 26, (const char*)&t_temp_c, 2);

					/*open image sample */
					t_imgSamp_pImg = cvLoadImage(TestImgFileName0_c, CV_LOAD_IMAGE_GRAYSCALE);
					//Get depth of sample
					FILE* depth_F = fopen(TestDepthFileName0_c, "r");
					if((t_imgSamp_pImg != NULL) && (depth_F != NULL))
					{	
						//Get the depth of hand point
						fscanf(depth_F, "%d,", &hand_st.p.x);
						fscanf(depth_F, "%d,", &hand_st.p.y);
						fscanf(depth_F, "%d,", &hand_st.d);	
						fclose(depth_F);

						/*extract feature */
						HandSegObj.handPoint[0].d = hand_st.d;
						HandSegObj.handPoint[0].p = cvPoint(hand_st.p.x, hand_st.p.y);
						HandSegObj.pBinImag = t_imgSamp_pImg;
						HandSegObj.HandSegmentation(0);

						/*Map to hand gesture struct to process the hand */
						HandGestureSt.dfdisthreshold = 5000/HandSegObj.handPoint[0].d;
						HandGestureSt.HandPoint = cvPoint(HandSegObj.handPoint[0].p.x, HandSegObj.handPoint[0].p.y);

						//HandGestureSt.thr_image = HandSegObj.pThImg;
						HandGestureSt.thr_image = cvCreateImage(cvGetSize(HandSegObj.pThImg), HandSegObj.pThImg->depth,  HandSegObj.pThImg->nChannels);
						cvCopy(HandSegObj.pThImg, HandGestureSt.thr_image, NULL);

						HandGestureSt.handDepth = HandSegObj.handPoint[0].d;
						HandGestureSt.RectTopHand = HandSegObj.RectTop;

						//hand processing
						handTrainingProcessing();

						//map to testing matrix
						GRecgCopyFvToTestingMat_V();
						
						//find test result
						Mat tsample;
						int answer;
						tsample = testing_data.row(0);
						answer = svm->predict(tsample);
						
						/*Write expected result */
						fprintf(g_testing_pfi, "%d;", t_gest_idx_u8);
						/*Write obtained result */						
						fprintf(g_testing_pfi, "%d", answer);	
						fprintf(g_testing_pfi, "\n");
								
						//release image;
						HandSegObj.ReleaseImg();
						cvReleaseImage(&HandGestureSt.thr_image);
						
						/*testing sample increased */						
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
		t_testUser_num++;
	}

	if (fclose(g_testing_pfi) == 0)
	{
		return 0;
	}
	else
	{
		return (-1);
	}
}

void GRecgCopyFvToTestingMat_V(void)
{
	memset(&feature_vector_st, 0, sizeof(TRAININGDATA_ST));
	//map to training data
	feature_vector_st.finger_num_u8 = HandGestureSt.num_fingers;	
	/*Write to training data*/
	//fprintf(g_recg_pfi, "%d;", feature_vector_st.finger_num_u8);
	testing_data.at<float>(0, 0) = (float)feature_vector_st.finger_num_u8;

	//sortFingers_V(HandGestureSt.fingers, FINGER_NUM);

	for (int idx = 0; idx < feature_vector_st.finger_num_u8; idx++)
	{
		feature_vector_st.angle_f[idx] = angleToCOG(HandGestureSt.fingers[idx], HandGestureSt.hand_center_mm, HandGestureSt.contourAxisAngle);
		feature_vector_st.dis_f[idx] = distanceP2P((const CvPoint*)&HandGestureSt.hand_center, (const CvPoint*)&HandGestureSt.fingers[idx]) - HandGestureSt.hand_radius;
		feature_vector_st.dis_f[idx] = feature_vector_st.dis_f[idx]/HandGestureSt.dfdisthreshold;
		if(idx < feature_vector_st.finger_num_u8 - 1)
		{
			feature_vector_st.angleFig_f[idx] = getAngle(&HandGestureSt.fingers[idx], &HandGestureSt.hand_center, &HandGestureSt.fingers[idx+1]);
		}	
	}

	//sortArray_V((float* const)&feature_vector_st.dis_f[0], (const uint8_t)FINGER_NUM);
	//sortArray_V((float* const)&feature_vector_st.angle_f[0], (const uint8_t)FINGER_NUM);

	int idy = 0;
	int idx;
	for (idx = 1; idx <= FINGER_NUM; idx++)
	{
		testing_data.at<float>(0, idx) = feature_vector_st.angle_f[idy++];
		//fprintf(g_recg_pfi, "%0.3f;", feature_vector_st.angle_f[idx]);
	}

	idy = 0;
	for (; idx <= (FINGER_NUM*2); idx++)
	{
		testing_data.at<float>(0, idx) = feature_vector_st.dis_f[idy++];
		//fprintf(g_recg_pfi, "%0.3f;", feature_vector_st.dis_f[idx]);
	}
		
	idy = 0;
	for (; idx < (FINGER_NUM*3); idx++)
	{
		testing_data.at<float>(0, idx) = feature_vector_st.angleFig_f[idy++];
	}
}
	
#endif