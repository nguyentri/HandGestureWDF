
#ifndef HAND_TRAINING_H
#define HAND_TRAINING_H

#ifdef HAND_TRAINING
	#define EXTERN
#else
	#define EXTERN extern
#endif


#define FINGER_NUM 		5
#define SAMPLE_NUM_MAX  99
#define USER_NUM		5
#define GEST_NUM		11

extern const char* trDBC_FileName_c;

typedef struct TRAININGDATA_ST
{
	uint8_t finger_num_u8;
	float angle_f[FINGER_NUM];
	float dis_f[FINGER_NUM];
	float angleFig_f[FINGER_NUM - 1];
};


EXTERN	TRAININGDATA_ST trainingData_st;

void sortArray_V(float* const arr_pc, const uint8_t arrLen_u8);

void sortFingers_V(CvPoint arr_pc[], const uint8_t arrLen_u8);

int createDBC_s32(const IplImage*	input_image);


#endif