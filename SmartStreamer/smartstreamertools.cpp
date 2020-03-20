#include "smartstreamertools.h"
#include "kardelenapi.h"
#include "debug.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

#include <QStringList>
#include <QCommandLineParser>
#include <QCommandLineOption>

SmartStreamerTools::SmartStreamerTools()
{
	target = "127.0.0.1";
}

int SmartStreamerTools::toolsMain(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCommandLineParser parser;
	parser.setApplicationDescription("Application test tools"
									 "SmartStreamerTools -t 127.0.0.1 -o grpcv2 -a run_motion \"Custom parameters\"");
	parser.setApplicationDescription("Supported actions:\n"
									 "orionapi: run_motion, stop_motion\n"
									 "kaapi: position, gotopos, status, motroi, getnum, getenum, setcam, setnum, pan, tilt\n"
									 "grpcv1: run_motion_roi, run_motion_sens, run_track, run_face, stop_motion, stop_track, stop_face, alarm\n"
									 "grpcv2: get_list_of_algorithms, run_algorithm, stop_algorithm, get_system_feature_url, set_system_feature_url, get_system_feature_custom, set_system_feature_custom, get_algorithm_parameters\n"
									 );

	parser.addHelpOption();
	QCommandLineOption option1(QStringList() << "t" << "target", "Please Set remote server IP address", "Default:127.0.0.1");
	QCommandLineOption option2(QStringList() << "o" << "option", "Tools options: grpcv2, orionapi, kaapi, grpcv1", "Default:none");
	QCommandLineOption option3(QStringList() << "a" << "action", "Please select application action list.", "Default:none");
	QCommandLineOption option4(QStringList() << "args" << "arguments", "Set arguments for action", "Default:none");
	parser.addOption(option1);
	parser.addOption(option2);
	parser.addOption(option3);
	parser.addOption(option4);
	parser.process(a);
	QString t = parser.value(option1);
	if (!t.isEmpty())
		target = t;
	QString option = parser.value(option2);
	QString action = parser.value(option3);
	QStringList args = parser.value(option4).split(",");
	if (option == "grpcv2")
		return testGrpcV2(action, target);
	else if (option == "grpcv1")
		return testGrpc(action, target);
	else if (option == "orionapi")
		return testOrionGrpc(action, target);
	else if (option == "kaapi") {
		return kardelenAPIClient(action, args, target);
	}
	ffDebug() << "no suitable options found. Please use -h or --help";
	return 0;
}

