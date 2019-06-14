#include "algorithmmanager.h"
#include "algorithmfunctions.h"
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/ptzpdriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/ptzp/kayidriver.h>

#include "QFile"
#include "ecl/debug.h"
#include "algorithmelement.h"

#include <QThread>
#include <QTimer>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using namespace std;

class GrpcThreadAlg : public QThread
{
public:
	GrpcThreadAlg (quint16 port, AlgorithmManager *s)
	{
		servicePort = port;
		streamer = s;
	}
	void run()
	{
		string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		qDebug() << "--------------------------------------------------" << QString::fromStdString(ep);
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(streamer);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	AlgorithmManager *streamer;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief AlgorithmManager::AlgorithmManager
/// \param parent
/// This class is responsible for the management of algorithms.
/// In this class, chosen algorithm obtains buffer from the method of processBlocking
/// that is a member function of BufferLmmElement class. Currently, available algorithms are
/// motion detection, object tracking, panaroma, stabilization.
/// From algorithm.json file, system and algorithm related parameters are checked. Please go through it in
/// case you are wondering general specifications.
/// ////////////////////////////////////////////////////////////////////////////////////////////////7/////
/// Other algorithms like classification, face detection, many of others will be implemented.
/// InitizalieAlg, reinitializeAlg, idleTo, holdTo functions will be implemented  in accordance with
/// the need/state/requirements of algorithms for on the fly functionality of this class.

//grpc::Status AlgorithmManager::RunAlgorithm()
//{
//	if(AlgorithmCommunication::RequestForAlgorithm::MOTION == algorithmType)
//		return RunMotion();

//	if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION == algorithmType)
//		return RunStab();

//	return grpc::Status(grpc::StatusCode::NOT_FOUND, "no such algoritm");
//}

grpc::Status AlgorithmManager::RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context);
	AlgorithmCommunication::RequestForAlgorithm::DeviceType deviceType;
	AlgorithmCommunication::RequestForAlgorithm::Algorithm algorithmType;
	deviceType = request->device();
	algorithmType = request->algorithmtype();
	AlgorithmElement::AlgorithmElementHandler algHandlerEl;
	AlgorithmElement::Algorithm alg;
	GRPCSetReturn gsr;
	if (!availableAlgortihms.contains((Algorithm)algorithmType)) {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
		response->set_err(31);
		return grpc::Status::CANCELLED;
	}
	if(deviceType == AlgorithmCommunication::RequestForAlgorithm::TV) {
		algHandlerEl.confUnit.param.stabilization = 1;
	} else if (deviceType == AlgorithmCommunication::RequestForAlgorithm::THERMAL)
	{
		algHandlerEl.confUnit.param.stabilization = 2;
	}

	AlgorithmElement *el = nullptr;
	if(AlgorithmCommunication::RequestForAlgorithm::MOTION == algorithmType)
	{
		AlgorithmCommunication::MotionParameters::Settings settings;
		settings = request->motionparam().settingchoice();
		int sensitivity = request->motionparam().sensitivity();
		::AlgorithmCommunication::TRoi TRoi= request->motionparam().roilist();
		gsr = handleMotionDetection(algoElements[MOTION].first(),settings,response,TRoi, sensitivity);
		alg = AlgorithmElement::Algorithm::MOTION;
		el = gsr.el;
		if (gsr.status)
			return grpc::Status::CANCELLED;
	}
	else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION == algorithmType)
	{
		AlgorithmCommunication::StabilizationParameters stabilParams = request->stabilizationparam();
		int sensitivity = request->stabilizationparam().sensitivity();
		gsr = handleStabilization(algoElements[STABILIZATION].first(),stabilParams,response,sensitivity);
		alg = AlgorithmElement::Algorithm::STABILIZATION;
		if (gsr.status)
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING == algorithmType)
	{
		AlgorithmCommunication::TrackParameters::TrackType trackType_;
		AlgorithmCommunication::TrackParameters::Mode mode_;
		trackType_ = request->trackparam().tracktype();
		mode_ = request->trackparam().mode();
		float trackScore = request->trackparam().trackscore();
		int trackInterval = request->trackparam().trackinterval();
		::AlgorithmCommunication::TrackObject obj;
		obj.set_width(request->trackparam().target().width());
		obj.set_height(request->trackparam().target().height());
		obj.set_point_y(request->trackparam().target().point_y());
		obj.set_point_x(request->trackparam().target().point_x());
		gsr = handleTracking(algoElements[TRACKING].first(),trackType_,mode_,response,obj,trackScore, trackInterval);
		alg = AlgorithmElement::Algorithm::TRACKING;
		el = gsr.el;
		if (!gsr.response)
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA == algorithmType)
	{
		AlgorithmCommunication::PanaromaParameters panaromaParams;
		panaromaParams = request->panaromaparam();
		gsr = handlePanaroma(algoElements[PANAROMA].first(),panaromaParams,response);
		alg = AlgorithmElement::Algorithm::PANAROMA;
		el = gsr.el;
		if (gsr.status)
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION == algorithmType)
	{
		AlgorithmCommunication::FaceDetectionParameters faceDetectionParams;
		faceDetectionParams = request->facedetectionparam();
		gsr = handleFaceDetection(algoElements[FACE_DETECTION].first(),faceDetectionParams,response);
		alg = AlgorithmElement::Algorithm::FACE_DETECTION;
		el = gsr.el;
		if (gsr.status)
			return grpc::Status::CANCELLED;
	} else {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
		response->set_err(1);
	}
	el->updateAlgorithmParameters(el->algHandlerEl,alg);
	if (el) {
		el->enableAlg(true);
	}
	return grpc::Status::OK;

}

