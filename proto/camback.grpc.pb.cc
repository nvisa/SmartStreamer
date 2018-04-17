// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: camback.proto

#include "camback.pb.h"
#include "camback.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace camback {

static const char* PTZService_method_names[] = {
  "/camback.PTZService/GetPTZPosInfo",
  "/camback.PTZService/PanLeft",
  "/camback.PTZService/PanRight",
  "/camback.PTZService/TiltUp",
  "/camback.PTZService/TiltDown",
  "/camback.PTZService/PanStop",
  "/camback.PTZService/PanTiltAbs",
  "/camback.PTZService/ZoomIn",
  "/camback.PTZService/ZoomOut",
  "/camback.PTZService/ZoomStop",
  "/camback.PTZService/PanAbsCmd",
  "/camback.PTZService/TiltAbsCmd",
  "/camback.PTZService/SetPanTiltPos",
};

std::unique_ptr< PTZService::Stub> PTZService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< PTZService::Stub> stub(new PTZService::Stub(channel));
  return stub;
}

PTZService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_GetPTZPosInfo_(PTZService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_PanLeft_(PTZService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_PanRight_(PTZService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_TiltUp_(PTZService_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_TiltDown_(PTZService_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_PanStop_(PTZService_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_PanTiltAbs_(PTZService_method_names[6], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ZoomIn_(PTZService_method_names[7], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ZoomOut_(PTZService_method_names[8], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ZoomStop_(PTZService_method_names[9], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_PanAbsCmd_(PTZService_method_names[10], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_TiltAbsCmd_(PTZService_method_names[11], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetPanTiltPos_(PTZService_method_names[12], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status PTZService::Stub::GetPTZPosInfo(::grpc::ClientContext* context, const ::camback::PTZInfoQ& request, ::camback::PTZPosInfo* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetPTZPosInfo_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PTZPosInfo>* PTZService::Stub::AsyncGetPTZPosInfoRaw(::grpc::ClientContext* context, const ::camback::PTZInfoQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PTZPosInfo>::Create(channel_.get(), cq, rpcmethod_GetPTZPosInfo_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PTZPosInfo>* PTZService::Stub::PrepareAsyncGetPTZPosInfoRaw(::grpc::ClientContext* context, const ::camback::PTZInfoQ& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PTZPosInfo>::Create(channel_.get(), cq, rpcmethod_GetPTZPosInfo_, context, request, false);
}

::grpc::Status PTZService::Stub::PanLeft(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_PanLeft_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncPanLeftRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanLeft_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncPanLeftRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanLeft_, context, request, false);
}

::grpc::Status PTZService::Stub::PanRight(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_PanRight_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncPanRightRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanRight_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncPanRightRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanRight_, context, request, false);
}

::grpc::Status PTZService::Stub::TiltUp(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_TiltUp_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncTiltUpRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltUp_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncTiltUpRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltUp_, context, request, false);
}

::grpc::Status PTZService::Stub::TiltDown(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_TiltDown_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncTiltDownRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltDown_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncTiltDownRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltDown_, context, request, false);
}

::grpc::Status PTZService::Stub::PanStop(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_PanStop_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncPanStopRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanStop_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncPanStopRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanStop_, context, request, false);
}

::grpc::Status PTZService::Stub::PanTiltAbs(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_PanTiltAbs_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncPanTiltAbsRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanTiltAbs_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncPanTiltAbsRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanTiltAbs_, context, request, false);
}

::grpc::Status PTZService::Stub::ZoomIn(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ZoomIn_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncZoomInRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomIn_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncZoomInRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomIn_, context, request, false);
}

::grpc::Status PTZService::Stub::ZoomOut(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ZoomOut_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncZoomOutRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomOut_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncZoomOutRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomOut_, context, request, false);
}

::grpc::Status PTZService::Stub::ZoomStop(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_ZoomStop_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncZoomStopRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomStop_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncZoomStopRaw(::grpc::ClientContext* context, const ::camback::PtzCmdPar& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_ZoomStop_, context, request, false);
}

::grpc::Status PTZService::Stub::PanAbsCmd(::grpc::ClientContext* context, const ::camback::PanCmdInfo& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_PanAbsCmd_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncPanAbsCmdRaw(::grpc::ClientContext* context, const ::camback::PanCmdInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanAbsCmd_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncPanAbsCmdRaw(::grpc::ClientContext* context, const ::camback::PanCmdInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_PanAbsCmd_, context, request, false);
}

::grpc::Status PTZService::Stub::TiltAbsCmd(::grpc::ClientContext* context, const ::camback::TiltCmdInfo& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_TiltAbsCmd_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncTiltAbsCmdRaw(::grpc::ClientContext* context, const ::camback::TiltCmdInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltAbsCmd_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncTiltAbsCmdRaw(::grpc::ClientContext* context, const ::camback::TiltCmdInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_TiltAbsCmd_, context, request, false);
}

::grpc::Status PTZService::Stub::SetPanTiltPos(::grpc::ClientContext* context, const ::camback::PanTiltPos& request, ::camback::PtzCommandResult* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetPanTiltPos_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::AsyncSetPanTiltPosRaw(::grpc::ClientContext* context, const ::camback::PanTiltPos& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_SetPanTiltPos_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::camback::PtzCommandResult>* PTZService::Stub::PrepareAsyncSetPanTiltPosRaw(::grpc::ClientContext* context, const ::camback::PanTiltPos& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::camback::PtzCommandResult>::Create(channel_.get(), cq, rpcmethod_SetPanTiltPos_, context, request, false);
}

PTZService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PTZInfoQ, ::camback::PTZPosInfo>(
          std::mem_fn(&PTZService::Service::GetPTZPosInfo), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::PanLeft), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::PanRight), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::TiltUp), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::TiltDown), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::PanStop), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::PanTiltAbs), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::ZoomIn), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::ZoomOut), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[9],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PtzCmdPar, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::ZoomStop), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[10],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PanCmdInfo, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::PanAbsCmd), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[11],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::TiltCmdInfo, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::TiltAbsCmd), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      PTZService_method_names[12],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< PTZService::Service, ::camback::PanTiltPos, ::camback::PtzCommandResult>(
          std::mem_fn(&PTZService::Service::SetPanTiltPos), this)));
}

PTZService::Service::~Service() {
}

::grpc::Status PTZService::Service::GetPTZPosInfo(::grpc::ServerContext* context, const ::camback::PTZInfoQ* request, ::camback::PTZPosInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::PanLeft(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::PanRight(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::TiltUp(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::TiltDown(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::PanStop(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::PanTiltAbs(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::ZoomIn(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::ZoomOut(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::ZoomStop(::grpc::ServerContext* context, const ::camback::PtzCmdPar* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::PanAbsCmd(::grpc::ServerContext* context, const ::camback::PanCmdInfo* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::TiltAbsCmd(::grpc::ServerContext* context, const ::camback::TiltCmdInfo* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status PTZService::Service::SetPanTiltPos(::grpc::ServerContext* context, const ::camback::PanTiltPos* request, ::camback::PtzCommandResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace camback

