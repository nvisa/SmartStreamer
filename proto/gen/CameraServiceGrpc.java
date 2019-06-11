package tr.com.aselsan.kardelen.orion.message.definition;

import static io.grpc.MethodDescriptor.generateFullMethodName;
import static io.grpc.stub.ClientCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ClientCalls.asyncClientStreamingCall;
import static io.grpc.stub.ClientCalls.asyncServerStreamingCall;
import static io.grpc.stub.ClientCalls.asyncUnaryCall;
import static io.grpc.stub.ClientCalls.blockingServerStreamingCall;
import static io.grpc.stub.ClientCalls.blockingUnaryCall;
import static io.grpc.stub.ClientCalls.futureUnaryCall;
import static io.grpc.stub.ServerCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ServerCalls.asyncClientStreamingCall;
import static io.grpc.stub.ServerCalls.asyncServerStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnaryCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedUnaryCall;

/**
 */
@javax.annotation.Generated(
    value = "by gRPC proto compiler (version 1.16.1)",
    comments = "Source: KardelenAPI.proto")
public final class CameraServiceGrpc {

  private CameraServiceGrpc() {}

  public static final String SERVICE_NAME = "kaapi.CameraService";

  // Static method descriptors that strictly reflect the proto.
  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> getGetPositionMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetPosition",
      requestType = com.google.protobuf.Empty.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> getGetPositionMethod() {
    io.grpc.MethodDescriptor<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> getGetPositionMethod;
    if ((getGetPositionMethod = CameraServiceGrpc.getGetPositionMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getGetPositionMethod = CameraServiceGrpc.getGetPositionMethod) == null) {
          CameraServiceGrpc.getGetPositionMethod = getGetPositionMethod = 
              io.grpc.MethodDescriptor.<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "GetPosition"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("GetPosition"))
                  .build();
          }
        }
     }
     return getGetPositionMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> getMoveAbsoluteMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "MoveAbsolute",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> getMoveAbsoluteMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> getMoveAbsoluteMethod;
    if ((getMoveAbsoluteMethod = CameraServiceGrpc.getMoveAbsoluteMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getMoveAbsoluteMethod = CameraServiceGrpc.getMoveAbsoluteMethod) == null) {
          CameraServiceGrpc.getMoveAbsoluteMethod = getMoveAbsoluteMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "MoveAbsolute"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("MoveAbsolute"))
                  .build();
          }
        }
     }
     return getMoveAbsoluteMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> getMoveRelativeMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "MoveRelative",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> getMoveRelativeMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> getMoveRelativeMethod;
    if ((getMoveRelativeMethod = CameraServiceGrpc.getMoveRelativeMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getMoveRelativeMethod = CameraServiceGrpc.getMoveRelativeMethod) == null) {
          CameraServiceGrpc.getMoveRelativeMethod = getMoveRelativeMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "MoveRelative"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("MoveRelative"))
                  .build();
          }
        }
     }
     return getMoveRelativeMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getGetCameraStatusMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetCameraStatus",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getGetCameraStatusMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getGetCameraStatusMethod;
    if ((getGetCameraStatusMethod = CameraServiceGrpc.getGetCameraStatusMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getGetCameraStatusMethod = CameraServiceGrpc.getGetCameraStatusMethod) == null) {
          CameraServiceGrpc.getGetCameraStatusMethod = getGetCameraStatusMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "GetCameraStatus"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("GetCameraStatus"))
                  .build();
          }
        }
     }
     return getGetCameraStatusMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getUpdateCameraStatusMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "UpdateCameraStatus",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getUpdateCameraStatusMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getUpdateCameraStatusMethod;
    if ((getUpdateCameraStatusMethod = CameraServiceGrpc.getUpdateCameraStatusMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getUpdateCameraStatusMethod = CameraServiceGrpc.getUpdateCameraStatusMethod) == null) {
          CameraServiceGrpc.getUpdateCameraStatusMethod = getUpdateCameraStatusMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "UpdateCameraStatus"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("UpdateCameraStatus"))
                  .build();
          }
        }
     }
     return getUpdateCameraStatusMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> getSetCameraMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetCamera",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> getSetCameraMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> getSetCameraMethod;
    if ((getSetCameraMethod = CameraServiceGrpc.getSetCameraMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getSetCameraMethod = CameraServiceGrpc.getSetCameraMethod) == null) {
          CameraServiceGrpc.getSetCameraMethod = getSetCameraMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "SetCamera"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("SetCamera"))
                  .build();
          }
        }
     }
     return getSetCameraMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> getGetNumericParameterMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetNumericParameter",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> getGetNumericParameterMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> getGetNumericParameterMethod;
    if ((getGetNumericParameterMethod = CameraServiceGrpc.getGetNumericParameterMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getGetNumericParameterMethod = CameraServiceGrpc.getGetNumericParameterMethod) == null) {
          CameraServiceGrpc.getGetNumericParameterMethod = getGetNumericParameterMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "GetNumericParameter"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("GetNumericParameter"))
                  .build();
          }
        }
     }
     return getGetNumericParameterMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> getGetEnumParameterMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetEnumParameter",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> getGetEnumParameterMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> getGetEnumParameterMethod;
    if ((getGetEnumParameterMethod = CameraServiceGrpc.getGetEnumParameterMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getGetEnumParameterMethod = CameraServiceGrpc.getGetEnumParameterMethod) == null) {
          CameraServiceGrpc.getGetEnumParameterMethod = getGetEnumParameterMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "GetEnumParameter"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("GetEnumParameter"))
                  .build();
          }
        }
     }
     return getGetEnumParameterMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> getSetNumericParameterMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetNumericParameter",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> getSetNumericParameterMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> getSetNumericParameterMethod;
    if ((getSetNumericParameterMethod = CameraServiceGrpc.getSetNumericParameterMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getSetNumericParameterMethod = CameraServiceGrpc.getSetNumericParameterMethod) == null) {
          CameraServiceGrpc.getSetNumericParameterMethod = getSetNumericParameterMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "SetNumericParameter"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("SetNumericParameter"))
                  .build();
          }
        }
     }
     return getSetNumericParameterMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> getSetEnumParameterMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetEnumParameter",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> getSetEnumParameterMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> getSetEnumParameterMethod;
    if ((getSetEnumParameterMethod = CameraServiceGrpc.getSetEnumParameterMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getSetEnumParameterMethod = CameraServiceGrpc.getSetEnumParameterMethod) == null) {
          CameraServiceGrpc.getSetEnumParameterMethod = getSetEnumParameterMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "SetEnumParameter"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("SetEnumParameter"))
                  .build();
          }
        }
     }
     return getSetEnumParameterMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> getSetEnumCommandMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetEnumCommand",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> getSetEnumCommandMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> getSetEnumCommandMethod;
    if ((getSetEnumCommandMethod = CameraServiceGrpc.getSetEnumCommandMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getSetEnumCommandMethod = CameraServiceGrpc.getSetEnumCommandMethod) == null) {
          CameraServiceGrpc.getSetEnumCommandMethod = getSetEnumCommandMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "SetEnumCommand"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("SetEnumCommand"))
                  .build();
          }
        }
     }
     return getSetEnumCommandMethod;
  }

  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> getGetVersionMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetVersion",
      requestType = com.google.protobuf.Empty.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> getGetVersionMethod() {
    io.grpc.MethodDescriptor<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> getGetVersionMethod;
    if ((getGetVersionMethod = CameraServiceGrpc.getGetVersionMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getGetVersionMethod = CameraServiceGrpc.getGetVersionMethod) == null) {
          CameraServiceGrpc.getGetVersionMethod = getGetVersionMethod = 
              io.grpc.MethodDescriptor.<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "GetVersion"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("GetVersion"))
                  .build();
          }
        }
     }
     return getGetVersionMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead> getCommunicationChannelMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "CommunicationChannel",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead.class,
      methodType = io.grpc.MethodDescriptor.MethodType.BIDI_STREAMING)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite,
      tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead> getCommunicationChannelMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead> getCommunicationChannelMethod;
    if ((getCommunicationChannelMethod = CameraServiceGrpc.getCommunicationChannelMethod) == null) {
      synchronized (CameraServiceGrpc.class) {
        if ((getCommunicationChannelMethod = CameraServiceGrpc.getCommunicationChannelMethod) == null) {
          CameraServiceGrpc.getCommunicationChannelMethod = getCommunicationChannelMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite, tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.BIDI_STREAMING)
              .setFullMethodName(generateFullMethodName(
                  "kaapi.CameraService", "CommunicationChannel"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead.getDefaultInstance()))
                  .setSchemaDescriptor(new CameraServiceMethodDescriptorSupplier("CommunicationChannel"))
                  .build();
          }
        }
     }
     return getCommunicationChannelMethod;
  }

  /**
   * Creates a new async stub that supports all call types for the service
   */
  public static CameraServiceStub newStub(io.grpc.Channel channel) {
    return new CameraServiceStub(channel);
  }

  /**
   * Creates a new blocking-style stub that supports unary and streaming output calls on the service
   */
  public static CameraServiceBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new CameraServiceBlockingStub(channel);
  }

  /**
   * Creates a new ListenableFuture-style stub that supports unary calls on the service
   */
  public static CameraServiceFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new CameraServiceFutureStub(channel);
  }

  /**
   */
  public static abstract class CameraServiceImplBase implements io.grpc.BindableService {

    /**
     */
    public void getPosition(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> responseObserver) {
      asyncUnimplementedUnaryCall(getGetPositionMethod(), responseObserver);
    }

    /**
     */
    public void moveAbsolute(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> responseObserver) {
      asyncUnimplementedUnaryCall(getMoveAbsoluteMethod(), responseObserver);
    }

    /**
     */
    public void moveRelative(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> responseObserver) {
      asyncUnimplementedUnaryCall(getMoveRelativeMethod(), responseObserver);
    }

    /**
     */
    public void getCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> responseObserver) {
      asyncUnimplementedUnaryCall(getGetCameraStatusMethod(), responseObserver);
    }

    /**
     */
    public void updateCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> responseObserver) {
      asyncUnimplementedUnaryCall(getUpdateCameraStatusMethod(), responseObserver);
    }

    /**
     */
    public void setCamera(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getSetCameraMethod(), responseObserver);
    }

    /**
     */
    public void getNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getGetNumericParameterMethod(), responseObserver);
    }

    /**
     */
    public void getEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getGetEnumParameterMethod(), responseObserver);
    }

    /**
     */
    public void setNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getSetNumericParameterMethod(), responseObserver);
    }

    /**
     */
    public void setEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getSetEnumParameterMethod(), responseObserver);
    }

    /**
     */
    public void setEnumCommand(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> responseObserver) {
      asyncUnimplementedUnaryCall(getSetEnumCommandMethod(), responseObserver);
    }

    /**
     */
    public void getVersion(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> responseObserver) {
      asyncUnimplementedUnaryCall(getGetVersionMethod(), responseObserver);
    }

    /**
     */
    public io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite> communicationChannel(
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead> responseObserver) {
      return asyncUnimplementedStreamingCall(getCommunicationChannelMethod(), responseObserver);
    }

    @java.lang.Override public final io.grpc.ServerServiceDefinition bindService() {
      return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor())
          .addMethod(
            getGetPositionMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.Empty,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo>(
                  this, METHODID_GET_POSITION)))
          .addMethod(
            getMoveAbsoluteMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult>(
                  this, METHODID_MOVE_ABSOLUTE)))
          .addMethod(
            getMoveRelativeMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult>(
                  this, METHODID_MOVE_RELATIVE)))
          .addMethod(
            getGetCameraStatusMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>(
                  this, METHODID_GET_CAMERA_STATUS)))
          .addMethod(
            getUpdateCameraStatusMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>(
                  this, METHODID_UPDATE_CAMERA_STATUS)))
          .addMethod(
            getSetCameraMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse>(
                  this, METHODID_SET_CAMERA)))
          .addMethod(
            getGetNumericParameterMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse>(
                  this, METHODID_GET_NUMERIC_PARAMETER)))
          .addMethod(
            getGetEnumParameterMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse>(
                  this, METHODID_GET_ENUM_PARAMETER)))
          .addMethod(
            getSetNumericParameterMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse>(
                  this, METHODID_SET_NUMERIC_PARAMETER)))
          .addMethod(
            getSetEnumParameterMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse>(
                  this, METHODID_SET_ENUM_PARAMETER)))
          .addMethod(
            getSetEnumCommandMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse>(
                  this, METHODID_SET_ENUM_COMMAND)))
          .addMethod(
            getGetVersionMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.Empty,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion>(
                  this, METHODID_GET_VERSION)))
          .addMethod(
            getCommunicationChannelMethod(),
            asyncBidiStreamingCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite,
                tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead>(
                  this, METHODID_COMMUNICATION_CHANNEL)))
          .build();
    }
  }

  /**
   */
  public static final class CameraServiceStub extends io.grpc.stub.AbstractStub<CameraServiceStub> {
    private CameraServiceStub(io.grpc.Channel channel) {
      super(channel);
    }

    private CameraServiceStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected CameraServiceStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new CameraServiceStub(channel, callOptions);
    }

    /**
     */
    public void getPosition(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetPositionMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void moveAbsolute(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getMoveAbsoluteMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void moveRelative(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getMoveRelativeMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetCameraStatusMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void updateCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getUpdateCameraStatusMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setCamera(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetCameraMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetNumericParameterMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetEnumParameterMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetNumericParameterMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetEnumParameterMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setEnumCommand(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetEnumCommandMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getVersion(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetVersionMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommWrite> communicationChannel(
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead> responseObserver) {
      return asyncBidiStreamingCall(
          getChannel().newCall(getCommunicationChannelMethod(), getCallOptions()), responseObserver);
    }
  }

  /**
   */
  public static final class CameraServiceBlockingStub extends io.grpc.stub.AbstractStub<CameraServiceBlockingStub> {
    private CameraServiceBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private CameraServiceBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected CameraServiceBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new CameraServiceBlockingStub(channel, callOptions);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo getPosition(com.google.protobuf.Empty request) {
      return blockingUnaryCall(
          getChannel(), getGetPositionMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult moveAbsolute(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters request) {
      return blockingUnaryCall(
          getChannel(), getMoveAbsoluteMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult moveRelative(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters request) {
      return blockingUnaryCall(
          getChannel(), getMoveRelativeMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus getCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters request) {
      return blockingUnaryCall(
          getChannel(), getGetCameraStatusMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus updateCameraStatus(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters request) {
      return blockingUnaryCall(
          getChannel(), getUpdateCameraStatusMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse setCamera(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest request) {
      return blockingUnaryCall(
          getChannel(), getSetCameraMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse getNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest request) {
      return blockingUnaryCall(
          getChannel(), getGetNumericParameterMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse getEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest request) {
      return blockingUnaryCall(
          getChannel(), getGetEnumParameterMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse setNumericParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest request) {
      return blockingUnaryCall(
          getChannel(), getSetNumericParameterMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse setEnumParameter(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest request) {
      return blockingUnaryCall(
          getChannel(), getSetEnumParameterMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse setEnumCommand(tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest request) {
      return blockingUnaryCall(
          getChannel(), getSetEnumCommandMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion getVersion(com.google.protobuf.Empty request) {
      return blockingUnaryCall(
          getChannel(), getGetVersionMethod(), getCallOptions(), request);
    }
  }

  /**
   */
  public static final class CameraServiceFutureStub extends io.grpc.stub.AbstractStub<CameraServiceFutureStub> {
    private CameraServiceFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private CameraServiceFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected CameraServiceFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new CameraServiceFutureStub(channel, callOptions);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo> getPosition(
        com.google.protobuf.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getGetPositionMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult> moveAbsolute(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters request) {
      return futureUnaryCall(
          getChannel().newCall(getMoveAbsoluteMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult> moveRelative(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters request) {
      return futureUnaryCall(
          getChannel().newCall(getMoveRelativeMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> getCameraStatus(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters request) {
      return futureUnaryCall(
          getChannel().newCall(getGetCameraStatusMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus> updateCameraStatus(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters request) {
      return futureUnaryCall(
          getChannel().newCall(getUpdateCameraStatusMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse> setCamera(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getSetCameraMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse> getNumericParameter(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getGetNumericParameterMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse> getEnumParameter(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getGetEnumParameterMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse> setNumericParameter(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getSetNumericParameterMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse> setEnumParameter(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getSetEnumParameterMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse> setEnumCommand(
        tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest request) {
      return futureUnaryCall(
          getChannel().newCall(getSetEnumCommandMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion> getVersion(
        com.google.protobuf.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getGetVersionMethod(), getCallOptions()), request);
    }
  }

  private static final int METHODID_GET_POSITION = 0;
  private static final int METHODID_MOVE_ABSOLUTE = 1;
  private static final int METHODID_MOVE_RELATIVE = 2;
  private static final int METHODID_GET_CAMERA_STATUS = 3;
  private static final int METHODID_UPDATE_CAMERA_STATUS = 4;
  private static final int METHODID_SET_CAMERA = 5;
  private static final int METHODID_GET_NUMERIC_PARAMETER = 6;
  private static final int METHODID_GET_ENUM_PARAMETER = 7;
  private static final int METHODID_SET_NUMERIC_PARAMETER = 8;
  private static final int METHODID_SET_ENUM_PARAMETER = 9;
  private static final int METHODID_SET_ENUM_COMMAND = 10;
  private static final int METHODID_GET_VERSION = 11;
  private static final int METHODID_COMMUNICATION_CHANNEL = 12;

  private static final class MethodHandlers<Req, Resp> implements
      io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
    private final CameraServiceImplBase serviceImpl;
    private final int methodId;

    MethodHandlers(CameraServiceImplBase serviceImpl, int methodId) {
      this.serviceImpl = serviceImpl;
      this.methodId = methodId;
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_GET_POSITION:
          serviceImpl.getPosition((com.google.protobuf.Empty) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.PosInfo>) responseObserver);
          break;
        case METHODID_MOVE_ABSOLUTE:
          serviceImpl.moveAbsolute((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveParameters) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.AbsoluteMoveResult>) responseObserver);
          break;
        case METHODID_MOVE_RELATIVE:
          serviceImpl.moveRelative((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveParameters) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.RelativeMoveResult>) responseObserver);
          break;
        case METHODID_GET_CAMERA_STATUS:
          serviceImpl.getCameraStatus((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetCameraStatusParameters) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>) responseObserver);
          break;
        case METHODID_UPDATE_CAMERA_STATUS:
          serviceImpl.updateCameraStatus((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.UpdateCameraStatusParameters) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CameraStatus>) responseObserver);
          break;
        case METHODID_SET_CAMERA:
          serviceImpl.setCamera((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetCameraResponse>) responseObserver);
          break;
        case METHODID_GET_NUMERIC_PARAMETER:
          serviceImpl.getNumericParameter((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetNumericParameterResponse>) responseObserver);
          break;
        case METHODID_GET_ENUM_PARAMETER:
          serviceImpl.getEnumParameter((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.GetEnumParameterResponse>) responseObserver);
          break;
        case METHODID_SET_NUMERIC_PARAMETER:
          serviceImpl.setNumericParameter((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetNumericParameterResponse>) responseObserver);
          break;
        case METHODID_SET_ENUM_PARAMETER:
          serviceImpl.setEnumParameter((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumParameterResponse>) responseObserver);
          break;
        case METHODID_SET_ENUM_COMMAND:
          serviceImpl.setEnumCommand((tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandRequest) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.SetEnumCommandResponse>) responseObserver);
          break;
        case METHODID_GET_VERSION:
          serviceImpl.getVersion((com.google.protobuf.Empty) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.ApiVersion>) responseObserver);
          break;
        default:
          throw new AssertionError();
      }
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public io.grpc.stub.StreamObserver<Req> invoke(
        io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_COMMUNICATION_CHANNEL:
          return (io.grpc.stub.StreamObserver<Req>) serviceImpl.communicationChannel(
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.CommRead>) responseObserver);
        default:
          throw new AssertionError();
      }
    }
  }

  private static abstract class CameraServiceBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoFileDescriptorSupplier, io.grpc.protobuf.ProtoServiceDescriptorSupplier {
    CameraServiceBaseDescriptorSupplier() {}

    @java.lang.Override
    public com.google.protobuf.Descriptors.FileDescriptor getFileDescriptor() {
      return tr.com.aselsan.kardelen.orion.message.definition.KardelenAPI.getDescriptor();
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.ServiceDescriptor getServiceDescriptor() {
      return getFileDescriptor().findServiceByName("CameraService");
    }
  }

  private static final class CameraServiceFileDescriptorSupplier
      extends CameraServiceBaseDescriptorSupplier {
    CameraServiceFileDescriptorSupplier() {}
  }

  private static final class CameraServiceMethodDescriptorSupplier
      extends CameraServiceBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoMethodDescriptorSupplier {
    private final String methodName;

    CameraServiceMethodDescriptorSupplier(String methodName) {
      this.methodName = methodName;
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.MethodDescriptor getMethodDescriptor() {
      return getServiceDescriptor().findMethodByName(methodName);
    }
  }

  private static volatile io.grpc.ServiceDescriptor serviceDescriptor;

  public static io.grpc.ServiceDescriptor getServiceDescriptor() {
    io.grpc.ServiceDescriptor result = serviceDescriptor;
    if (result == null) {
      synchronized (CameraServiceGrpc.class) {
        result = serviceDescriptor;
        if (result == null) {
          serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME)
              .setSchemaDescriptor(new CameraServiceFileDescriptorSupplier())
              .addMethod(getGetPositionMethod())
              .addMethod(getMoveAbsoluteMethod())
              .addMethod(getMoveRelativeMethod())
              .addMethod(getGetCameraStatusMethod())
              .addMethod(getUpdateCameraStatusMethod())
              .addMethod(getSetCameraMethod())
              .addMethod(getGetNumericParameterMethod())
              .addMethod(getGetEnumParameterMethod())
              .addMethod(getSetNumericParameterMethod())
              .addMethod(getSetEnumParameterMethod())
              .addMethod(getSetEnumCommandMethod())
              .addMethod(getGetVersionMethod())
              .addMethod(getCommunicationChannelMethod())
              .build();
        }
      }
    }
    return result;
  }
}