grpc::Status AlgorithmManager::StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context);
	Q_UNUSED(request);
	AlgorithmCommunication::RequestForAlgorithm::DeviceType deviceType;
	AlgorithmCommunication::RequestForAlgorithm::Algorithm algorithmType;
	deviceType = request->device();
	algorithmType = request->algorithmtype();

	if (!availableAlgortihms.contains((Algorithm)algorithmType)) {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
		response->set_err(31);
		return grpc::Status::CANCELLED;
	}
	AlgorithmElement *el = NULL;
	if (AlgorithmCommunication::RequestForAlgorithm::MOTION == algorithmType)
	{
		el = algoElements[MOTION].first();
		if (el->algHandlerEl.initialize == 0) {
			el->enableAlg(false);
			QThread::sleep(2);
			el->clean();
		}
		//For motion specific parameters, this scope is created
	} else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION == algorithmType)
	{
		//For stabilization specific parameters, this scope is created
		el = algoElements[STABILIZATION].first();
		if (el->algHandlerEl.initialize == 0) {
			el->enableAlg(false);
			QThread::sleep(2);
			el->clean();
		}
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING == algorithmType)
	{
		el = algoElements[TRACKING].first();
		//For tracking specific parameters, this scope is created
		el = algoElements[TRACKING].first();
		if (el->algHandlerEl.initialize == 0) {
			el->enableAlg(false);
			QThread::sleep(2);
			el->clean();
		}
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA == algorithmType)
	{
		//For panaroma specific parameters, this scope is created
		el = algoElements[PANAROMA].first();
		if (el->algHandlerEl.initialize == 0) {
			el->enableAlg(false);
			QThread::sleep(2);
			el->clean();
		}
	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION == algorithmType)
	{
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GRPC FACE STOP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
		el = algoElements[FACE_DETECTION].first();
		el->clean();
		//For face detection specific parameters, this scope is created
		el = algoElements[FACE_DETECTION].first();
		if (el->algHandlerEl.initialize == 0) {
			el->enableAlg(false);
			QThread::sleep(2);
			el->clean();
		}
	} else {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
		response->set_err(11);
	}
	response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
	response->set_err(0);

	el->algHandlerEl.initialize = 1;
	return grpc::Status::OK;
}

grpc::Status AlgorithmManager::SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context)
	AlgorithmCommunication::RequestForAlgorithm::DeviceType deviceType;
	AlgorithmCommunication::RequestForAlgorithm::Algorithm algorithmType;

	deviceType = request->device();
	algorithmType = request->algorithmtype();

	if (AlgorithmCommunication::RequestForAlgorithm::MOTION)
	{
		if (request->motionparam().sensitivity() != NULL && request->motionparam().sensitivity() != 0) {
			algoElements[MOTION].first()->algHandlerEl.motionA.sensitivity = request->motionparam().sensitivity();
			if (request->motionparam().roilist().polygon().size() < 1)
				return grpc::Status::OK;
		}
		else if ((request->motionparam().sensitivity() == NULL || request->motionparam().sensitivity() == 0) && request->motionparam().roilist().polygon().size() < 1)
			return grpc::Status::CANCELLED;
		if (request->motionparam().roilist().polygon().size() > 1)
		{
			QFile f("points.txt");
			if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
				response->set_err(2);
				return grpc::Status::CANCELLED;
			}
			QTextStream out(&f);
			out << request->motionparam().roilist().polygon().size() << "\n";
			for (int i = 0; i < request->motionparam().roilist().polygon().size(); i++) {
				AlgorithmCommunication::TPolygon myPoly = request->motionparam().roilist().polygon(i);
				out << myPoly.points_size() << "\n";
			}
			for (int i = 0; i < request->motionparam().roilist().polygon().size(); i++) {
				AlgorithmCommunication::TPolygon myPoly = request->motionparam().roilist().polygon(i);
				qDebug() << "Is polygon active? " << myPoly.is_active();
				int state = (myPoly.is_active() == true) ? 1 : 0;
				out << state << "\n";
			}

			for (int i = 0; i < request->motionparam().roilist().polygon().size(); i++) {
				AlgorithmCommunication::TPolygon myPoly = request->motionparam().roilist().polygon(i);
				for (int k = 0; k < myPoly.points_size(); k++) {
					qDebug() << myPoly.points(k).x();
					qDebug() << myPoly.points(k).y();
					out << myPoly.points(k).x() << "\n";
					out << myPoly.points(k).y() << "\n";
				}
			}

			// rect1
			out << request->motionparam().roilist().rect1().upperleft().x() << "\n";
			out << request->motionparam().roilist().rect1().upperleft().y() << "\n";
			out << request->motionparam().roilist().rect1().bottomright().x() << "\n";
			out << request->motionparam().roilist().rect1().bottomright().y() << "\n";

			// rect2
			out << request->motionparam().roilist().rect2().upperleft().x() << "\n";
			out << request->motionparam().roilist().rect2().upperleft().y() << "\n";
			out << request->motionparam().roilist().rect2().bottomright().x() << "\n";
			out << request->motionparam().roilist().rect2().bottomright().y() << "\n";
			f.close();
			return grpc::Status::OK;
		}
		else
			return grpc::Status::CANCELLED;

	} else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION)
	{
		if (request->stabilizationparam().sensitivity() != NULL && request->stabilizationparam().sensitivity() == 0)
			algoElements[STABILIZATION].first()->algHandlerEl.stabilA.sensitivity = request->stabilizationparam().sensitivity();
		else
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING)
	{
		if (request->trackparam().trackinterval() != NULL || request->trackparam().trackinterval() != 0)
			algoElements[TRACKING].first()->algHandlerEl.trackA.trackDuration = request->trackparam().trackinterval();
		else
			return grpc::Status::CANCELLED;
		if (request->trackparam().trackscore() != NULL || request->trackparam().trackscore() != 0)
			algoElements[TRACKING].first()->algHandlerEl.trackA.trackScore = request->trackparam().trackscore();
		else
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA)
	{

	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION)
	{

	}
	return grpc::Status::OK;
}

