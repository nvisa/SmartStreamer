#include "trackalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"
#include "applicationinfo.h"

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

TrackAlgorithmElement::TrackAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	reverseTilt = false;
	//printf("start zoom reading\n");
	ZoomLevelNo = 258;
	FILE *file_alg_params = fopen("/etc/smartstreamer/Zoom_value.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params, "%f%*[^\n]",&zoomvalues[ii]);
	fclose(file_alg_params);

	FILE *file_alg_params2 = fopen("/etc/smartstreamer/Hfov.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params2, "%f%*[^\n]",&zoomvalues[ii + ZoomLevelNo]);
	fclose(file_alg_params2);

	FILE *file_alg_params3 = fopen("/etc/smartstreamer/Vfov.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params3, "%f%*[^\n]",&zoomvalues[ii + 2*ZoomLevelNo]);
	fclose(file_alg_params3);

	printf("end zoom reading\n");
}
void TrackAlgorithmElement::zoom2degree_conversion(int zoomReadOut,float* HV_fovAngles)
{
	int index = 0;
	for(int i=0;i<ZoomLevelNo;i++)
	{
		if(zoomReadOut<=zoomvalues[i])
		{
			index = i;
			break;
		}
	}
	//printf("index: %d -- %d\n",index,zoomReadOut);
	if(index==0)//max zoom out
	{
		HV_fovAngles[3] = zoomvalues[index + ZoomLevelNo];
		HV_fovAngles[4] = zoomvalues[index + ZoomLevelNo*2];
	}
	else if(index==ZoomLevelNo-1)// max zoom in
	{
		HV_fovAngles[3] = zoomvalues[index + ZoomLevelNo];
		HV_fovAngles[4] = zoomvalues[index + ZoomLevelNo*2];
	}
	else // interpolate zoom
	{
		float w1 = zoomvalues[index] - zoomReadOut;
		float w0 = - zoomReadOut - zoomvalues[index-1];

		HV_fovAngles[3] = (w1*zoomvalues[index + ZoomLevelNo] + w0*zoomvalues[index-1 + ZoomLevelNo])/(w0+w1);
		HV_fovAngles[4] = (w1*zoomvalues[index + 2*ZoomLevelNo] + w0*zoomvalues[index-1 + 2*ZoomLevelNo])/(w0+w1);

		//printf("H-F intermadiate values: %f %f\n",zoomvalues[index + ZoomLevelNo],zoomvalues[index + 2*ZoomLevelNo]);
	}
	printf("H-F angles: %f %f\n",HV_fovAngles[3],HV_fovAngles[4]);
}
int TrackAlgorithmElement::init()
{
	v.rgb = 1;
	v.debug = 0;
	v.ill = 1;
	v.privacy = 0;
	v.record = 0;
	v.shadow = 0;
	v.stabilization = 0;
	control.initialize = 1;

	return BaseAlgorithmElement::init();
}

int TrackAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	if (mode == AUTO)
		return autoTrack(buf);
	else if (mode == MANUAL)
		return manualTrack(buf);
	else
		mDebug("No selected mode");
	return 0;
}

int TrackAlgorithmElement::release()
{
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);
	headpt->panTiltAbs(0, 0);
#if HAVE_VIA_TRACK
	if (mode == AUTO)
		asel_via_track_release();
	else if (mode == MANUAL)
		asel_direct_track_release();
	else
		mDebug("No selected mode");
#endif
	return 0;
}

int TrackAlgorithmElement::autoTrack(const RawBuffer &buf)
{

	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

	PtzpHead *headz = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0);
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);
	if (headpt == nullptr)
		headpt = headz;
	float ta = headpt->getTiltAngle();
	if (reverseTilt)
		ta *= -1;
	float panTiltZoomRead[] = {headpt->getPanAngle(), ta, 0, 12, 12};
	//zoom2degree_conversion(headz->getZoom(),panTiltZoomRead);
	float fovh = 0, fovv = 0;
