// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: AlgorithmCommunication.proto

#include "AlgorithmCommunication.pb.h"
#include "AlgorithmCommunication.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace AlgorithmCommunication {

static const char* AlgorithmService_method_names[] = {
  "/AlgorithmCommunication.AlgorithmService/GetScreenShot",
  "/AlgorithmCommunication.AlgorithmService/GetScreenShotStream",
  "/AlgorithmCommunication.AlgorithmService/RunAlgorithm",
  "/AlgorithmCommunication.AlgorithmService/StopAlgorithm",
  "/AlgorithmCommunication.AlgorithmService/SetAlgorithmParameters",
  "/AlgorithmCommunication.AlgorithmService/GetAlgorithmParameters",
  "/AlgorithmCommunication.AlgorithmService/GetSystemFeature",
  "/AlgorithmCommunication.AlgorithmService/SetSystemFeature",
  "/AlgorithmCommunication.AlgorithmService/GetAllSystemFeatures",
  "/AlgorithmCommunication.AlgorithmService/SetAllSystemFeatures",
  "/AlgorithmCommunication.AlgorithmService/GetAlarm",
};

std::unique_ptr< AlgorithmService::Stub> AlgorithmService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< AlgorithmService::Stub> stub(new AlgorithmService::Stub(channel));
  return stub;
}