grpc::Status AlgorithmManager::GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response)
{
	Q_UNUSED(context)
	AlgorithmCommunication::RequestForAlgorithm::DeviceType deviceType;
	AlgorithmCommunication::RequestForAlgorithm::Algorithm algorithmType;

	deviceType = request->device();
	algorithmType = request->algorithmtype();

	if (AlgorithmCommunication::RequestForAlgorithm::MOTION)
	{
		AlgorithmCommunication::MotionParameters *motionParam;
		motionParam->set_sensitivity(algoElements[MOTION].first()->algHandlerEl.motionA.sensitivity);
		QFile f("points.txt");
		if (!f.open(QIODevice::ReadOnly))
			return grpc::Status::CANCELLED;
		QTextStream in(&f);
		QStringList lines;
		while (!in.atEnd()) {
			lines << in.readLine();
		}
		int numOfPolySize = lines.at(0).toInt();
		QList<int> pointSize;
		for (int i = 0; i < numOfPolySize; i++) {
			pointSize << lines.at(i + 1).toInt();
		}

		QList<int> polyState;
		for (int i = 0; i < numOfPolySize; i++) {
			polyState << lines.at(i + 1 + numOfPolySize).toInt();
		}

		int polyCounter = 0;
		AlgorithmCommunication::TRoi *myRoi;
		for (int j = 0; j < numOfPolySize; j++) {
			// = motionParam->roilist();
			AlgorithmCommunication::TPolygon *myPoly1 = myRoi->add_polygon();
			bool state = (polyState[j] == 1) ? true : false;
			myPoly1->set_is_active(state);
			for (int i = 0; i < pointSize[j] * 2; i = i + 2) {
				AlgorithmCommunication::TPoint *myPoints = myPoly1->add_points();
				myPoints->set_x(lines.at(polyCounter + i + 1  + 2 * numOfPolySize).toFloat());
				myPoints->set_y(lines.at(polyCounter + i + 2  + 2 * numOfPolySize).toFloat());
			}
			polyCounter = polyCounter + 2 * pointSize[j];
		}

		AlgorithmCommunication::TRectangle *myRect1 =  myRoi->mutable_rect1();
		AlgorithmCommunication::TRectangle *myRect2 =  myRoi->mutable_rect2();
		myRect1->mutable_upperleft()->set_x(lines.at(lines.size() - 8).toFloat());
		myRect1->mutable_upperleft()->set_y(lines.at(lines.size() - 7).toFloat());
		myRect1->mutable_bottomright()->set_x(lines.at(lines.size() - 6).toFloat());
		myRect1->mutable_bottomright()->set_y(lines.at(lines.size() - 5).toFloat());

		myRect2->mutable_upperleft()->set_x(lines.at(lines.size() - 4).toFloat());
		myRect2->mutable_upperleft()->set_y(lines.at(lines.size() - 3).toFloat());
		myRect2->mutable_bottomright()->set_x(lines.at(lines.size() - 2).toFloat());
		myRect2->mutable_bottomright()->set_y(lines.at(lines.size() - 1).toFloat());
		motionParam->set_allocated_roilist(myRoi);
		response->set_allocated_motionparam(motionParam);

		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	} else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION)
	{
		AlgorithmCommunication::StabilizationParameters *stabilParam;
		stabilParam->set_sensitivity(algoElements[STABILIZATION].first()->algHandlerEl.stabilA.sensitivity);
		response->set_allocated_stabilizationparam(stabilParam);
		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING)
	{
		AlgorithmCommunication::TrackParameters *trackParam;
		trackParam->set_trackinterval(algHandlerEl.trackA.trackDuration);
		trackParam->set_trackscore(algHandlerEl.trackA.trackScore);
		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA)
	{
		AlgorithmCommunication::PanaromaParameters *panaromaParam;
		panaromaParam->set_dummy(algoElements[PANAROMA].first()->algHandlerEl.panaromA.dummy);
		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION)
	{
		AlgorithmCommunication::FaceDetectionParameters *faceDetectionParam;
		faceDetectionParam->set_dummy(algoElements[FACE_DETECTION].first()->algHandlerEl.faceA.dummy);
		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	}
	return grpc::Status::OK;

}

grpc::Status AlgorithmManager::SetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response)
{
	Q_UNUSED(context)
	if (AlgorithmCommunication::RequestForSystemFeatures::GPIO)
	{
		int gpioNumber = request->whichgpio();

		return grpc::Status::OK;
	} else if (AlgorithmCommunication::RequestForSystemFeatures::BITRATE)
	{
		int bitrate = request->valueforfeature();

		return grpc::Status::OK;
	} else {
		return grpc::Status::CANCELLED;
	}
}

grpc::Status AlgorithmManager::GetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response)
{
	Q_UNUSED(context)
	if (AlgorithmCommunication::RequestForSystemFeatures::UPTIME)
	{
		int upTime;
		response->set_valueforfeature(upTime);
		return grpc::Status::OK;
	} else if (AlgorithmCommunication::RequestForSystemFeatures::LIFETIME)
	{
		int lifeTime;
		response->set_valueforfeature(lifeTime);
		return grpc::Status::OK;
	} else if (AlgorithmCommunication::RequestForSystemFeatures::FOV_VALUE)
	{
		float fovValue;
		response->set_valueforfeaturefloat(fovValue);
		return grpc::Status::OK;
	}else if (AlgorithmCommunication::RequestForSystemFeatures::GPIO)
	{
		int valueOfGPIO;
		response->set_valueforfeature(valueOfGPIO);
		return grpc::Status::OK;
	} else if (AlgorithmCommunication::RequestForSystemFeatures::BITRATE)
	{
		int bitrate;
		response->set_valueforfeature(bitrate);
		return grpc::Status::OK;
	} else {
		return grpc::Status::CANCELLED;
	}
}

