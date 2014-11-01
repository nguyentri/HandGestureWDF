
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

using namespace cv;

/*Defines  */
#define NUMBER_OF_TRAINING_SAMPLES 138
#define ATTRIBUTES_PER_SAMPLE 11
#define NUMBER_OF_TESTING_SAMPLES 1
#define NUMBER_OF_CLASSES 1

#define GEST_CNT_MAX  2

Mat training_data = Mat(NUMBER_OF_TRAINING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
Mat training_classifications = Mat(NUMBER_OF_TRAINING_SAMPLES, 1, CV_32FC1);	
Mat testing_data = Mat(NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
Mat testing_classifications = Mat(NUMBER_OF_TESTING_SAMPLES, 1, CV_32FC1);

TRAININGDATA_ST feature_vector_st;

FILE* g_recg_pfi;

int  previous_answer = 0xffff;
int  count_answer = 0;
int  count_notanswer = 0;
bool gest_int_flag = true;


const char* recg_FileName_c = ".\\training_data\\dbc\\test.csv";

static int read_data_from_dbc(const char* filename, Mat data, Mat classes, int n_samples );

CvSVM* svm;

/***************** loads the sample database from file*******************/
//Function description: This function has responsibility for reading data from training database file and store to mat class

int read_data_from_dbc(const char* filename, Mat data, Mat classes, int n_samples )
{
	float tmp;
	// if we can't read the input file then return 0
	FILE* f = fopen( filename, "r" );
	if( !f)
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
	if(read_data_from_dbc(trDBC_FileName_c, training_data, training_classifications, NUMBER_OF_TRAINING_SAMPLES) != (-1) )
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
		svm->train(training_data, training_classifications, Mat(), Mat(), params);
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

		if (count_answer >= GEST_CNT_MAX)
		{
			gest_int_flag = true;
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

#endif