int SmartStreamerTools::testGrpcV2(const QString &action, const QString &target)
{
	QString ep = QString("%1:50061").arg(target);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<algorithm::v2::AlgorithmService::Stub> stub = algorithm::v2::AlgorithmService::NewStub(chn);
	if (action == "get_list_of_algorithms") {
		algorithm::v2::AlgorithmListResponse resp;
		google::protobuf::Empty request;

		status = stub->ListAlgorithms(&ctx,request,&resp);
		if (resp.algorithms_size() != 0) {
			for (int i = 0; i < resp.algorithms_size(); i++) {
				qDebug() << QString::fromStdString(resp.algorithms(i).head_info().algorithm_node());
				qDebug() << QString::fromStdString(resp.algorithms(i).head_info().friendly_name());
				qDebug() << resp.algorithms(i).status().is_running();
			}
		}
	}
	if (action == "run_algorithm")
	{
		google::protobuf::Empty resp;
		algorithm::v2::AlgorithmHead request;
		std::string algorithm_node_name = "node0";
		request.set_algorithm_node(algorithm_node_name);
		request.set_friendly_name("Smart Motion");
		status = stub->RunAlgorithm(&ctx,request,&resp);
	}
	if (action == "stop_algorithm")
	{
		google::protobuf::Empty resp;
		algorithm::v2::AlgorithmHead request;
		std::string algorithm_node_name = "node0";
		request.set_algorithm_node(algorithm_node_name);
		request.set_friendly_name("Smart Motion");
		status = stub->StopAlgorithm(&ctx,request,&resp);
	}
	if (action == "get_system_feature_url")
	{
		algorithm::v2::SystemFeature resp;
		algorithm::v2::SystemFeature req;
		::algorithm::v2::FeatureIPVideoSource video_source;
		req.set_allocated_video_source(&video_source);
		status = stub->GetSystemFeature(&ctx, req, &resp);
		qDebug() << "url streamed by device is " << QString::fromStdString(resp.video_source().url());
	}
	if (action == "set_system_feature_url")
		{
			algorithm::v2::SystemFeature resp;
			algorithm::v2::SystemFeature req;
			::algorithm::v2::FeatureIPVideoSource video_source;
			//Will be implemented for setting URL
			status = stub->GetSystemFeature(&ctx, req, &resp);
		}
	if (action == "get_system_feature_custom")
		{
			algorithm::v2::SystemFeature resp;
			algorithm::v2::SystemFeature req;
			::algorithm::v2::FeatureCustom custom;
			custom.set_key("fusion_stream_mode");
			req.set_allocated_custom(&custom);
			status = stub->GetSystemFeature(&ctx, req, &resp);
			qDebug() << "custom feature from device is " << QString::fromStdString(resp.custom().value());
		}
	if (action == "set_system_feature_custom")
	{
		algorithm::v2::SystemFeature resp;
		algorithm::v2::SystemFeature req;
		::algorithm::v2::FeatureCustom custom;
		custom.set_key("fusion_stream_mode");
		custom.set_value("day");
		req.set_allocated_custom(&custom);
		status = stub->SetSystemFeature(&ctx, req, &resp);
		qDebug() << "custom feature from device is " << QString::fromStdString(resp.custom().value());
	}
	if (action == "get_algorithm_parameters")
	{
		algorithm::v2::AlgorithmParameters resp;
		algorithm::v2::AlgorithmHead req;
		std::string algorithm_node_name = "node0";
		req.set_algorithm_node(algorithm_node_name);
		status = stub->GetAlgorithmParameters(&ctx, req, &resp);
		qDebug() << "Algorithm Parameters are.....";
		qDebug() << "Video stabilization is " << resp.video_stabilization();
		qDebug() << "Tamper detection is " << resp.tamper_detection();
		qDebug() << "Privacy masking is " << resp.privacy_masking();
		qDebug() << "Pre_processing_degree is " << resp.pre_processing_degree();
		qDebug() << "Smart motion parameters are....";
		qDebug() << "--------------------------------";
		algorithm::v2::SmartMotionAlgorithmParameters smart_param = resp.smart_parameters();
		qDebug() << "Total Smart Motion Region size is " << smart_param.regions_size();
		for (int i = 0; i < smart_param.regions_size(); i++)
		{
			algorithm::v2::SmartMotionRegion region = smart_param.regions(i);
			qDebug() << "Wander time in msecs for region " << i << " is " << region.wander_time_msecs();
			qDebug() << "Stand time in msecs for region " << i << " is " << region.stand_time_msecs();
			qDebug() << "Is region " << i << " active? " << region.active();
			for (int j = 0; j < region.detection_region_point_size(); j++) {
				qDebug() << "Point " << j << " is " << region.detection_region_point(j).x() << region.detection_region_point(j).y();
			}
			for (int k = 0; k < region.motion_direction_size(); k++)
				qDebug() << region.motion_direction(k);
		}

		qDebug() << "min region rectangle is " << smart_param.min_region().top_left().x()
				 << smart_param.min_region().top_left().y()
				 << smart_param.min_region().bottom_right().x()
				 << smart_param.min_region().bottom_right().y();

		qDebug() << "max region rectangle is " << smart_param.max_region().top_left().x()
				 << smart_param.max_region().top_left().y()
				 << smart_param.max_region().bottom_right().x()
				 << smart_param.max_region().bottom_right().y();

		for (int m = 0; m < smart_param.lines_size(); m++) {
			algorithm::v2::LineCrossRegion line_param = smart_param.lines(m);
			qDebug() << "first point of line " << m << " is " << line_param.pt1().x() << line_param.pt1().y();
			qDebug() << "second point of line " << m << " is " << line_param.pt2().x() << line_param.pt2().y();
			qDebug() << "Is line " << m << " active? " << line_param.active();
		}
	}
	if (status.error_code() != grpc::OK) {
		qDebug("error '%d' in grpc call", status.error_code());
		return -1;
	}
	return 0;
}