grpc::Status AlgorithmManager::SendInformation(grpc::ServerContext *context, const AlgorithmCommunication::WhichInfo *request, ::grpc::ServerWriter<AlgorithmCommunication::GeneralInfo> *writer)
{
	Q_UNUSED(context);
	Q_UNUSED(request);
	::AlgorithmCommunication::GeneralInfo *generalInfo = new AlgorithmCommunication::GeneralInfo();
	::AlgorithmCommunication::AlarmInfo *alarmInfo_ = new AlgorithmCommunication::AlarmInfo();

	while (1) {
		alarmQueueLock.lock();
		if (alarmQueue.size()) {
			const AlgorithmCommunication::AlarmInfo &info
					= alarmQueue.takeFirst();
			*alarmInfo_ = info;
			alarmQueueLock.unlock();

			/* fill alarm info */
			alarmInfo_->set_alarmflag(alarmInfo.alarmFlag);
			QString format = "yyyyMMddHHmmss.zzz";
			QDateTime time;
			time = QDateTime::currentDateTimeUtc();
			QString dateFormatted = QString("%1").arg(time.toString(format));
			alarmInfo_->set_date(dateFormatted.toStdString());
			generalInfo->set_allocated_alarminfo(alarmInfo_);

			if (!writer->Write(*generalInfo))
				break;
		} else {
			alarmQueueLock.unlock();
			QThread::sleep(1);
		}
	}

	return grpc::Status::OK;
}

AlgorithmManager::GRPCSetReturn AlgorithmManager::handleMotionDetection(AlgorithmElement * el_, const AlgorithmCommunication::MotionParameters::Settings settings,AlgorithmCommunication::ResponseOfRequests *response,const ::AlgorithmCommunication::TRoi TRoi, int sensitivity)
{
	GRPCSetReturn gsr;
	gsr.el = nullptr;
	gsr.el = el_;
	gsr.response = nullptr;
	gsr.response = response;
	if (settings == AlgorithmCommunication::MotionParameters::SENSITIVITY) {
		if (sensitivity == NULL) {
			if (gsr.el->algHandlerEl.motionA.sensitivity == 0 || gsr.el->algHandlerEl.motionA.sensitivity == NULL) {
				gsr.response->set_err(5);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				gsr.status = grpc::StatusCode::CANCELLED;
				return gsr;
			}
		} else if (sensitivity == 0)
		{
			if (gsr.el->algHandlerEl.motionA.sensitivity == 0 || gsr.el->algHandlerEl.motionA.sensitivity == NULL) {
				gsr.response->set_err(5);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				gsr.status = grpc::StatusCode::CANCELLED;
				return gsr;
			}
		} else if (sensitivity != 0 && sensitivity != NULL) {
			gsr.el->algHandlerEl.motionA.sensitivity = sensitivity;
			gsr.response->set_err(0);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
			gsr.status = grpc::StatusCode::OK;
		}
	} if (settings == AlgorithmCommunication::MotionParameters::ROI) {
		if (TRoi.polygon().size() < 1) {
			if (!QFile("points.txt").exists()) {
				gsr.response->set_err(1);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
				gsr.status =  grpc::StatusCode::CANCELLED;
				return gsr;
			} else {
				gsr.response->set_err(0);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				gsr.status =  grpc::StatusCode::CANCELLED;
				return gsr;
			}
		}
		QFile f("points.txt");
		if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
			gsr.response->set_err(2);
			gsr.status =  grpc::StatusCode::CANCELLED;
			return gsr;
		}
		QTextStream out(&f);
		out << TRoi.polygon().size() << "\n";
		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			out << myPoly.points_size() << "\n";
		}
		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			int state = (myPoly.is_active() == true) ? 1 : 0;
			out << state << "\n";
		}

		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			for (int k = 0; k < myPoly.points_size(); k++) {
				out << myPoly.points(k).x() << "\n";
				out << myPoly.points(k).y() << "\n";
			}
		}

		// rect1
		out << TRoi.rect1().upperleft().x() << "\n";
		out << TRoi.rect1().upperleft().y() << "\n";
		out << TRoi.rect1().bottomright().x() << "\n";
		out << TRoi.rect1().bottomright().y() << "\n";

		// rect2
		out << TRoi.rect2().upperleft().x() << "\n";
		out << TRoi.rect2().upperleft().y() << "\n";
		out << TRoi.rect2().bottomright().x() << "\n";
		out << TRoi.rect2().bottomright().y() << "\n";
		f.close();
		gsr.response->set_err(0);
		gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		gsr.status = grpc::StatusCode::OK;
	}
	if (settings == AlgorithmCommunication::MotionParameters::BOTH) {
		if (sensitivity == NULL) {
			if (gsr.el->algHandlerEl.motionA.sensitivity == 0 || gsr.el->algHandlerEl.motionA.sensitivity == NULL) {
				gsr.response->set_err(5);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				gsr.status = grpc::StatusCode::CANCELLED;
			}
		} else if (sensitivity == 0)
		{
			if (gsr.el->algHandlerEl.motionA.sensitivity == 0 || gsr.el->algHandlerEl.motionA.sensitivity == NULL) {
				gsr.response->set_err(5);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				gsr.status =  grpc::StatusCode::CANCELLED;
			}
		}
		else if (sensitivity != 0 && sensitivity != NULL) {
			gsr.el->algHandlerEl.motionA.sensitivity = sensitivity;
			gsr.response->set_err(0);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		}
		if (TRoi.polygon().size() < 1) {
			if (!QFile("points.txt").exists()) {
				gsr.response->set_err(1);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
				gsr.status =  grpc::StatusCode::CANCELLED;
			} else {
				gsr.response->set_err(0);
				gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				gsr.status =  grpc::StatusCode::CANCELLED;
			}
		}
		QFile f("points.txt");
		if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
			gsr.response->set_err(2);
			gsr.status =  grpc::StatusCode::CANCELLED;
		}
		QTextStream out(&f);
		out << TRoi.polygon().size() << "\n";
		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			out << myPoly.points_size() << "\n";
		}
		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			int state = (myPoly.is_active() == true) ? 1 : 0;
			out << state << "\n";
		}

		for (int i = 0; i < TRoi.polygon().size(); i++) {
			AlgorithmCommunication::TPolygon myPoly = TRoi.polygon(i);
			for (int k = 0; k < myPoly.points_size(); k++) {
				out << myPoly.points(k).x() << "\n";
				out << myPoly.points(k).y() << "\n";
			}
		}

		// rect1
		out << TRoi.rect1().upperleft().x() << "\n";
		out << TRoi.rect1().upperleft().y() << "\n";
		out << TRoi.rect1().bottomright().x() << "\n";
		out << TRoi.rect1().bottomright().y() << "\n";

		// rect2
		out << TRoi.rect2().upperleft().x() << "\n";
		out << TRoi.rect2().upperleft().y() << "\n";
		out << TRoi.rect2().bottomright().x() << "\n";
		out << TRoi.rect2().bottomright().y() << "\n";
		f.close();
		gsr.response->set_err(0);
		gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		gsr.status = grpc::StatusCode::OK;
	}
	return gsr;
}