//	if (!headz->getFOV(fovh, fovv)) {
//		panTiltZoomRead[3] = 2.0;
//		panTiltZoomRead[4] = 1.5;
//	}

	int fov_type = headz->getProperty(2);
	qDebug() << "fov_type is " << fov_type << headz->getProperty(61) << headz->getProperty(3);
	if (headz->getProperty(61) == 0) {
		if (headz->getProperty(3) == 0) {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 11.0, panTiltZoomRead[4] = 8.25;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.5;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.5;
			}
		} else {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 25.0, panTiltZoomRead[4] = 20.0;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.8;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.6;
			}
		}
	} else if (headz->getProperty(61) == 1) {
		if (headz->getProperty(3) == 0) {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 11.0, panTiltZoomRead[4] = 8.25;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.5;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.5;
			}
		} else {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 25.0, panTiltZoomRead[4] = 20.0;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.8;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.6;
			}
		}
	}


	qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~fov values are  are " << panTiltZoomRead[0] <<  panTiltZoomRead[1] << panTiltZoomRead[3] <<  panTiltZoomRead[4];
#if HAVE_VIA_TRACK
	asel_via_track((uchar *)buf.constData(), width * height, width , height,
				   v.rgb,v.shadow, v.ill, v.debug, v.stabilization, v.privacy,
				   control.meta,panTiltZoomRead,control.initialize,control.sensitivity,true);
#endif

	float speed_pan  = (float)control.meta[11];
	float speed_tilt = (float)control.meta[12];
	if(speed_pan>64)
		speed_pan = -(speed_pan-64);
	if(speed_tilt>64)
		speed_tilt = -(speed_tilt-64);
	if((int)control.meta[31]==0)//track score
	{
		speed_pan  = 0;
		speed_tilt = 0;
		//TODO: stop algorithm
	}
//	printf("final speeds MA track: %d %d\n",(int)speed_pan,(int)(speed_tilt));
//	headpt->panTiltAbs((float)speed_pan/63.0, (float)speed_tilt/63.0);

	qDebug() << "Pan&Tilt degree values are " << panTiltZoomRead[3] <<  panTiltZoomRead[4];
	if (control.initialize == 0 && (control.meta[31] != 0))
		headpt->panTiltDegree(panTiltZoomRead[3], panTiltZoomRead[4]);

	if (control.initialize)
		control.initialize = 0;

	return 0;
}

int TrackAlgorithmElement::semiAutoTrack(const RawBuffer &buf)
{
	return 0;
}

int TrackAlgorithmElement::manualTrack(const RawBuffer &buf)
{
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

	PtzpHead *headz = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0);
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);
	if (headpt == nullptr)
		headpt = headz;

	/* algorithm doesn't want >180 pan values */
	float ta = headpt->getTiltAngle();
	if (ta > 180)
		ta -= 360;
	if (reverseTilt)
		ta *= -1;

	/* if we do not support fov reading for some reason, 12/12 are sane defaults */
	float panTiltZoomRead[] = {headpt->getPanAngle(), ta, 0, 12, 12};
	float fovh = 0, fovv = 0;
//	if (!headz->getFOV(fovh, fovv)) {
//		panTiltZoomRead[3] = 25.0;
//		panTiltZoomRead[4] = 20.0;
//	}
	int fov_type = headz->getProperty(4);
	if (headz->getProperty(61) == 0) {
		if (headz->getProperty(3) == 0) {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 11.0, panTiltZoomRead[4] = 8.25;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.5;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.5;
			}
		} else {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 25.0, panTiltZoomRead[4] = 20.0;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.8;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.6;
			}
		}
	} else if (headz->getProperty(61) == 1) {
		if (headz->getProperty(3) == 0) {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 11.0, panTiltZoomRead[4] = 8.25;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.5;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.5;
			}
		} else {
			if (fov_type == 0) {
				panTiltZoomRead[3] = 25.0, panTiltZoomRead[4] = 20.0;
			} else if (fov_type == 1) {
				panTiltZoomRead[3] = 6.0, panTiltZoomRead[4] = 4.8;
			} else {
				panTiltZoomRead[3] = 2.0, panTiltZoomRead[4] = 1.6;
			}
		}
	}
	bool migrateDebug = false;
	if (migrateDebug) {
		mDebug("######################### ptzp head fovh=%f fovv=%f", fovv, fovh);
		zoom2degree_conversion(headz->getZoom(), panTiltZoomRead);
		mDebug("######################### cevo fovh=%f fovv=%f", panTiltZoomRead[3], panTiltZoomRead[4]);
	}

	mInfo("start tracking %f %f %f %f\n", objProp[0], objProp[1], objProp[2], objProp[3]);

