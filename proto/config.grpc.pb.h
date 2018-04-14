// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: config.proto
#ifndef GRPC_config_2eproto__INCLUDED
#define GRPC_config_2eproto__INCLUDED

#include "config.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace config {

class AppConfig final {
 public:
  static constexpr char const* service_full_name() {
    return "config.AppConfig";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status SetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> AsyncSetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(AsyncSetPanoramaRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> PrepareAsyncSetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(PrepareAsyncSetPanoramaRaw(context, request, cq));
    }
    virtual ::grpc::Status SetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::config::AppCommandResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> AsyncSetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(AsyncSetMotionDetectionRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> PrepareAsyncSetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(PrepareAsyncSetMotionDetectionRaw(context, request, cq));
    }
    virtual ::grpc::Status GetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> AsyncGetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(AsyncGetCurrentModeRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> PrepareAsyncGetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(PrepareAsyncGetCurrentModeRaw(context, request, cq));
    }
    virtual ::grpc::Status GetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> AsyncGetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(AsyncGetLastPanaromaFrameRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> PrepareAsyncGetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(PrepareAsyncGetLastPanaromaFrameRaw(context, request, cq));
    }
    virtual ::grpc::Status GetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> AsyncGetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(AsyncGetPanaromaFramesRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>> PrepareAsyncGetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>>(PrepareAsyncGetPanaromaFramesRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* AsyncSetPanoramaRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* PrepareAsyncSetPanoramaRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* AsyncSetMotionDetectionRaw(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* PrepareAsyncSetMotionDetectionRaw(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* AsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* PrepareAsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* AsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* PrepareAsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* AsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::config::AppCommandResult>* PrepareAsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status SetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> AsyncSetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(AsyncSetPanoramaRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> PrepareAsyncSetPanorama(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(PrepareAsyncSetPanoramaRaw(context, request, cq));
    }
    ::grpc::Status SetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::config::AppCommandResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> AsyncSetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(AsyncSetMotionDetectionRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> PrepareAsyncSetMotionDetection(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(PrepareAsyncSetMotionDetectionRaw(context, request, cq));
    }
    ::grpc::Status GetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> AsyncGetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(AsyncGetCurrentModeRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> PrepareAsyncGetCurrentMode(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(PrepareAsyncGetCurrentModeRaw(context, request, cq));
    }
    ::grpc::Status GetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> AsyncGetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(AsyncGetLastPanaromaFrameRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> PrepareAsyncGetLastPanaromaFrame(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(PrepareAsyncGetLastPanaromaFrameRaw(context, request, cq));
    }
    ::grpc::Status GetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::config::AppCommandResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> AsyncGetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(AsyncGetPanaromaFramesRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>> PrepareAsyncGetPanaromaFrames(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>>(PrepareAsyncGetPanaromaFramesRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AsyncSetPanoramaRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* PrepareAsyncSetPanoramaRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AsyncSetMotionDetectionRaw(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* PrepareAsyncSetMotionDetectionRaw(::grpc::ClientContext* context, const ::config::MotionDetectionParameters& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* PrepareAsyncGetCurrentModeRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* PrepareAsyncGetLastPanaromaFrameRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* AsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::config::AppCommandResult>* PrepareAsyncGetPanaromaFramesRaw(::grpc::ClientContext* context, const ::config::DummyInfo& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_SetPanorama_;
    const ::grpc::internal::RpcMethod rpcmethod_SetMotionDetection_;
    const ::grpc::internal::RpcMethod rpcmethod_GetCurrentMode_;
    const ::grpc::internal::RpcMethod rpcmethod_GetLastPanaromaFrame_;
    const ::grpc::internal::RpcMethod rpcmethod_GetPanaromaFrames_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status SetPanorama(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response);
    virtual ::grpc::Status SetMotionDetection(::grpc::ServerContext* context, const ::config::MotionDetectionParameters* request, ::config::AppCommandResult* response);
    virtual ::grpc::Status GetCurrentMode(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response);
    virtual ::grpc::Status GetLastPanaromaFrame(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response);
    virtual ::grpc::Status GetPanaromaFrames(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_SetPanorama : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_SetPanorama() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_SetPanorama() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SetPanorama(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestSetPanorama(::grpc::ServerContext* context, ::config::DummyInfo* request, ::grpc::ServerAsyncResponseWriter< ::config::AppCommandResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_SetMotionDetection : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_SetMotionDetection() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_SetMotionDetection() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SetMotionDetection(::grpc::ServerContext* context, const ::config::MotionDetectionParameters* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestSetMotionDetection(::grpc::ServerContext* context, ::config::MotionDetectionParameters* request, ::grpc::ServerAsyncResponseWriter< ::config::AppCommandResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_GetCurrentMode : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_GetCurrentMode() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_GetCurrentMode() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetCurrentMode(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetCurrentMode(::grpc::ServerContext* context, ::config::DummyInfo* request, ::grpc::ServerAsyncResponseWriter< ::config::AppCommandResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_GetLastPanaromaFrame : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_GetLastPanaromaFrame() {
      ::grpc::Service::MarkMethodAsync(3);
    }
    ~WithAsyncMethod_GetLastPanaromaFrame() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetLastPanaromaFrame(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetLastPanaromaFrame(::grpc::ServerContext* context, ::config::DummyInfo* request, ::grpc::ServerAsyncResponseWriter< ::config::AppCommandResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(3, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_GetPanaromaFrames : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_GetPanaromaFrames() {
      ::grpc::Service::MarkMethodAsync(4);
    }
    ~WithAsyncMethod_GetPanaromaFrames() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetPanaromaFrames(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetPanaromaFrames(::grpc::ServerContext* context, ::config::DummyInfo* request, ::grpc::ServerAsyncResponseWriter< ::config::AppCommandResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(4, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_SetPanorama<WithAsyncMethod_SetMotionDetection<WithAsyncMethod_GetCurrentMode<WithAsyncMethod_GetLastPanaromaFrame<WithAsyncMethod_GetPanaromaFrames<Service > > > > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_SetPanorama : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_SetPanorama() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_SetPanorama() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SetPanorama(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_SetMotionDetection : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_SetMotionDetection() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_SetMotionDetection() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SetMotionDetection(::grpc::ServerContext* context, const ::config::MotionDetectionParameters* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_GetCurrentMode : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_GetCurrentMode() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_GetCurrentMode() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetCurrentMode(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_GetLastPanaromaFrame : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_GetLastPanaromaFrame() {
      ::grpc::Service::MarkMethodGeneric(3);
    }
    ~WithGenericMethod_GetLastPanaromaFrame() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetLastPanaromaFrame(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_GetPanaromaFrames : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_GetPanaromaFrames() {
      ::grpc::Service::MarkMethodGeneric(4);
    }
    ~WithGenericMethod_GetPanaromaFrames() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetPanaromaFrames(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_SetPanorama : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_SetPanorama() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::config::DummyInfo, ::config::AppCommandResult>(std::bind(&WithStreamedUnaryMethod_SetPanorama<BaseClass>::StreamedSetPanorama, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_SetPanorama() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status SetPanorama(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedSetPanorama(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::config::DummyInfo,::config::AppCommandResult>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_SetMotionDetection : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_SetMotionDetection() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::StreamedUnaryHandler< ::config::MotionDetectionParameters, ::config::AppCommandResult>(std::bind(&WithStreamedUnaryMethod_SetMotionDetection<BaseClass>::StreamedSetMotionDetection, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_SetMotionDetection() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status SetMotionDetection(::grpc::ServerContext* context, const ::config::MotionDetectionParameters* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedSetMotionDetection(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::config::MotionDetectionParameters,::config::AppCommandResult>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetCurrentMode : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_GetCurrentMode() {
      ::grpc::Service::MarkMethodStreamed(2,
        new ::grpc::internal::StreamedUnaryHandler< ::config::DummyInfo, ::config::AppCommandResult>(std::bind(&WithStreamedUnaryMethod_GetCurrentMode<BaseClass>::StreamedGetCurrentMode, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_GetCurrentMode() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetCurrentMode(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetCurrentMode(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::config::DummyInfo,::config::AppCommandResult>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetLastPanaromaFrame : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_GetLastPanaromaFrame() {
      ::grpc::Service::MarkMethodStreamed(3,
        new ::grpc::internal::StreamedUnaryHandler< ::config::DummyInfo, ::config::AppCommandResult>(std::bind(&WithStreamedUnaryMethod_GetLastPanaromaFrame<BaseClass>::StreamedGetLastPanaromaFrame, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_GetLastPanaromaFrame() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetLastPanaromaFrame(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetLastPanaromaFrame(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::config::DummyInfo,::config::AppCommandResult>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetPanaromaFrames : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_GetPanaromaFrames() {
      ::grpc::Service::MarkMethodStreamed(4,
        new ::grpc::internal::StreamedUnaryHandler< ::config::DummyInfo, ::config::AppCommandResult>(std::bind(&WithStreamedUnaryMethod_GetPanaromaFrames<BaseClass>::StreamedGetPanaromaFrames, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_GetPanaromaFrames() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetPanaromaFrames(::grpc::ServerContext* context, const ::config::DummyInfo* request, ::config::AppCommandResult* response) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetPanaromaFrames(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::config::DummyInfo,::config::AppCommandResult>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_SetPanorama<WithStreamedUnaryMethod_SetMotionDetection<WithStreamedUnaryMethod_GetCurrentMode<WithStreamedUnaryMethod_GetLastPanaromaFrame<WithStreamedUnaryMethod_GetPanaromaFrames<Service > > > > > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_SetPanorama<WithStreamedUnaryMethod_SetMotionDetection<WithStreamedUnaryMethod_GetCurrentMode<WithStreamedUnaryMethod_GetLastPanaromaFrame<WithStreamedUnaryMethod_GetPanaromaFrames<Service > > > > > StreamedService;
};

}  // namespace config


#endif  // GRPC_config_2eproto__INCLUDED