int SmartStreamerTools::testOrionGrpc(const QString &action, const QString &target)
{
	QString ep = QString("%1:50060").arg(target);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<OrionCommunication::OrionCommunicationService::Stub> stub = OrionCommunication::OrionCommunicationService::NewStub(chn);
	OrionCommunication::AppCommandResult res;
	if (action == "run_motion") {
		qDebug() << "running motion";
		OrionCommunication::DummyInfo req;
		status = stub->RunMotion(&ctx, req, &res);
	} else if (action == "stop_motion") {
		qDebug() << "stoping motion";
		OrionCommunication::DummyInfo req;
		status = stub->StopMotion(&ctx, req, &res);
	}
	if (status.error_code() != grpc::OK) {
		qDebug("error '%d' in grpc call", status.error_code());
		return -1;
	}
	return 0;
}

int SmartStreamerTools::testGrpc(const QString &action, const QString &target)
{
	QString ep = QString("%1:50059").arg(target);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<AlgorithmCommunication::AlgorithmService::Stub> stub = AlgorithmCommunication::AlgorithmService::NewStub(chn);
	if (action == "run_motion_roi") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::MotionParameters *params = new AlgorithmCommunication::MotionParameters;
		AlgorithmCommunication::TRoi *Roi = new AlgorithmCommunication::TRoi;
		float points[16]{50,50,450,50,450,900,50,900,600,50,1800,50,1800,900,600,900};
		int polyCounter = 0;
		for (int i = 0; i < 2 ; ++i)
		{
			AlgorithmCommunication::TPolygon *Poly = Roi->add_polygon();
			Poly->set_is_active(true);
			for (int j = 0; j < 8; j += 2)
			{
				AlgorithmCommunication::TPoint *Point = Poly->add_points();
				Point->set_x(points[j + polyCounter]);
				Point->set_y(points[j + 1 + polyCounter]);
			}
			polyCounter = polyCounter + 4 * 2;
		}
		AlgorithmCommunication::TRectangle *myRect1 = Roi->mutable_rect1();
		AlgorithmCommunication::TRectangle *myRect2 = Roi->mutable_rect2();
		myRect1->mutable_upperleft()->set_x(100);
		myRect1->mutable_upperleft()->set_y(100);
		myRect1->mutable_bottomright()->set_x(200);
		myRect1->mutable_bottomright()->set_y(200);
		myRect2->mutable_upperleft()->set_x(300);
		myRect2->mutable_upperleft()->set_y(300);
		myRect2->mutable_bottomright()->set_x(500);
		myRect2->mutable_bottomright()->set_y(500);
		params->set_allocated_roilist(Roi);
		params->set_settingchoice(AlgorithmCommunication::MotionParameters_Settings_ROI);
		int a = qrand() % 100;
		params->set_sensitivity(a);
		req.set_allocated_motionparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} if (action == "run_motion_sens") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::MotionParameters *params = new AlgorithmCommunication::MotionParameters;
		params->set_settingchoice(AlgorithmCommunication::MotionParameters_Settings_SENSITIVITY);
		int a = qrand() % 100;
		params->set_sensitivity(a);
		req.set_allocated_motionparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "run_track"){
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_TRACKING);
		AlgorithmCommunication::TrackParameters *params = new AlgorithmCommunication::TrackParameters;
		params->set_tracktype(AlgorithmCommunication::TrackParameters_TrackType_AUTO);
		//		::AlgorithmCommunication::TrackObject* target;
		//		target->set_point_x(0.5);
		//		target->set_point_y(0.5);
		//		params->set_allocated_target(target);
		req.set_allocated_trackparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "run_face") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_FACE_DETECTION);
		AlgorithmCommunication::FaceDetectionParameters *params = new AlgorithmCommunication::FaceDetectionParameters;
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_motion") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_track") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_TRACKING);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_face") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_FACE_DETECTION);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "alarm") {
		AlgorithmCommunication::AlarmReqInfo req;
		req.set_intervalmsecs(1000);
		auto stream = stub->GetAlarm(&ctx);
		if (!stream->Write(req)) {
			qDebug("Error starting getalarm request");
			return -1;
		}
		AlgorithmCommunication::Alarms res;
		while (stream->Read(&res)) {
			QDateTime dt = QDateTime::fromMSecsSinceEpoch(res.ts());
			qDebug("%d alarms at %s", res.alarms_size(), qPrintable(dt.toString()));
			for (int i = 0; i < res.alarms_size(); i++) {
				const AlgorithmCommunication::Alarm &alarm = res.alarms(i);
				qDebug("Alarm %d type is '%s'", i, alarm.type().data());
				if (alarm.type() == "cit") {
					const AlgorithmCommunication::CITInfo ci = alarm.cit();
					qDebug("module %d, pt %d, usb %d", ci.moduleinfo(), ci.motorinfo(), ci.usbinfo());
				}
			}

			req.set_intervalmsecs(1000);
			stream->Write(req);
		}
	}
	if (status.error_code() != grpc::OK) {
		qDebug("error '%d' in grpc call", status.error_code());
		return -1;
	}
	return 0;
}