#if HAVE_VIA_TRACK
	asel_direct_track((uchar *)buf.constData(), width * height, width , height,
					  v.debug, control.meta, panTiltZoomRead,
					  objProp, control.initialize);
#endif

	if (migrateDebug) {
		float speed_pan  = (float)control.meta[11];
		float speed_tilt = (float)control.meta[12];
		if(speed_pan>64)
			speed_pan = -(speed_pan-64);
		if(speed_tilt>64)
			speed_tilt = -(speed_tilt-64);
		if((int)control.meta[31]==0)//track score
		{
			speed_pan  = 0;
			speed_tilt = 0;
			//TODO: stop algorithm
		}
		mDebug("######################### cevo speed pan=%d tilt=%d", speed_pan, speed_tilt);
		mDebug("######################### ptzp head pan=%d tilt=%d", panTiltZoomRead[3], panTiltZoomRead[4]);
	}
	qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~Pan&Tilt degree values are " << panTiltZoomRead[3] <<  panTiltZoomRead[4];
	if (control.initialize == 0)
		headpt->panTiltDegree(panTiltZoomRead[3], panTiltZoomRead[4]);

	if (control.initialize)
		control.initialize = 0;

	return 0;
}

int TrackAlgorithmElement::reloadJson(const QJsonObject &node)
{
	QJsonObject roi = node["roi"].toObject();
	objProp[0] = roi["x"].toDouble();
	objProp[1] = roi["y"].toDouble();
	objProp[2] = roi["width"].toDouble();
	objProp[3] = roi["height"].toDouble();
	control.trackScore = node["score"].toDouble();
	control.trackInterval = node["interval"].toInt();
	control.sensitivity = node["sensitivity"].toInt();
	if (node["mode"].toString() == "auto")
		mode = AUTO;
	else if (node["mode"].toString() == "manual")
		mode = MANUAL;
	else
		mode = AUTO;
	return 0;
}

QJsonObject TrackAlgorithmElement::resaveJson(const QJsonObject &node)
{
	QJsonObject tr = node;
	QJsonObject roi;
	roi["x"] = control.obj.point_x;
	roi["y"] = control.obj.point_y;
	roi["width"] = control.obj.width;
	roi["height"] = control.obj.height;
	tr["roi"] = roi;
	tr["score"] = control.trackScore;
	tr["interval"] = control.trackInterval;
	tr["sensitivity"] = control.sensitivity;
	if (mode == AUTO)
		tr["mode"] = QString("auto");
	else if (mode == MANUAL)
		tr["mode"] = QString("manual");
	return tr;
}

int TrackAlgorithmElement::forwardToObjPropFromControl(const TrackAlgorithmElement::TrackControl &control)
{
	objProp[0] = control.obj.point_x;
	objProp[1] = control.obj.point_y;
	objProp[2] = control.obj.width;
	objProp[3] = control.obj.height;
	return 0;
}

int TrackAlgorithmElement::setTrackObjInfo(float x, float y, float w, float h)
{
	control.obj.point_x = x;
	control.obj.point_y = y;
	control.obj.width = w;
	control.obj.height = h;
	forwardToObjPropFromControl(control);
	return 0;
}

