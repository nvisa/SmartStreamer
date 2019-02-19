#include "algorithmmanager.h"
#include "algorithmfunctions.h"
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/ptzpdriver.h>
#include <ecl/ptzp/tbgthdriver.h>

#include "QFile"
#include "ecl/debug.h"

#include <QThread>

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
///
///

grpc::Status AlgorithmManager::RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context);
	AlgorithmCommunication::RequestForAlgorithm::DeviceType deviceType;
	AlgorithmCommunication::RequestForAlgorithm::Algorithm algorithmType;

	deviceType = request->device();
	algorithmType = request->algorithmtype();
	if(deviceType == AlgorithmCommunication::RequestForAlgorithm::TV) {
		confUnit.param.stabilization = 1;
	} else if (deviceType == AlgorithmCommunication::RequestForAlgorithm::THERMAL)
	{
		confUnit.param.stabilization = 2;
	}
	BaseLmmElement *el = NULL;
	if(AlgorithmCommunication::RequestForAlgorithm::MOTION == algorithmType)
	{
		el = algoElements[MOTION].first();
		AlgorithmCommunication::MotionParameters::Settings settings;
		settings = request->motionparam().settingchoice();
		qDebug() << "WTF !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << settings << request->motionparam().sensitivity();
		if (settings == AlgorithmCommunication::MotionParameters::SENSITIVITY) {
			if (request->motionparam().sensitivity() == NULL) {
				response->set_err(5);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				return grpc::Status::CANCELLED;
			} else if (request->motionparam().sensitivity() == 0)
			{
				response->set_err(5);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				return grpc::Status::CANCELLED;
			}
			algHandler.motionA.sensitivity = request->motionparam().sensitivity();
		} else if (settings == AlgorithmCommunication::MotionParameters::ROI) {
			if (request->motionparam().roilist().polygon().size() < 1) {
				response->set_err(1);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
				return grpc::Status::CANCELLED;
			}
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
		} else if (settings == AlgorithmCommunication::MotionParameters::BOTH) {
			AlgorithmCommunication::MotionParameters::Settings settings;
			settings = request->motionparam().settingchoice();
			if (settings == AlgorithmCommunication::MotionParameters::SENSITIVITY) {
				if (request->motionparam().sensitivity() == NULL) {
					response->set_err(5);
					response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
					return grpc::Status::CANCELLED;
				} else if (request->motionparam().sensitivity() == 0)
				{
					response->set_err(5);
					response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
					return grpc::Status::CANCELLED;
				}
				algHandler.motionA.sensitivity = request->motionparam().sensitivity();
			} else if (settings == AlgorithmCommunication::MotionParameters::ROI) {
				if (request->motionparam().roilist().polygon().size() < 1) {
					response->set_err(1);
					response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
					return grpc::Status::CANCELLED;
				}
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
			} else {
				response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
				response->set_err(2);
				return grpc::Status::CANCELLED;
			}
		}
		response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		response->set_err(0);
		algHandler.currentActiveAlg = MOTION;
		algHandler.stateOfAlg = NOT_INIT;
	} else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION == algorithmType)
	{
		if (request->stabilizationparam().sensitivity() == NULL) {
			response->set_err(5);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
			return grpc::Status::CANCELLED;
		} else if (request->stabilizationparam().sensitivity() == 0)
		{
			response->set_err(5);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
			return grpc::Status::CANCELLED;
		}
		algHandler.stabilA.sensitivity = request->stabilizationparam().sensitivity();
		response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
		response->set_err(0);
		algHandler.currentActiveAlg = STABILIZATION;
		algHandler.stateOfAlg = NOT_INIT;
		return grpc::Status::OK;
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING == algorithmType)
	{
		AlgorithmCommunication::TrackParameters::TrackType trackType;
		trackType = request->trackparam().tracktype();
		if (trackType == AlgorithmCommunication::TrackParameters::AUTO)
		{
			if(!algHandler.trackA.enabledModes.key(TrackMode::AUTO))
			{
				response->set_err(8);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				return grpc::Status::CANCELLED;
			}
		} else if (trackType == AlgorithmCommunication::TrackParameters::SEMI_AUTO)
		{
			if(!algHandler.trackA.enabledModes.key(TrackMode::SEMI_AUTO))
			{
				response->set_err(8);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				return grpc::Status::CANCELLED;
			}
		} else if (trackType == AlgorithmCommunication::TrackParameters::MANUAL)
		{
			if(!algHandler.trackA.enabledModes.key(TrackMode::MANUAL))
			{
				response->set_err(8);
				response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
				return grpc::Status::CANCELLED;
			}
		}
		if (request->trackparam().trackinterval() != NULL || request->trackparam().trackinterval() != 0)
		{
			algHandler.trackA.trackDuration = request->trackparam().trackinterval();
		} else {
			response->set_err(12);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
			return grpc::Status::CANCELLED;
		}
		if (request->trackparam().trackscore() != NULL || request->trackparam().trackscore() != 0)
		{
			algHandler.trackA.trackScore = request->trackparam().trackscore();
		} else {
			response->set_err(12);
			response->set_response(AlgorithmCommunication::ResponseOfRequests::MISSING_PARAMETER);
			return grpc::Status::CANCELLED;
		}
		if (0.02 > request->trackparam().target().width() || 0.02 > request->trackparam().target().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too small", "region too small");
		if (0.45 < request->trackparam().target().width() || 0.45 < request->trackparam().target().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too big", "region too big");
		algHandler.trackA.objWidth = request->trackparam().target().width();
		algHandler.trackA.objHeight = request->trackparam().target().height();
		algHandler.trackA.objPointX = request->trackparam().target().point_x();
		algHandler.trackA.objPointY = request->trackparam().target().point_y();
		algHandler.currentActiveAlg = TRACKING;
		algHandler.stateOfAlg = NOT_INIT;
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA == algorithmType)
	{
		algHandler.currentActiveAlg = PANAROMA;
		algHandler.stateOfAlg = NOT_INIT;
	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION == algorithmType)
	{

	} else {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::FAIL);
		response->set_err(1);
	}

	qDebug() << "-------------------------------------" << el;
	if (el)
		el->setPassThru(false);

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
	BaseLmmElement *el = NULL;
	if (AlgorithmCommunication::RequestForAlgorithm::MOTION == algorithmType)
	{
		el = algoElements[MOTION].first();
		//For motion specific parameters, this scope is created
	} else if (AlgorithmCommunication::RequestForAlgorithm::STABILIZATION == algorithmType)
	{
		//For stabilization specific parameters, this scope is created
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING == algorithmType)
	{
		//For tracking specific parameters, this scope is created
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA == algorithmType)
	{
		//For panaroma specific parameters, this scope is created
	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION == algorithmType)
	{
		//For face detection specific parameters, this scope is created
	} else {
		response->set_response(AlgorithmCommunication::ResponseOfRequests::NOT_APPLICABLE);
		response->set_err(11);
	}
	response->set_response(AlgorithmCommunication::ResponseOfRequests::SUCCESSFUL);
	response->set_err(0);
	algHandler.stateOfAlg = TERMINATED;
	if (el)
		el->setPassThru(true);
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
			algHandler.motionA.sensitivity = request->motionparam().sensitivity();
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
			algHandler.stabilA.sensitivity = request->stabilizationparam().sensitivity();
		else
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING)
	{
		if (request->trackparam().trackinterval() != NULL || request->trackparam().trackinterval() != 0)
			algHandler.trackA.trackDuration = request->trackparam().trackinterval();
		else
			return grpc::Status::CANCELLED;
		if (request->trackparam().trackscore() != NULL || request->trackparam().trackscore() != 0)
			algHandler.trackA.trackScore = request->trackparam().trackscore();
		else
			return grpc::Status::CANCELLED;
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA)
	{

	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION)
	{

	}
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
		motionParam->set_sensitivity(algHandler.motionA.sensitivity);

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
		response->set_allocated_stabilizationparam(stabilParam);
		response->set_response(AlgorithmCommunication::ResponseOfParameterRequests::SUCCESSFUL);
		response->set_error(0);
	} else if (AlgorithmCommunication::RequestForAlgorithm::TRACKING)
	{
		AlgorithmCommunication::TrackParameters *trackParam;
		trackParam->set_trackinterval(algHandler.trackA.trackDuration);
		trackParam->set_trackscore(algHandler.trackA.trackScore);
	} else if (AlgorithmCommunication::RequestForAlgorithm::PANAROMA)
	{

	} else if (AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION)
	{

	}

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

