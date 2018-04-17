// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: config.proto

#include "config.pb.h"
#include "config.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace config {

static const char* AppConfig_method_names[] = {
  "/config.AppConfig/SetPanaromaParameters",
  "/config.AppConfig/SetMotionDetectionParameters",
  "/config.AppConfig/SetCurrentMode",
  "/config.AppConfig/GetCurrentMode",
  "/config.AppConfig/GetLastPanaromaFrame",
  "/config.AppConfig/GetPanaromaFrames",
};

std::unique_ptr< AppConfig::Stub> AppConfig::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< AppConfig::Stub> stub(new AppConfig::Stub(channel));
  return stub;
}

AppConfig::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_SetPanaromaParameters_(AppConfig_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetMotionDetectionParameters_(AppConfig_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetCurrentMode_(AppConfig_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetCurrentMode_(AppConfig_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetLastPanaromaFrame_(AppConfig_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetPanaromaFrames_(AppConfig_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status AppConfig::Stub::SetPanaromaParameters(::grpc::ClientContext* context, const ::config::PanoramaPars& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetPanaromaParameters_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncSetPanaromaParametersRaw(::grpc::ClientContext* context, const ::config::PanoramaPars& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetPanaromaParameters_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncSetPanaromaParametersRaw(::grpc::ClientContext* context, const ::config::PanoramaPars& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetPanaromaParameters_, context, request, false);
}

::grpc::Status AppConfig::Stub::SetMotionDetectionParameters(::grpc::ClientContext* context, const ::config::MotDetPars& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetMotionDetectionParameters_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncSetMotionDetectionParametersRaw(::grpc::ClientContext* context, const ::config::MotDetPars& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetMotionDetectionParameters_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncSetMotionDetectionParametersRaw(::grpc::ClientContext* context, const ::config::MotDetPars& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetMotionDetectionParameters_, context, request, false);
}

::grpc::Status AppConfig::Stub::SetCurrentMode(::grpc::ClientContext* context, const ::config::SetModeQ& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetCurrentMode_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncSetCurrentModeRaw(::grpc::ClientContext* context, const ::config::SetModeQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetCurrentMode_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncSetCurrentModeRaw(::grpc::ClientContext* context, const ::config::SetModeQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_SetCurrentMode_, context, request, false);
}

::grpc::Status AppConfig::Stub::GetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetCurrentMode_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetCurrentMode_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetCurrentMode_, context, request, false);
}

::grpc::Status AppConfig::Stub::GetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetLastPanaromaFrame_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetLastPanaromaFrame_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetLastPanaromaFrame_, context, request, false);
}

::grpc::Status AppConfig::Stub::GetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetPanaromaFrames_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::AsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetPanaromaFrames_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AppConfig::Stub::PrepareAsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::config::AppCommandResult>::Create(channel_.get(), cq, rpcmethod_GetPanaromaFrames_, context, request, false);
}

AppConfig::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::PanoramaPars, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::SetPanaromaParameters), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::MotDetPars, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::SetMotionDetectionParameters), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::SetModeQ, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::SetCurrentMode), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::DummyInfo, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::GetCurrentMode), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::DummyInfo, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::GetLastPanaromaFrame), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AppConfig_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< AppConfig::Service, ::config::DummyInfo, ::config::AppCommandResult>(
          std::mem_fn(&AppConfig::Service::GetPanaromaFrames), this)));
}

AppConfig::Service::~Service() {
}

::grpc::Status AppConfig::Service::SetPanaromaParameters(::grpc::ServerContext* context, const ::config::PanoramaPars* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AppConfig::Service::SetMotionDetectionParameters(::grpc::ServerContext* context, const ::config::MotDetPars* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AppConfig::Service::SetCurrentMode(::grpc::ServerContext* context, const ::config::SetModeQ* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AppConfig::Service::GetCurrentMode(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AppConfig::Service::GetLastPanaromaFrame(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AppConfig::Service::GetPanaromaFrames(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace config