AlgorithmManager::GRPCSetReturn AlgorithmManager::handleTracking(AlgorithmElement * el_, const AlgorithmCommunication::TrackParameters::TrackType trackType_, const AlgorithmCommunication::TrackParameters::Mode, AlgorithmCommunication::ResponseOfRequests *response, const AlgorithmCommunication::TrackObject obj, float trackScore, int trackInterval)
{
	GRPCSetReturn gsr;
	gsr.el = nullptr;
	gsr.el = el_;
	gsr.response = nullptr;
	gsr.response = response;
	AlgorithmCommunication::TrackParameters::TrackType trackType;
	trackType = trackType_;

	if (trackType == AlgorithmCommunication::TrackParameters::AUTO)
	{
		if(!gsr.el->algHandlerEl.trackA.enabledModes.contains(gsr.el->algHandlerEl.trackA.enabledModes.value(AlgorithmElement::TrackMode::AUTO)))
		{
			gsr.response->set_err(8);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
			gsr.status = grpc::StatusCode::CANCELLED;
			return gsr;
		} else
		{
			gsr.el->algHandlerEl.trackA.trackMode = AlgorithmElement::TrackMode::AUTO;
		}
	} else if (trackType == AlgorithmCommunication::TrackParameters::SEMI_AUTO)
	{
		if(!gsr.el->algHandlerEl.trackA.enabledModes.contains(gsr.el->algHandlerEl.trackA.enabledModes.value(AlgorithmElement::TrackMode::SEMI_AUTO)))
		{
			response->set_err(8);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
			gsr.status = grpc::StatusCode::CANCELLED;
			return gsr;
		} else
		{
			gsr.el->algHandlerEl.trackA.trackMode = AlgorithmElement::TrackMode::SEMI_AUTO;
		}
	} else if (trackType == AlgorithmCommunication::TrackParameters::MANUAL)
	{
		if(!gsr.el->algHandlerEl.trackA.enabledModes.contains(gsr.el->algHandlerEl.trackA.enabledModes.value(AlgorithmElement::TrackMode::MANUAL)))
		{
			response->set_err(8);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
			gsr.status = grpc::StatusCode::CANCELLED;
			return gsr;
		} else
		{
			gsr.el->algHandlerEl.trackA.trackMode = AlgorithmElement::TrackMode::MANUAL;
		}
	}
	if (trackInterval != NULL || trackInterval != 0)
	{
		gsr.el->algHandlerEl.trackA.trackDuration = trackInterval;
	} else {
		if (!gsr.el->algHandlerEl.trackA.trackDuration)
		{
			gsr.response->set_err(12);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
			gsr.status = grpc::StatusCode::CANCELLED;
			return gsr;
		}
	}
	if (trackScore != NULL || trackScore != 0)
	{
		gsr.el->algHandlerEl.trackA.trackScore = trackScore;
	} else {
		if (!algHandlerEl.trackA.trackScore)
		{
			gsr.response->set_err(12);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
			gsr.status = grpc::StatusCode::CANCELLED;
			return gsr;
		}
	}
	if (0.02 > obj.width() || 0.02 > obj.height())
	{
		if (!algHandlerEl.trackA.objHeight || !algHandlerEl.trackA.objHeight)
			gsr.status = grpc::StatusCode::OUT_OF_RANGE;
		return gsr;
	}
	if (0.45 < obj.width() || 0.45 < obj.height())
	{
		if (!gsr.el->algHandlerEl.trackA.objHeight || !gsr.el->algHandlerEl.trackA.objHeight)
			gsr.status = grpc::StatusCode::OUT_OF_RANGE;
		return gsr;
	}
	if (obj.width() != 0 && obj.width() != NULL) {
		gsr.el->algHandlerEl.trackA.objWidth = obj.width();
	}
	if (obj.height() != 0 && obj.height() != NULL) {
		gsr.el->algHandlerEl.trackA.objHeight = obj.width();
	}
	gsr.el->algHandlerEl.trackA.objPointX = obj.point_x();
	gsr.el->algHandlerEl.trackA.objPointY = obj.point_y();
	gsr.response->set_err(0);
	gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
	return gsr;
}