AlgorithmManager::AlgorithmManager(QObject *parent)
	: AlgorithmCommunication::AlgorithmService::Service()
{
	algHandler.currentActiveAlg = MOTION;
	algHandler.stateOfAlg = NOT_INIT;
	algHandler.systemHandler = EMPTY_SYSTEM;
	pt = NULL;
	openAlgRelatedJson();
	algHandler.systemParameters = checkSystemParameters();
}

AlgorithmManager::~AlgorithmManager()
{

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
	QJsonValue sec_device = obj_info.value(QString("device"));
	QJsonObject itemized_sec_device = sec_device.toObject();
	QJsonObject dev_resol = itemized_sec_device["resolution"].toObject();
	confUnit.devProp.width = dev_resol["width"].toInt();
	confUnit.devProp.height = dev_resol["height"].toInt();
	if (confUnit.devProp.height == 1080) {
		deviceType = 1;
	} else if (confUnit.devProp.height == 576) {
		deviceType = 2;
	} else {
		deviceType = 0; //default can be set or not
	}
	confUnit.devProp.frameSize = confUnit.devProp.width * confUnit.devProp.height;
	bufsize = confUnit.devProp.width * confUnit.devProp.height;
	confUnit.devProp.fps = itemized_sec_device["fps"].toInt();


	//Overall properties that are used in algorithms obtained with the following lines
	QJsonValue sec_param = obj_info.value(QString("parameters"));
	QJsonObject itemized_sec_param = sec_param.toObject();
	confUnit.param.rgb = itemized_sec_param["rgb"].toInt();
	confUnit.param.ill = itemized_sec_param["ill"].toInt();
	confUnit.param.debug = itemized_sec_param["debug"].toInt();
	confUnit.param.shadow = itemized_sec_param["shadow"].toInt();
	confUnit.param.record = itemized_sec_param["record"].toInt();
	confUnit.param.privacy = itemized_sec_param["privacy"].toInt();
	confUnit.param.stabilization = itemized_sec_param["stabilization"].toInt();

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
		availableAlgList.insert(indexForAlgList,Algorithm::MOTION);
		indexForAlgList++;
		algHandler.motionA.sensitivity = motion["sensitivity"].toInt();
		algHandler.motionA.classification = motion["classification"].toBool();
	}
	if (stab["enabled"].toBool() == true) {
		availableAlgList.insert(indexForAlgList,Algorithm::STABILIZATION);
		indexForAlgList++;
	}
	if (track["enabled"].toBool() == true) {
		availableAlgList.insert(indexForAlgList,Algorithm::TRACKING);
		indexForAlgList++;
		algHandler.trackA.trackScore = (float) track["track_score"].toDouble();
		algHandler.trackA.trackDuration = track["track_duration"].toInt();
		QJsonObject type = track["track_types"].toObject();
		int counter = 0;
		if (type["auto"].toBool())
		{
			algHandler.trackA.enabledModes.insert(counter,TrackMode::AUTO);
			counter++;
		}
		if (type["semi_auto"].toBool())
		{
			algHandler.trackA.enabledModes.insert(counter,TrackMode::SEMI_AUTO);
			counter++;
		}
		if (type["manual"].toBool())
		{
			algHandler.trackA.enabledModes.insert(counter,TrackMode::MANUAL);
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
	}
	if (panaroma["enabled"].toBool())
	{
		availableAlgList.insert(indexForAlgList,Algorithm::PANAROMA);
		indexForAlgList++;
	}
	if (face["enabled"].toBool())
	{
		availableAlgList.insert(indexForAlgList,Algorithm::FACE_DETECTION);
		indexForAlgList++;
	}
	algHandler.confUnit = confUnit;
	qDebug() << confUnit.devProp.width << confUnit.devProp.height << confUnit.param.rgb <<
			confUnit.param.record <<  confUnit.param.shadow << confUnit.param.ill << confUnit.param.debug;
	qDebug() << confUnit.param.stabilization << confUnit.param.privacy <<
			algHandler.meta << pan_tilt_zoom_read << algHandler.motionA.alarmFlag << algHandler.initialize << algHandler.motionA.sensitivity;
}