int SmartStreamerTools::kardelenAPIClient(const QString &action, QStringList args, const QString &target)
{
	google::protobuf::Empty ereq;
	QString ep = QString("%1:50059").arg(target);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<kaapi::CameraService::Stub> stub = kaapi::CameraService::NewStub(chn);
	kaapi::PosInfo posi;
	if (args.isEmpty()) {
		ffDebug() << "This api wants to get arguments";
		return -10;
	}
	if (action == "position") {
		status = stub->GetPosition(&ctx, ereq, &posi);
		fDebug("Got position: %lf %lf", posi.panpos(), posi.tiltpos());
	} else if (action == "gotopos") {
		QString panpos = args.at(0);
		QString tiltpos =args.at(1);
		kaapi::AbsoluteMoveResult res;
		kaapi::AbsoluteMoveParameters req;
		req.set_panpos(panpos.toFloat());
		req.set_tiltpos(tiltpos.toFloat());
		status = stub->MoveAbsolute(&ctx, req, &res);
	} else if (action == "motroi") {
		ffDebug() << "motion roi";
		kaapi::MotionROIRequest req;
		kaapi::Rectangle *r = req.mutable_far();
		r->set_width(10);
		r->set_height(20);
		r = req.mutable_close();
		r->set_width(30);
		r->set_height(40);
		google::protobuf::Empty res;
		status = stub->SetMotionROI(&ctx, req, &res);
	} else if (action == "status") {
		kaapi::CameraStatus cstatus;
		kaapi::GetCameraStatusParameters preq;
		status = stub->GetCameraStatus(&ctx, preq, &cstatus);
		fDebug("capabilities = 0x%lx", cstatus.capabilities());
	} else if (action == "getnum") {
		QString index = args.at(0);
		kaapi::GetNumericParameterResponse resp;
		kaapi::GetNumericParameterRequest preq;
		preq.set_index(index.toInt());
		status = stub->GetNumericParameter(&ctx, preq, &resp);
		fDebug("num param (%d): %lf", index.toInt(), resp.value().value());
	} else if (action == "getenum") {
		QString index = args.at(0);
		kaapi::GetEnumParameterResponse resp;
		kaapi::GetEnumParameterRequest preq;
		preq.set_index(index.toInt());
		status = stub->GetEnumParameter(&ctx, preq, &resp);
		int32_t v = resp.value();
		fDebug("enum param (%d): %d", index.toInt(), v);
	} else if (action == "setcam") {
		QString cam = args.at(0);
		kaapi::SetCameraResponse resp;
		kaapi::SetCameraRequest preq;
		preq.set_cameratype(cam.toInt());
		status = stub->SetCamera(&ctx, preq, &resp);
		fDebug("setcam (%d): 0x%lx", cam.toInt(), resp.capabilities());
	} else if (action == "setnum") {
		QString index = args.at(0);
		QString value = args.at(1);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(index.toInt());
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		status = stub->SetNumericParameter(&ctx, preq, &resp);
		fDebug("set numeric parameter ok");
	} else if (action == "pan") {
		QString value = args.at(0);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(kaapi::NUM_PARAM_YAW);
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		np->set_byte2(0x0);
		status = stub->SetNumericParameter(&ctx, preq, &resp);
		fDebug("pan ok");
	} else if (action == "tilt") {
		QString value = args.at(0);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(kaapi::NUM_PARAM_PITCH);
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		np->set_byte2(0x0);
		status = stub->SetNumericParameter(&ctx, preq, &resp);
	}
	if (status.error_code() != grpc::OK) {
		qDebug("Action '%s' , error '%d' in grpc call", action, status.error_code());
		return -1;
	}
	return 0;
}