AlgorithmManager::GRPCSetReturn AlgorithmManager::handleStabilization(AlgorithmElement * el_, const AlgorithmCommunication::StabilizationParameters, AlgorithmCommunication::ResponseOfRequests *response, int sensitivity)
{
	GRPCSetReturn gsr;
	gsr.el = nullptr;
	gsr.el = el_;
	gsr.response = nullptr;
	gsr.response = response;

	if (sensitivity != 0 || sensitivity != NULL) {
		gsr.el->algHandlerEl.stabilA.sensitivity = sensitivity;
		gsr.response->set_err(0);
		gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		gsr.status = grpc::StatusCode::OK;
	} else
	{
		if (gsr.el->algHandlerEl.stabilA.sensitivity != 0 || gsr.el->algHandlerEl.stabilA.sensitivity != NULL)
		{
			gsr.response->set_err(0);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
			gsr.status = grpc::StatusCode::OK;
		}
		else
		{
			gsr.response->set_err(0);
			gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
			gsr.status = grpc::StatusCode::CANCELLED;
		}
	}
	return gsr;
}

AlgorithmManager::GRPCSetReturn AlgorithmManager::handlePanaroma(AlgorithmElement * el_, const AlgorithmCommunication::PanaromaParameters, AlgorithmCommunication::ResponseOfRequests *response)
{
	GRPCSetReturn gsr;
	gsr.el = nullptr;
	gsr.el = el_;
	gsr.response = nullptr;
	gsr.response = response;
	gsr.response->set_err(0);
	gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
	gsr.status = grpc::StatusCode::OK;
	return gsr;
}

AlgorithmManager::GRPCSetReturn AlgorithmManager::handleFaceDetection(AlgorithmElement * el_, const AlgorithmCommunication::FaceDetectionParameters, AlgorithmCommunication::ResponseOfRequests *response)
{
	GRPCSetReturn gsr;
	gsr.el = nullptr;
	gsr.el = el_;
	gsr.response = nullptr;
	gsr.response = response;
	gsr.response->set_err(0);
	gsr.response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
	gsr.status = grpc::StatusCode::OK;
	return gsr;
}

AlgorithmManager::AlgorithmManager(QObject *parent)
	: AlgorithmCommunication::AlgorithmService::Service()
{
	openAlgRelatedJson();
	algHandlerEl.systemParameters = checkSystemParameters();
	watcher = new QFileSystemWatcher();
	QFile file;
	file.setFileName("algorithm.json");
	if (file.exists()) {
		watcher->addPath("/home/nvidia/botas_v1.0.0/builds/lmm_apps/SmartStreamer/algorithm.json");
		QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [=](){
			this->updateParametersFromJson();
			watcher->addPath("/home/nvidia/botas_v1.0.0/builds/lmm_apps/SmartStreamer/algorithm.json");
		});
	}
	pt = NULL;
	ptzp = NULL;
	setupDeviceController(systemHandler);
	timer = new QTimer();
	QObject::connect(timer,&QTimer::timeout,[=](){
		this->updatePTInfo();
	});
	timer->start(100);
}

void AlgorithmManager::updatePTInfo()
{
	if(availableAlgortihms.value(TRACKING)){
		ptzInfo.action = algoElements[TRACKING].first()->ptzInfo.action;
		if (ptzInfo.action != PAN_STOP) {
			ptzInfo.pan = algoElements[TRACKING].first()->ptzInfo.pan;
			ptzInfo.tilt = algoElements[TRACKING].first()->ptzInfo.tilt;
			ptzInfo.zoom = algoElements[TRACKING].first()->ptzInfo.zoom;
			setPT(ptzInfo);
			setZoom(ptzInfo.zoom);
		} else
		{
			setPT(ptzInfo);
			setZoom(ptzInfo.zoom);
		}
	}
}

AlgorithmManager::~AlgorithmManager()
{
	watcher->deleteLater();
	timer->deleteLater();

}