AlgorithmService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_GetScreenShot_(AlgorithmService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetScreenShotStream_(AlgorithmService_method_names[1], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_RunAlgorithm_(AlgorithmService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_StopAlgorithm_(AlgorithmService_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetAlgorithmParameters_(AlgorithmService_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetAlgorithmParameters_(AlgorithmService_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetSystemFeature_(AlgorithmService_method_names[6], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetSystemFeature_(AlgorithmService_method_names[7], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetAllSystemFeatures_(AlgorithmService_method_names[8], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetAllSystemFeatures_(AlgorithmService_method_names[9], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetAlarm_(AlgorithmService_method_names[10], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  {}

::grpc::Status AlgorithmService::Stub::GetScreenShot(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request, ::AlgorithmCommunication::ScreenFrame* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetScreenShot_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::GetScreenShot(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo* request, ::AlgorithmCommunication::ScreenFrame* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetScreenShot_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ScreenFrame>* AlgorithmService::Stub::AsyncGetScreenShotRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(channel_.get(), cq, rpcmethod_GetScreenShot_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ScreenFrame>* AlgorithmService::Stub::PrepareAsyncGetScreenShotRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(channel_.get(), cq, rpcmethod_GetScreenShot_, context, request, false);
}

::grpc::ClientReader< ::AlgorithmCommunication::ScreenFrame>* AlgorithmService::Stub::GetScreenShotStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request) {
  return ::grpc::internal::ClientReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(channel_.get(), rpcmethod_GetScreenShotStream_, context, request);
}

void AlgorithmService::Stub::experimental_async::GetScreenShotStream(::grpc::ClientContext* context, ::AlgorithmCommunication::ScreenshotInfo* request, ::grpc::experimental::ClientReadReactor< ::AlgorithmCommunication::ScreenFrame>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(stub_->channel_.get(), stub_->rpcmethod_GetScreenShotStream_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::AlgorithmCommunication::ScreenFrame>* AlgorithmService::Stub::AsyncGetScreenShotStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(channel_.get(), cq, rpcmethod_GetScreenShotStream_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::AlgorithmCommunication::ScreenFrame>* AlgorithmService::Stub::PrepareAsyncGetScreenShotStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::ScreenshotInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::AlgorithmCommunication::ScreenFrame>::Create(channel_.get(), cq, rpcmethod_GetScreenShotStream_, context, request, false, nullptr);
}

::grpc::Status AlgorithmService::Stub::RunAlgorithm(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_RunAlgorithm_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::RunAlgorithm(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_RunAlgorithm_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::AsyncRunAlgorithmRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_RunAlgorithm_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::PrepareAsyncRunAlgorithmRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_RunAlgorithm_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::StopAlgorithm(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_StopAlgorithm_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::StopAlgorithm(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_StopAlgorithm_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::AsyncStopAlgorithmRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_StopAlgorithm_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::PrepareAsyncStopAlgorithmRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_StopAlgorithm_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::SetAlgorithmParameters(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetAlgorithmParameters_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::SetAlgorithmParameters(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SetAlgorithmParameters_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::AsyncSetAlgorithmParametersRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_SetAlgorithmParameters_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfRequests>* AlgorithmService::Stub::PrepareAsyncSetAlgorithmParametersRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfRequests>::Create(channel_.get(), cq, rpcmethod_SetAlgorithmParameters_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::GetAlgorithmParameters(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::AlgorithmCommunication::ResponseOfParameterRequests* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetAlgorithmParameters_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::GetAlgorithmParameters(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfParameterRequests* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetAlgorithmParameters_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfParameterRequests>* AlgorithmService::Stub::AsyncGetAlgorithmParametersRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfParameterRequests>::Create(channel_.get(), cq, rpcmethod_GetAlgorithmParameters_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::ResponseOfParameterRequests>* AlgorithmService::Stub::PrepareAsyncGetAlgorithmParametersRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::RequestForAlgorithm& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::ResponseOfParameterRequests>::Create(channel_.get(), cq, rpcmethod_GetAlgorithmParameters_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::GetSystemFeature(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::AlgorithmCommunication::SystemFeature* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetSystemFeature_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::GetSystemFeature(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature* request, ::AlgorithmCommunication::SystemFeature* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetSystemFeature_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeature>* AlgorithmService::Stub::AsyncGetSystemFeatureRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeature>::Create(channel_.get(), cq, rpcmethod_GetSystemFeature_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeature>* AlgorithmService::Stub::PrepareAsyncGetSystemFeatureRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeature>::Create(channel_.get(), cq, rpcmethod_GetSystemFeature_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::SetSystemFeature(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::AlgorithmCommunication::SystemFeature* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetSystemFeature_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::SetSystemFeature(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature* request, ::AlgorithmCommunication::SystemFeature* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SetSystemFeature_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeature>* AlgorithmService::Stub::AsyncSetSystemFeatureRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeature>::Create(channel_.get(), cq, rpcmethod_SetSystemFeature_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeature>* AlgorithmService::Stub::PrepareAsyncSetSystemFeatureRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeature& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeature>::Create(channel_.get(), cq, rpcmethod_SetSystemFeature_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::GetAllSystemFeatures(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::AlgorithmCommunication::SystemFeatures* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetAllSystemFeatures_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::GetAllSystemFeatures(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures* request, ::AlgorithmCommunication::SystemFeatures* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetAllSystemFeatures_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeatures>* AlgorithmService::Stub::AsyncGetAllSystemFeaturesRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeatures>::Create(channel_.get(), cq, rpcmethod_GetAllSystemFeatures_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeatures>* AlgorithmService::Stub::PrepareAsyncGetAllSystemFeaturesRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeatures>::Create(channel_.get(), cq, rpcmethod_GetAllSystemFeatures_, context, request, false);
}

::grpc::Status AlgorithmService::Stub::SetAllSystemFeatures(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::AlgorithmCommunication::SystemFeatures* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetAllSystemFeatures_, context, request, response);
}

void AlgorithmService::Stub::experimental_async::SetAllSystemFeatures(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures* request, ::AlgorithmCommunication::SystemFeatures* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SetAllSystemFeatures_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeatures>* AlgorithmService::Stub::AsyncSetAllSystemFeaturesRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeatures>::Create(channel_.get(), cq, rpcmethod_SetAllSystemFeatures_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::SystemFeatures>* AlgorithmService::Stub::PrepareAsyncSetAllSystemFeaturesRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::SystemFeatures& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::SystemFeatures>::Create(channel_.get(), cq, rpcmethod_SetAllSystemFeatures_, context, request, false);
}

::grpc::ClientReaderWriter< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>* AlgorithmService::Stub::GetAlarmRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>::Create(channel_.get(), rpcmethod_GetAlarm_, context);
}

void AlgorithmService::Stub::experimental_async::GetAlarm(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::AlgorithmCommunication::AlarmReqInfo,::AlgorithmCommunication::Alarms>* reactor) {
  ::grpc::internal::ClientCallbackReaderWriterFactory< ::AlgorithmCommunication::AlarmReqInfo,::AlgorithmCommunication::Alarms>::Create(stub_->channel_.get(), stub_->rpcmethod_GetAlarm_, context, reactor);
}

::grpc::ClientAsyncReaderWriter< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>* AlgorithmService::Stub::AsyncGetAlarmRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>::Create(channel_.get(), cq, rpcmethod_GetAlarm_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>* AlgorithmService::Stub::PrepareAsyncGetAlarmRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>::Create(channel_.get(), cq, rpcmethod_GetAlarm_, context, false, nullptr);
}

AlgorithmService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::ScreenshotInfo, ::AlgorithmCommunication::ScreenFrame>(
          std::mem_fn(&AlgorithmService::Service::GetScreenShot), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< AlgorithmService::Service, ::AlgorithmCommunication::ScreenshotInfo, ::AlgorithmCommunication::ScreenFrame>(
          std::mem_fn(&AlgorithmService::Service::GetScreenShotStream), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::RequestForAlgorithm, ::AlgorithmCommunication::ResponseOfRequests>(
          std::mem_fn(&AlgorithmService::Service::RunAlgorithm), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::RequestForAlgorithm, ::AlgorithmCommunication::ResponseOfRequests>(
          std::mem_fn(&AlgorithmService::Service::StopAlgorithm), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::RequestForAlgorithm, ::AlgorithmCommunication::ResponseOfRequests>(
          std::mem_fn(&AlgorithmService::Service::SetAlgorithmParameters), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::RequestForAlgorithm, ::AlgorithmCommunication::ResponseOfParameterRequests>(
          std::mem_fn(&AlgorithmService::Service::GetAlgorithmParameters), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::SystemFeature, ::AlgorithmCommunication::SystemFeature>(
          std::mem_fn(&AlgorithmService::Service::GetSystemFeature), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::SystemFeature, ::AlgorithmCommunication::SystemFeature>(
          std::mem_fn(&AlgorithmService::Service::SetSystemFeature), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::SystemFeatures, ::AlgorithmCommunication::SystemFeatures>(
          std::mem_fn(&AlgorithmService::Service::GetAllSystemFeatures), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[9],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AlgorithmService::Service, ::AlgorithmCommunication::SystemFeatures, ::AlgorithmCommunication::SystemFeatures>(
          std::mem_fn(&AlgorithmService::Service::SetAllSystemFeatures), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AlgorithmService_method_names[10],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< AlgorithmService::Service, ::AlgorithmCommunication::AlarmReqInfo, ::AlgorithmCommunication::Alarms>(
          std::mem_fn(&AlgorithmService::Service::GetAlarm), this)));
}

AlgorithmService::Service::~Service() {
}

::grpc::Status AlgorithmService::Service::GetScreenShot(::grpc::ServerContext* context, const ::AlgorithmCommunication::ScreenshotInfo* request, ::AlgorithmCommunication::ScreenFrame* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::GetScreenShotStream(::grpc::ServerContext* context, const ::AlgorithmCommunication::ScreenshotInfo* request, ::grpc::ServerWriter< ::AlgorithmCommunication::ScreenFrame>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::RunAlgorithm(::grpc::ServerContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::StopAlgorithm(::grpc::ServerContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::SetAlgorithmParameters(::grpc::ServerContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfRequests* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::GetAlgorithmParameters(::grpc::ServerContext* context, const ::AlgorithmCommunication::RequestForAlgorithm* request, ::AlgorithmCommunication::ResponseOfParameterRequests* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::GetSystemFeature(::grpc::ServerContext* context, const ::AlgorithmCommunication::SystemFeature* request, ::AlgorithmCommunication::SystemFeature* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::SetSystemFeature(::grpc::ServerContext* context, const ::AlgorithmCommunication::SystemFeature* request, ::AlgorithmCommunication::SystemFeature* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::GetAllSystemFeatures(::grpc::ServerContext* context, const ::AlgorithmCommunication::SystemFeatures* request, ::AlgorithmCommunication::SystemFeatures* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::SetAllSystemFeatures(::grpc::ServerContext* context, const ::AlgorithmCommunication::SystemFeatures* request, ::AlgorithmCommunication::SystemFeatures* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AlgorithmService::Service::GetAlarm(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AlgorithmCommunication::Alarms, ::AlgorithmCommunication::AlarmReqInfo>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* DiagnosticService_method_names[] = {
  "/AlgorithmCommunication.DiagnosticService/GetDiagnosticStatusStream",
  "/AlgorithmCommunication.DiagnosticService/GetDiagnosticStatus",
};

std::unique_ptr< DiagnosticService::Stub> DiagnosticService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< DiagnosticService::Stub> stub(new DiagnosticService::Stub(channel));
  return stub;
}

DiagnosticService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_GetDiagnosticStatusStream_(DiagnosticService_method_names[0], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_GetDiagnosticStatus_(DiagnosticService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::ClientReader< ::AlgorithmCommunication::DiagnosticInfo>* DiagnosticService::Stub::GetDiagnosticStatusStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request) {
  return ::grpc::internal::ClientReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(channel_.get(), rpcmethod_GetDiagnosticStatusStream_, context, request);
}

void DiagnosticService::Stub::experimental_async::GetDiagnosticStatusStream(::grpc::ClientContext* context, ::AlgorithmCommunication::DiagnosticQ* request, ::grpc::experimental::ClientReadReactor< ::AlgorithmCommunication::DiagnosticInfo>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(stub_->channel_.get(), stub_->rpcmethod_GetDiagnosticStatusStream_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::AlgorithmCommunication::DiagnosticInfo>* DiagnosticService::Stub::AsyncGetDiagnosticStatusStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(channel_.get(), cq, rpcmethod_GetDiagnosticStatusStream_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::AlgorithmCommunication::DiagnosticInfo>* DiagnosticService::Stub::PrepareAsyncGetDiagnosticStatusStreamRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(channel_.get(), cq, rpcmethod_GetDiagnosticStatusStream_, context, request, false, nullptr);
}

::grpc::Status DiagnosticService::Stub::GetDiagnosticStatus(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request, ::AlgorithmCommunication::DiagnosticInfo* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetDiagnosticStatus_, context, request, response);
}

void DiagnosticService::Stub::experimental_async::GetDiagnosticStatus(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ* request, ::AlgorithmCommunication::DiagnosticInfo* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetDiagnosticStatus_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::DiagnosticInfo>* DiagnosticService::Stub::AsyncGetDiagnosticStatusRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(channel_.get(), cq, rpcmethod_GetDiagnosticStatus_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::AlgorithmCommunication::DiagnosticInfo>* DiagnosticService::Stub::PrepareAsyncGetDiagnosticStatusRaw(::grpc::ClientContext* context, const ::AlgorithmCommunication::DiagnosticQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::AlgorithmCommunication::DiagnosticInfo>::Create(channel_.get(), cq, rpcmethod_GetDiagnosticStatus_, context, request, false);
}

DiagnosticService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DiagnosticService_method_names[0],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< DiagnosticService::Service, ::AlgorithmCommunication::DiagnosticQ, ::AlgorithmCommunication::DiagnosticInfo>(
          std::mem_fn(&DiagnosticService::Service::GetDiagnosticStatusStream), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      DiagnosticService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< DiagnosticService::Service, ::AlgorithmCommunication::DiagnosticQ, ::AlgorithmCommunication::DiagnosticInfo>(
          std::mem_fn(&DiagnosticService::Service::GetDiagnosticStatus), this)));
}

DiagnosticService::Service::~Service() {
}

::grpc::Status DiagnosticService::Service::GetDiagnosticStatusStream(::grpc::ServerContext* context, const ::AlgorithmCommunication::DiagnosticQ* request, ::grpc::ServerWriter< ::AlgorithmCommunication::DiagnosticInfo>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status DiagnosticService::Service::GetDiagnosticStatus(::grpc::ServerContext* context, const ::AlgorithmCommunication::DiagnosticQ* request, ::AlgorithmCommunication::DiagnosticInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace AlgorithmCommunication

