#ifndef ALGORITHMFUNCTIONS
#define ALGORITHMFUNCTIONS

#endif // ALGORITHMFUNCTIONS

void asel_via_EGO(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,unsigned char* meta,int tilt_degree, int pan_degree,bool &alarmFlag);

void asel_direct_track(unsigned char *buf, int size, int width, int height,int record_case,int debug_case,unsigned char *meta,int headtype,float pan_tilt_zoom_read[],float trackObj[],int init_direct_track);
void asel_direct_track_release();

void record_video(unsigned char *buf, int size, int width, int height);

void asel_bypass(unsigned char *buf, int size, int width, int height,int debug_case,int stabilization_case,int privacy_case,int init_bypass,int sensitivity);
void asel_bypass_release(int headtype);


//void asel_pan(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,unsigned char* meta,float pan_tilt_zoom_read[],int init_pan);
void asel_pan(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,unsigned char* meta,int tilt_degree, int pan_degree);
void asel_pan_release();

void asel_via_base(unsigned char *buf, int size, int width, int height,int RGB_case,int record_case,int shadow_case,int ill_norm_case,int debug_case,int stabilization_case,int privacy_case,unsigned char* meta,float pan_tilt_zoom_read[],bool &alarmFlag,int init_viabase, int sensitivity);
void asel_via_base_release();

int asel_arya_calibrate(unsigned char *buf, int size, int width, int height,int init_arya_calibration,int start_fov_calculation);
void asel_arya_calibration_release();