int AlgorithmManager::setupDeviceController(const QString &deviceInfo)
{
	if (deviceInfo.contains("botas")) {
		botas = new IRDomeDriver();
		algHandler.systemHandler = BOTAS;
		ptzp = botas;
	}
	else if (deviceInfo.contains("tbgth"))
	{
		tbgth = new TbgthDriver(true);
		algHandler.systemHandler = TBGTH;
		ptzp = tbgth;
	}
	else if (deviceInfo.contains("arya"))
	{
		arya = new AryaDriver();
		algHandler.systemHandler = ARYA;
		ptzp = arya;
	}
	else if (deviceInfo.contains("test"))
	{
		//For test usage, it will be implemented later
	}
}

AlgorithmManager::AlgorithmHandler AlgorithmManager::getAlgHandlerFor(int index)
{
	(void)index;
	return algHandler;
}

void AlgorithmManager::registerAlgorithm(AlgorithmManager::Algorithm alg, BaseLmmElement *el)
{
	algoElements[alg] << el;
}

bool AlgorithmManager::checkSystemParameters()
{
	switch(algHandler.currentActiveAlg) {
	case MOTION:
		if (!QFile::exists("points.txt") && !QFile::exists("System_parameters.txt")) {
			return false;
		}
		break;
	case STABILIZATION:
		if (!QFile::exists("stabilization_parameters.txt") || !QFile::exists("stabilization_parameters_Thermal.txt")) {
			return false;
		}
		break;

	case TRACKING:
		if (!QFile::exists("track_alg_parameters.txt") && !QFile::exists("track_parameters.txt")) {
			return false;
		}
		break;

	case PANAROMA:
		if (!QFile::exists("pan_params.txt") && !QFile::exists("pan_shift_parameters.txt") && !QFile::exists("panchange_parameters.txt")) {
			return false;
		}
		break;
	case FACE_DETECTION:
		break;
	case NONE:
		return true;
		break;
	}
}

void AlgorithmManager::startGrpc()
{
	GrpcThreadAlg *grpcServ = new GrpcThreadAlg(50059, this);
	grpcServ->start();
}

int AlgorithmManager::setPT(PTZinformation info)
{
	float vel_x = info.pan;
	float vel_y = info.tilt;
	PTZactions act = info.action;
	if (!ptzp)
		switch (act) {
		case PAN_STOP:
			//sadece pan stop komutu olursa
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
			//sadece tilt stop komutu olursa
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
		algHandler.locationInfo.pan = ptzp->getHead(1)->getPanAngle();
		algHandler.locationInfo.tilt = ptzp->getHead(1)->getTiltAngle();
		algHandler.locationInfo.zoom = ptzp->getHead(0)->getZoom();
	} else {
		qDebug() << "System is not defined so that location information is not available";
	}
}

int AlgorithmManager::setZoom(uint pos)
{
	//Lower and Upper bounds should be defined for the sake of robustness
	if (!ptzp)
		return -1;
	ptzp->getHead(0)->setZoom(pos);
	return 0;
}