int AlgorithmManager::openAlgRelatedJson()
{
	int deviceType = 0;
	QString info;
	QFile file;
	file.setFileName("algorithm.json");
	if (file.exists()) {
		file.open(QIODevice::ReadOnly | QIODevice::Text);
	} else {
		//		mDebug("Algorithm.json file is not found");
		return -1;
	}
	info = file.readAll();
	file.close();
	QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
	QJsonObject obj_info = doc.object();
	//Device related properties like fps,resolution are obtained with the following lines
	QJsonValue source = obj_info.value(QString("source"));
	QJsonObject cam_ip = source.toObject()["IP"].toObject();

	algHandlerEl.confUnit.devProp.cameraIp = cam_ip["address"].toString();
	QJsonValue sec_device = obj_info.value(QString("device"));
	QJsonObject itemized_sec_device = sec_device.toObject();
	QJsonObject dev_resol = itemized_sec_device["resolution"].toObject();
	algHandlerEl.confUnit.devProp.width = dev_resol["width"].toInt();
	algHandlerEl.confUnit.devProp.height = dev_resol["height"].toInt();
	if (algHandlerEl.confUnit.devProp.height == 1080) {
		deviceType = 1;
	} else if (algHandlerEl.confUnit.devProp.height == 576) {
		deviceType = 2;
	} else {
		deviceType = 0; //default can be set or not
	}
	algHandlerEl.confUnit.devProp.frameSize = algHandlerEl.confUnit.devProp.width * algHandlerEl.confUnit.devProp.height;
	bufsize = algHandlerEl.confUnit.devProp.width * algHandlerEl.confUnit.devProp.height;
	algHandlerEl.confUnit.devProp.fps = (float)itemized_sec_device["fps"].toDouble();
	//Overall properties that are used in algorithms obtained with the following lines
	QJsonValue sec_param = obj_info.value(QString("parameters"));
	QJsonObject itemized_sec_param = sec_param.toObject();
	algHandlerEl.confUnit.param.rgb = itemized_sec_param["rgb"].toInt();
	algHandlerEl.confUnit.param.ill = itemized_sec_param["ill"].toInt();
	algHandlerEl.confUnit.param.debug = itemized_sec_param["debug"].toInt();
	algHandlerEl.confUnit.param.shadow = itemized_sec_param["shadow"].toInt();
	algHandlerEl.confUnit.param.record = itemized_sec_param["record"].toInt();
	algHandlerEl.confUnit.param.privacy = itemized_sec_param["privacy"].toInt();
	algHandlerEl.confUnit.param.stabilization = itemized_sec_param["stabilization"].toInt();
	//Which algorithm will be used with specific properties in the current project
	QJsonValue sec_alg = obj_info.value(QString("algorithms"));
	QJsonObject itemized_sec_alg = sec_alg.toObject();
	QJsonObject motion = itemized_sec_alg["motion_detection"].toObject();
	QJsonObject stab = itemized_sec_alg["stabilization"].toObject();
	QJsonObject track = itemized_sec_alg["tracking"].toObject();
	QJsonObject panaroma = itemized_sec_alg["panaroma"].toObject();
	QJsonObject face = itemized_sec_alg["face_detection"].toObject();
	int indexForAlgList = 0;
	if (motion["enabled"].toBool() == true) {
		indexForAlgList++;
		algHandlerEl.motionA.sensitivity = motion["sensitivity"].toInt();
		algHandlerEl.motionA.classification = motion["classification"].toInt();
		algHandlerEl.motionA.classification_ = motion["classification"].toBool();
		availableAlgortihms.insert(Algorithm::MOTION,true);
	}
	if (stab["enabled"].toBool() == true) {
		indexForAlgList++;
		availableAlgortihms.insert(Algorithm::STABILIZATION,true);
	}
	if (track["enabled"].toBool() == true) {
		indexForAlgList++;
		algHandlerEl.trackA.trackScore = (float) track["track_score"].toDouble();
		algHandlerEl.trackA.trackDuration = track["track_duration"].toInt();
		algHandlerEl.trackA.objHeight = 100;
		algHandlerEl.trackA.objWidth = 100;
		algHandlerEl.trackA.objPointX = 100;
		algHandlerEl.trackA.objPointY = 100;
		QJsonObject type = track["track_types"].toObject();
		int counter = 0;
		if (type["auto"].toBool())
		{
			algHandlerEl.trackA.enabledModes.insert(counter,AlgorithmElement::TrackMode::AUTO);
			counter++;
		}
		if (type["semi_auto"].toBool())
		{
			algHandlerEl.trackA.enabledModes.insert(counter,AlgorithmElement::TrackMode::SEMI_AUTO);
			counter++;
		}
		if (type["manual"].toBool())
		{
			algHandlerEl.trackA.enabledModes.insert(counter,AlgorithmElement::TrackMode::MANUAL);
			counter++;
		}
		if (!track["multiple_track_type_enabled"].toBool())
		{
			//Tracking type is limited to one mode, if there exist more than
			//one mode binary will shut down itself
			if (counter > 1)
			{
				qDebug() << "There exist violation in the json file under tracking section";
				qDebug() << "mult_track_type cannot be disabled if there exist more than one mode for track type";
				return -2;
			}
			if (counter == 0)
			{
				qDebug() << "None of track types was not selected so auto mode is set as default mode";
			}
		} else {
			//Multiple tracking type is selected
		}
		availableAlgortihms.insert(Algorithm::TRACKING,true);
	}
	if (panaroma["enabled"].toBool())
	{
		indexForAlgList++;
		availableAlgortihms.insert(Algorithm::PANAROMA,true);
	}
	if (face["enabled"].toBool())
	{
		algHandlerEl.faceA.isTileOn = face["isTileOn"].toBool();
		algHandlerEl.faceA.xTile = face["xTile"].toInt();
		algHandlerEl.faceA.yTile = face["yTile"].toInt();
		algHandlerEl.faceA.mode = face["mode"].toInt();
		algHandlerEl.faceA.isAlignmentOn = face["isAlignmentOn"].toBool();
		indexForAlgList++;
		availableAlgortihms.insert(Algorithm::FACE_DETECTION,true);
	}

	QJsonObject itemized_alarm_info = obj_info.value(QString("alarmInfo")).toObject();
	if (!itemized_alarm_info.isEmpty()) {
		alarmInfo.baseId = itemized_alarm_info["baseId"].toInt();
		alarmInfo.stationId = itemized_alarm_info["stationId"].toInt();
		alarmInfo.deviceId = itemized_alarm_info["deviceId"].toInt();
		alarmInfo.unitType = itemized_alarm_info["unitType"].toInt();
	}

	QJsonValue project_info = obj_info.value(QString("projects"));
	QJsonObject itemized_project_info = project_info.toObject();
	bool botas_fix = itemized_project_info["botas_fix"].toBool();
	bool botas_dome = itemized_project_info["botas_dome"].toBool();
	bool tbgth = itemized_project_info["tbgth"].toBool();
	bool arya  = itemized_project_info["arya"].toBool();

	if (botas_fix && !botas_dome && !tbgth && !arya)
		systemHandler = BOTAS_FIX;
	else if (botas_dome && !botas_fix && !tbgth && !arya)
		systemHandler = BOTAS_DOME;
	else if (tbgth && !botas_fix && !botas_dome && !arya)
		systemHandler = TBGTH;
	else if (arya && !tbgth && !botas_fix && !botas_dome)
		systemHandler = ARYA;
	else if (itemized_project_info["kayi"].toBool())
		systemHandler = KAYI;
	else
		systemHandler = EMPTY_SYSTEM;
//	qDebug() << confUnit.devProp.width << confUnit.devProp.height << confUnit.param.rgb <<
//				confUnit.param.record <<  confUnit.param.shadow << confUnit.param.ill << confUnit.param.debug;
//	qDebug() << confUnit.param.stabilization << confUnit.param.privacy <<
//				algHandler.meta << pan_tilt_zoom_read << algHandler.motionA.alarmFlag << algHandler.initialize << algHandler.motionA.sensitivity;
}

