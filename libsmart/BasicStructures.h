#ifndef BASICSTRUCTURES
#define BASICSTRUCTURES

#include <vector>
//#include <opencv2/opencv.hpp>

namespace aselsmart {

enum AlgorithmInitMode {
	ALGORITHM_SMART_MOTION,
	ALGORITHM_FUSION = -1,
	ALGORITHM_CHANGE_DETECTION = -2,
};

enum AlgorithmRunMode {
	AUTO_TRACK,
	SEMI_AUTO_TRACK,
	MANUEL_TRACK,
	SMART_MOTION,
	BYPASS,
};

enum VideoPixelFormat {
	PIXEL_FORMAT_YUV420,  //Y planed followed by interleaved UV planed, probably this is same as NV12
	PIXEL_FORMAT_YUV420P, //Y plane followed by U planed, followed by V plane
	PIXEL_FORMAT_YUV422,  //Y,U,V interleaved, UV is subsampled
	PIXEL_FORMAT_RGB888,
};

enum MotionDirection {
	RIGHT			= 0,
	DOWN_RIGHT,
	DOWN			= 2,
	DOWN_LEFT,
	LEFT			= 4,
	UP_LEFT,
	UP				= 6,
	UP_RIGHT,
	STOP
};

enum ObjectType {
	HUMAN,
	VEHICLE,
	OTHERS,
};

enum ObjectColor {
	DARK,
	GRAY,
	LIGHT,
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
};

struct VideoBuffer {
	unsigned char *buffer;
	int w;
	int h;
	VideoPixelFormat pixelFormat;
};

struct TrackStates {
	float gate_percentage = 5;//pt stop gate
	float centerize_time = 1.5;//seconds to get target in the center
	float scoreThreshold = 0.05;
	int track_type = 2;//among different tracker options, KCF etc.
};

struct AlgorithmStates {
	int debug = 0;
	int track = 1;
	int privacy = 0;
	int illumination_correction = 1;
	int stabilization = 0;
	int videoformat = 1;
	int shadow = 0;
	int sensitivity = 50;
	int classify = 0;
	int enhancement_type = 0;// 0: no, 1: contrast, 2: dehaze
	int enhancement_degree = 50;

	float pan_tilt_zoom_read[10];
	TrackStates track_states;
};

enum AlarmType {
	ALARM_ROI_ENTER = 1,
	ALARM_LINE_CROSS,
	ALARM_WANDERING,
	ALARM_LEFT_OBJECT,
	ALARM_STANDING,

	//Following alarms unimplemented ATM
	ALARM_ROI_LEAVE,
	ALARM_ROI_WAIT,
	ALARM_STOLEN_OBJECT,
};

struct BBox{
	int uS;
	int vS;
	int uE;
	int vE;
	AlarmType alarm_type;
	int objectID;
	ObjectType objectType;
	ObjectColor objectColor;
	MotionDirection motionDirection;
};

struct AlarmTypes {
	std::vector<BBox> object_BBs;
};

struct TrackAlarm {
	BBox track_Object;
	float panSpeed  = 0;
	float tiltSpeed = 0;
	float zoomSpeed = 0;
};

struct points {
	float u;
	float v;
};

//aa.objectType = Object::type::human;
struct ROI {
	std::vector<points> list_of_points;
	std::vector<MotionDirection> valid_directions_for_alarm;
	int wander_time=10 * 25;//in terms of frames
	int stand_time=10 * 25;//in terms of frames
	int active;
	bool updated = false;
};

struct LINE {
	std::vector<points> list_of_points;
	std::vector<float> equation;
	bool active;
	bool updated = false;
};

struct CALIBRATION {
	std::vector<points> list_of_points;
	int min_in_meter;
	int max_in_meter;
};

typedef std::vector<ROI> ROI_list;
typedef std::vector<LINE> LINE_list;
typedef std::vector<AlarmTypes> ALARM_list;

struct UIs
{
	ROI_list rois;
	LINE_list lines;
	BBox track_object;
	std::vector<points> list_of_change_points;
	int change_index;
};

struct Allinputs
{
	UIs ui;
	AlgorithmStates states;
	AlgorithmRunMode run_mode;
};

struct Alloutputs
{
	ALARM_list alarms;
	AlgorithmStates states;
	TrackAlarm track_alarm;
};

void * init_algorithm(int type,void *input,VideoBuffer* vb);
void process_algorithm(void *algo, void *input, void *output, std::vector<VideoBuffer *>vb_vector);
void release_algorithm(void *algo);
void restart_algorithm(void *algo,void *input,VideoBuffer* vb);

#ifdef _LIBSMART_INTERNAL_

class BasicStructures
{
public:

	class node
	{
	public:
		node();

		float x;
		float y;


		float xS;
		float yS;
		float xE;
		float yE;

		float W;
		float H;

		float area;
		int label;
		int pre_label;
		int C[3];

		int stop_counter;

		int count_check;
		int count_time;
		float mX,mY;
		int dir;//Null:Left:Right (0:1:2), Null:Up:Down(0:1:2) -- > 0-8 (3*Left:Right + Up:Down), -1: not a valid direction
		int direction = 8;//the final decision same a dir
		int class_type;
		int color_type;
		int colorCount[10];
		int objID;
		int cnn_count;
		int cnn_visit;
		int classCount[3];
	};

	class pix
	{
	public:
		pix();

		int ped_count;
		int vec_count;
		int back_count;
	};



};

typedef BasicStructures::node node;
typedef BasicStructures::pix pix;

typedef std::vector<  int  > Dynamic1D;
typedef std::vector<  pix  > pixList;

typedef std::vector<Dynamic1D*> DynMatrix;

typedef std::vector<double*> Dynamic1Dp;

typedef std::vector< node  > nodeList;

typedef std::vector< nodeList  > DynamicList;
typedef std::vector< pixList  > typeList;

#endif

}

#ifdef _LIBSMART_INTERNAL_
#include "debug.h"
#endif

#endif //BASICSTRUCTURES
