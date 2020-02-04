#include "algorithmgrpcserverv2.h"
#include "libsmartelement.h"

using namespace algorithm::v2;

AlgorithmGrpcServerV2 *AlgorithmGrpcServerV2::instance()
{
	static AlgorithmGrpcServerV2 *inst = nullptr;
	if (!inst)
		inst = new AlgorithmGrpcServerV2;
	return inst;
}

AlgorithmGrpcServerV2::AlgorithmGrpcServerV2()
	: AlgorithmService::Service()
{

}

grpc::Status AlgorithmGrpcServerV2::GetScreenShot(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, algorithm::v2::ScreenFrame *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::GetScreenShotStream(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, ::grpc::ServerWriter<algorithm::v2::ScreenFrame> *writer)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::RunAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	el->setPassThru(false);

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::RestartAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::StopAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	el->setPassThru(true);

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::SetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmParametersSetRequest *request, google::protobuf::Empty *response)
{
	if (request->algorithm().head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm().algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto pars = request->parameters();
	auto el = LibSmartElement::instance();
}

grpc::Status AlgorithmGrpcServerV2::GetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, algorithm::v2::AlgorithmParameters *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	response->CopyFrom(el->getParameters());

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::ListAlgorithms(grpc::ServerContext *context, const google::protobuf::Empty *request, algorithm::v2::AlgorithmListResponse *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::GetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::SetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::GetAllSystemFeatures(grpc::ServerContext *context, const algorithm::v2::SystemFeatures *request, algorithm::v2::SystemFeatures *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::SetAllSystemFeatures(grpc::ServerContext *context, const algorithm::v2::SystemFeatures *request, algorithm::v2::SystemFeatures *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::GetAlarm(grpc::ServerContext *context, ::grpc::ServerReaderWriter<algorithm::v2::Alarms, algorithm::v2::AlarmReqInfo> *stream)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::AssignTask(grpc::ServerContext *context, const algorithm::v2::TaskInformation *request, google::protobuf::Empty *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}