int AlgorithmManager::updateParametersFromJson()
{
	openAlgRelatedJson();
	AlgorithmElement *el = NULL;
	foreach(Algorithm alg, availableAlgortihms.keys())
	{
		bool prevState = false;
		if (availableAlgortihms.value(alg))
		{
			el = algoElements[alg].first();
			if (el->algHandlerEl.initialize == 0) {
				el->enableAlg(false);
				prevState = true;
			}
			el->updateAlgorithmParameters(algHandlerEl,(AlgorithmElement::Algorithm)alg);
			if (prevState) {
				QThread::sleep(2);
				el->algHandlerEl.initialize = 0;
				el->enableAlg(true);
			}
		}
	}
}

int AlgorithmManager::setupDeviceController(const System systemInfo)
{
	if (ptzp == NULL)
	{
		if (systemInfo == BOTAS_FIX) {
			botas = new IRDomeDriver();
			botas->startGrpcApi(50058);
			botas->setTarget("ttyS0?baud=9600;null");
			ptzp = botas;
			qDebug() << "botas is initialized";
		} else if (systemInfo == BOTAS_DOME) {
					botas = new IRDomeDriver();
			botas->startGrpcApi(50058);
			botas->setTarget("ttyS0?baud=9600;ttyTHS2?baud=9600");
			ptzp = botas;
			qDebug() << "botas is initialized";
		} else if (systemInfo == TBGTH) {
			tbgth = new TbgthDriver(true);
			ptzp = tbgth;
		} else if (systemInfo == ARYA) {
			arya = new AryaDriver();
			ptzp = arya;
		} else if (systemInfo == KAYI) {
			kayi = nullptr;//new KayiDriver;
			kayi->startGrpcApi(50058);
			kayi->setTarget("ttyXRUSB0?baud=9600?protocol=422;ttyXRUSB1?baud=19200?protocol=422?parity=1?databits=7?stopbits=1");
		} else if (systemInfo == EMPTY_SYSTEM) {
			//For test usage, it will be implemented later
		}
	}
	return 0;
}

AlgorithmElement::AlgorithmElementHandler AlgorithmManager::getAlgHandlerFor(int index)
{
	(void)index;
	return algHandlerEl;
}

void AlgorithmManager::registerAlgorithm(AlgorithmManager::Algorithm alg, AlgorithmElement *el)
{
	algoElements[alg] << el;
	el->enableAlg(false);
}

const AlgorithmElement *AlgorithmManager::getAlgorithmElement(AlgorithmManager::Algorithm alg)
{
	return algoElements[alg].first();
}

int AlgorithmManager::addAlarm(const AlgorithmCommunication::AlarmInfo &info)
{
	QMutexLocker ml(&alarmQueueLock);
	alarmQueue << info;
	return 0;
}

bool AlgorithmManager::checkSystemParameters()
{
	if(availableAlgortihms.value(Algorithm::MOTION)) {
		if (!QFile::exists("points.txt") && !QFile::exists("System_parameters.txt")) {
			return false;
		}
	} else if(availableAlgortihms.value(Algorithm::STABILIZATION)) {
		if (!QFile::exists("stabilization_parameters.txt") || !QFile::exists("stabilization_parameters_Thermal.txt")) {
			return false;
		}
	} else if(availableAlgortihms.value(Algorithm::TRACKING)) {
		if (!QFile::exists("track_alg_parameters.txt") && !QFile::exists("track_parameters.txt")) {
			return false;
		}
	} else if(availableAlgortihms.value(Algorithm::PANAROMA)) {
		if (!QFile::exists("pan_params.txt") && !QFile::exists("pan_shift_parameters.txt") && !QFile::exists("panchange_parameters.txt")) {
			return false;
		}
	}
	else if(availableAlgortihms.value(Algorithm::FACE_DETECTION)) {
		if (!QFile::exists("face.txt") && !QFile::exists("face_parameters.txt")) {
			return false;
		}
	}
}

int AlgorithmManager::setPT(PTZinformation info)
{
	float vel_x = info.pan;
	float vel_y = info.tilt;
	PTZactions act = info.action;
	if (!ptzp)
		switch (act) {
		case PAN_STOP:
			ptzp->getHead(1)->panTiltStop();
			break;
		case PAN_RIGHT:
			ptzp->getHead(1)->panRight(vel_x);
			break;
		case PAN_LEFT:
			ptzp->getHead(1)->panLeft(vel_x);
			break;
		case TILT_UP:
			ptzp->getHead(1)->tiltUp(vel_x);
			break;
		case TILT_DOWN:
			ptzp->getHead(1)->tiltDown(vel_x);
			break;
		case TILT_STOP:
			ptzp->getHead(1)->panTiltStop();
			break;
		case PAN_RIGHT_TILT_UP:
			ptzp->getHead(1)->panTiltAbs(vel_x,vel_y);
			break;
		case PAN_RIGHT_TILT_DOWN:
			ptzp->getHead(1)->panTiltAbs(vel_x,vel_y);
			break;
		case PAN_LEFT_TILT_UP:
			ptzp->getHead(1)->panTiltAbs(vel_x,vel_y);
			break;
		case PAN_LEFT_TILT_DOWN:
			ptzp->getHead(1)->panTiltAbs(vel_x,vel_y);
			break;
		case PAN_TILT_POS:
			ptzp->getHead(1)->panTiltAbs(vel_x,vel_y);
			break;
		default:
			break;
		}
}

void AlgorithmManager::getPTZ()
{
	if (ptzp) {
		ptzInfo.pan = ptzp->getHead(1)->getPanAngle();
		ptzInfo.tilt = ptzp->getHead(1)->getTiltAngle();
		ptzInfo.zoom = ptzp->getHead(0)->getZoom();
	} else {
		qDebug() << "System is not defined so that location information is not available";
	}
}

int AlgorithmManager::setZoom(uint pos)
{
	if (!ptzp)
		return -1;
	ptzp->getHead(0)->setZoom(pos);
	return 0;
}

void AlgorithmManager::startGrpc()
{
	GrpcThreadAlg *grpcServ = new GrpcThreadAlg(60059, this);
	grpcServ->start();
}
