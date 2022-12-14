#ifndef ALGORITHMFUNCTIONS
#define ALGORITHMFUNCTIONS

#endif // ALGORITHMFUNCTIONS

#include "algorithmparameters.h"
#include <vector>
#include <iostream>
using namespace std;

void asel_via_track(unsigned char *buf, int size, int width, int height,int VideoFormat,int shadow_case,int ill_norm_case,int debug_case,int stabilization_case,int privacy_case,unsigned char* meta,float pan_tilt_zoom_read[],int init_via_track,int sensitivity,bool Overlay);
void asel_via_track_release();

void asel_direct_track(unsigned char *buf, int size, int width, int height,int debug_case,unsigned char *meta,float pan_tilt_zoom_read[],float trackObj[],int init_direct_track);
void asel_direct_track_release();

void record_video(unsigned char *buf, int size, int width, int height);

void asel_bypass(unsigned char *buf, int size, int width, int height,float pan_tilt_zoom_read[],int debug_case,int stabilization_case,int privacy_case,int init_bypass,int sensitivity,int VideoFormat);
void asel_bypass_release();

void asel_pan(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,unsigned char* meta,float pan_tilt_zoom_read[],int init_pan);
void asel_pan_release();

#if HAVE_TX1
void asel_via_base(unsigned char *buf, int size, int width, int height,int VideoFormat,int shadow_case,int ill_norm_case,int debug_case,int stabilization_case,int privacy_case, unsigned char* meta,float pan_tilt_zoom_read[],bool &alarmFlag,int init_viabase,int sensitivity,bool Classification,bool Overlay);
#endif

#if HAVE_TK1
void asel_via_base(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,int stabilization_case,int privacy_case,unsigned char* meta,float pan_tilt_zoom_read[],bool &alarmFlag,int init_viabase,int sensitivity);
#endif
void asel_via_base_release();

int asel_arya_calibrate(unsigned char *buf, int size, int width, int height,int init_arya_calibration,int start_fov_calculation);
void asel_arya_calibration_release();

#if HAVE_TX1
void asel_face(unsigned char *buf, int &numFace, int debug_case,unsigned char* meta,int init_viaface, int camId, FaceDetection faceParams);
void asel_face_release();
void asel_pan_change(unsigned char *buf,int width, int height,float longitude,float latitude,int ROI_id,int init_ROI);
void asel_pan_change_release();
#endif
