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
    comments = "Source: ptzp.proto")
public final class PTZServiceGrpc {

  private PTZServiceGrpc() {}

  public static final String SERVICE_NAME = "ptzp.PTZService";

  // Static method descriptors that strictly reflect the proto.
  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> getGetPTZPosInfoMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetPTZPosInfo",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> getGetPTZPosInfoMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> getGetPTZPosInfoMethod;
    if ((getGetPTZPosInfoMethod = PTZServiceGrpc.getGetPTZPosInfoMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getGetPTZPosInfoMethod = PTZServiceGrpc.getGetPTZPosInfoMethod) == null) {
          PTZServiceGrpc.getGetPTZPosInfoMethod = getGetPTZPosInfoMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "GetPTZPosInfo"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("GetPTZPosInfo"))
                  .build();
          }
        }
     }
     return getGetPTZPosInfoMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanLeftMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PanLeft",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanLeftMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanLeftMethod;
    if ((getPanLeftMethod = PTZServiceGrpc.getPanLeftMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPanLeftMethod = PTZServiceGrpc.getPanLeftMethod) == null) {
          PTZServiceGrpc.getPanLeftMethod = getPanLeftMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PanLeft"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PanLeft"))
                  .build();
          }
        }
     }
     return getPanLeftMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanRightMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PanRight",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanRightMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanRightMethod;
    if ((getPanRightMethod = PTZServiceGrpc.getPanRightMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPanRightMethod = PTZServiceGrpc.getPanRightMethod) == null) {
          PTZServiceGrpc.getPanRightMethod = getPanRightMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PanRight"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PanRight"))
                  .build();
          }
        }
     }
     return getPanRightMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PanStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanStopMethod;
    if ((getPanStopMethod = PTZServiceGrpc.getPanStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPanStopMethod = PTZServiceGrpc.getPanStopMethod) == null) {
          PTZServiceGrpc.getPanStopMethod = getPanStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PanStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PanStop"))
                  .build();
          }
        }
     }
     return getPanStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTiltAbsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PanTiltAbs",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTiltAbsMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTiltAbsMethod;
    if ((getPanTiltAbsMethod = PTZServiceGrpc.getPanTiltAbsMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPanTiltAbsMethod = PTZServiceGrpc.getPanTiltAbsMethod) == null) {
          PTZServiceGrpc.getPanTiltAbsMethod = getPanTiltAbsMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PanTiltAbs"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PanTiltAbs"))
                  .build();
          }
        }
     }
     return getPanTiltAbsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTilt2PosMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PanTilt2Pos",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTilt2PosMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPanTilt2PosMethod;
    if ((getPanTilt2PosMethod = PTZServiceGrpc.getPanTilt2PosMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPanTilt2PosMethod = PTZServiceGrpc.getPanTilt2PosMethod) == null) {
          PTZServiceGrpc.getPanTilt2PosMethod = getPanTilt2PosMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PanTilt2Pos"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PanTilt2Pos"))
                  .build();
          }
        }
     }
     return getPanTilt2PosMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltUpMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "TiltUp",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltUpMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltUpMethod;
    if ((getTiltUpMethod = PTZServiceGrpc.getTiltUpMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getTiltUpMethod = PTZServiceGrpc.getTiltUpMethod) == null) {
          PTZServiceGrpc.getTiltUpMethod = getTiltUpMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "TiltUp"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("TiltUp"))
                  .build();
          }
        }
     }
     return getTiltUpMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltDownMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "TiltDown",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltDownMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltDownMethod;
    if ((getTiltDownMethod = PTZServiceGrpc.getTiltDownMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getTiltDownMethod = PTZServiceGrpc.getTiltDownMethod) == null) {
          PTZServiceGrpc.getTiltDownMethod = getTiltDownMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "TiltDown"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("TiltDown"))
                  .build();
          }
        }
     }
     return getTiltDownMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "TiltStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getTiltStopMethod;
    if ((getTiltStopMethod = PTZServiceGrpc.getTiltStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getTiltStopMethod = PTZServiceGrpc.getTiltStopMethod) == null) {
          PTZServiceGrpc.getTiltStopMethod = getTiltStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "TiltStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("TiltStop"))
                  .build();
          }
        }
     }
     return getTiltStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomInMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "ZoomIn",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomInMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomInMethod;
    if ((getZoomInMethod = PTZServiceGrpc.getZoomInMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getZoomInMethod = PTZServiceGrpc.getZoomInMethod) == null) {
          PTZServiceGrpc.getZoomInMethod = getZoomInMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "ZoomIn"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("ZoomIn"))
                  .build();
          }
        }
     }
     return getZoomInMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomOutMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "ZoomOut",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomOutMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomOutMethod;
    if ((getZoomOutMethod = PTZServiceGrpc.getZoomOutMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getZoomOutMethod = PTZServiceGrpc.getZoomOutMethod) == null) {
          PTZServiceGrpc.getZoomOutMethod = getZoomOutMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "ZoomOut"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("ZoomOut"))
                  .build();
          }
        }
     }
     return getZoomOutMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "ZoomStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getZoomStopMethod;
    if ((getZoomStopMethod = PTZServiceGrpc.getZoomStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getZoomStopMethod = PTZServiceGrpc.getZoomStopMethod) == null) {
          PTZServiceGrpc.getZoomStopMethod = getZoomStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "ZoomStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("ZoomStop"))
                  .build();
          }
        }
     }
     return getZoomStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetGoMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PresetGo",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetGoMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetGoMethod;
    if ((getPresetGoMethod = PTZServiceGrpc.getPresetGoMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPresetGoMethod = PTZServiceGrpc.getPresetGoMethod) == null) {
          PTZServiceGrpc.getPresetGoMethod = getPresetGoMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PresetGo"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PresetGo"))
                  .build();
          }
        }
     }
     return getPresetGoMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetSaveMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PresetSave",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetSaveMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetSaveMethod;
    if ((getPresetSaveMethod = PTZServiceGrpc.getPresetSaveMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPresetSaveMethod = PTZServiceGrpc.getPresetSaveMethod) == null) {
          PTZServiceGrpc.getPresetSaveMethod = getPresetSaveMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PresetSave"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PresetSave"))
                  .build();
          }
        }
     }
     return getPresetSaveMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetDeleteMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PresetDelete",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetDeleteMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPresetDeleteMethod;
    if ((getPresetDeleteMethod = PTZServiceGrpc.getPresetDeleteMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPresetDeleteMethod = PTZServiceGrpc.getPresetDeleteMethod) == null) {
          PTZServiceGrpc.getPresetDeleteMethod = getPresetDeleteMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PresetDelete"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PresetDelete"))
                  .build();
          }
        }
     }
     return getPresetDeleteMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPresetGetListMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PresetGetList",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPresetGetListMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPresetGetListMethod;
    if ((getPresetGetListMethod = PTZServiceGrpc.getPresetGetListMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPresetGetListMethod = PTZServiceGrpc.getPresetGetListMethod) == null) {
          PTZServiceGrpc.getPresetGetListMethod = getPresetGetListMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PresetGetList"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PresetGetList"))
                  .build();
          }
        }
     }
     return getPresetGetListMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolSaveMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolSave",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolSaveMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolSaveMethod;
    if ((getPatrolSaveMethod = PTZServiceGrpc.getPatrolSaveMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolSaveMethod = PTZServiceGrpc.getPatrolSaveMethod) == null) {
          PTZServiceGrpc.getPatrolSaveMethod = getPatrolSaveMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolSave"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolSave"))
                  .build();
          }
        }
     }
     return getPatrolSaveMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolDeleteMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolDelete",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolDeleteMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolDeleteMethod;
    if ((getPatrolDeleteMethod = PTZServiceGrpc.getPatrolDeleteMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolDeleteMethod = PTZServiceGrpc.getPatrolDeleteMethod) == null) {
          PTZServiceGrpc.getPatrolDeleteMethod = getPatrolDeleteMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolDelete"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolDelete"))
                  .build();
          }
        }
     }
     return getPatrolDeleteMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolRunMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolRun",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolRunMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolRunMethod;
    if ((getPatrolRunMethod = PTZServiceGrpc.getPatrolRunMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolRunMethod = PTZServiceGrpc.getPatrolRunMethod) == null) {
          PTZServiceGrpc.getPatrolRunMethod = getPatrolRunMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolRun"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolRun"))
                  .build();
          }
        }
     }
     return getPatrolRunMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatrolStopMethod;
    if ((getPatrolStopMethod = PTZServiceGrpc.getPatrolStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolStopMethod = PTZServiceGrpc.getPatrolStopMethod) == null) {
          PTZServiceGrpc.getPatrolStopMethod = getPatrolStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolStop"))
                  .build();
          }
        }
     }
     return getPatrolStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatrolGetListMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolGetList",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatrolGetListMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatrolGetListMethod;
    if ((getPatrolGetListMethod = PTZServiceGrpc.getPatrolGetListMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolGetListMethod = PTZServiceGrpc.getPatrolGetListMethod) == null) {
          PTZServiceGrpc.getPatrolGetListMethod = getPatrolGetListMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolGetList"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolGetList"))
                  .build();
          }
        }
     }
     return getPatrolGetListMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> getPatrolGetDetailsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatrolGetDetails",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> getPatrolGetDetailsMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> getPatrolGetDetailsMethod;
    if ((getPatrolGetDetailsMethod = PTZServiceGrpc.getPatrolGetDetailsMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatrolGetDetailsMethod = PTZServiceGrpc.getPatrolGetDetailsMethod) == null) {
          PTZServiceGrpc.getPatrolGetDetailsMethod = getPatrolGetDetailsMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatrolGetDetails"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatrolGetDetails"))
                  .build();
          }
        }
     }
     return getPatrolGetDetailsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternRunMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternRun",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternRunMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternRunMethod;
    if ((getPatternRunMethod = PTZServiceGrpc.getPatternRunMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternRunMethod = PTZServiceGrpc.getPatternRunMethod) == null) {
          PTZServiceGrpc.getPatternRunMethod = getPatternRunMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternRun"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternRun"))
                  .build();
          }
        }
     }
     return getPatternRunMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopMethod;
    if ((getPatternStopMethod = PTZServiceGrpc.getPatternStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternStopMethod = PTZServiceGrpc.getPatternStopMethod) == null) {
          PTZServiceGrpc.getPatternStopMethod = getPatternStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternStop"))
                  .build();
          }
        }
     }
     return getPatternStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStartRecordingMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternStartRecording",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStartRecordingMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStartRecordingMethod;
    if ((getPatternStartRecordingMethod = PTZServiceGrpc.getPatternStartRecordingMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternStartRecordingMethod = PTZServiceGrpc.getPatternStartRecordingMethod) == null) {
          PTZServiceGrpc.getPatternStartRecordingMethod = getPatternStartRecordingMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternStartRecording"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternStartRecording"))
                  .build();
          }
        }
     }
     return getPatternStartRecordingMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopRecordingMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternStopRecording",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopRecordingMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternStopRecordingMethod;
    if ((getPatternStopRecordingMethod = PTZServiceGrpc.getPatternStopRecordingMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternStopRecordingMethod = PTZServiceGrpc.getPatternStopRecordingMethod) == null) {
          PTZServiceGrpc.getPatternStopRecordingMethod = getPatternStopRecordingMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternStopRecording"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternStopRecording"))
                  .build();
          }
        }
     }
     return getPatternStopRecordingMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternDeleteMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternDelete",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternDeleteMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getPatternDeleteMethod;
    if ((getPatternDeleteMethod = PTZServiceGrpc.getPatternDeleteMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternDeleteMethod = PTZServiceGrpc.getPatternDeleteMethod) == null) {
          PTZServiceGrpc.getPatternDeleteMethod = getPatternDeleteMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternDelete"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternDelete"))
                  .build();
          }
        }
     }
     return getPatternDeleteMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatternGetListMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "PatternGetList",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatternGetListMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> getPatternGetListMethod;
    if ((getPatternGetListMethod = PTZServiceGrpc.getPatternGetListMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getPatternGetListMethod = PTZServiceGrpc.getPatternGetListMethod) == null) {
          PTZServiceGrpc.getPatternGetListMethod = getPatternGetListMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "PatternGetList"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("PatternGetList"))
                  .build();
          }
        }
     }
     return getPatternGetListMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusInMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "FocusIn",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusInMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusInMethod;
    if ((getFocusInMethod = PTZServiceGrpc.getFocusInMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getFocusInMethod = PTZServiceGrpc.getFocusInMethod) == null) {
          PTZServiceGrpc.getFocusInMethod = getFocusInMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "FocusIn"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("FocusIn"))
                  .build();
          }
        }
     }
     return getFocusInMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusOutMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "FocusOut",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusOutMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusOutMethod;
    if ((getFocusOutMethod = PTZServiceGrpc.getFocusOutMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getFocusOutMethod = PTZServiceGrpc.getFocusOutMethod) == null) {
          PTZServiceGrpc.getFocusOutMethod = getFocusOutMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "FocusOut"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("FocusOut"))
                  .build();
          }
        }
     }
     return getFocusOutMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusStopMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "FocusStop",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusStopMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> getFocusStopMethod;
    if ((getFocusStopMethod = PTZServiceGrpc.getFocusStopMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getFocusStopMethod = PTZServiceGrpc.getFocusStopMethod) == null) {
          PTZServiceGrpc.getFocusStopMethod = getFocusStopMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "FocusStop"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("FocusStop"))
                  .build();
          }
        }
     }
     return getFocusStopMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getGetSettingsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetSettings",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getGetSettingsMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getGetSettingsMethod;
    if ((getGetSettingsMethod = PTZServiceGrpc.getGetSettingsMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getGetSettingsMethod = PTZServiceGrpc.getGetSettingsMethod) == null) {
          PTZServiceGrpc.getGetSettingsMethod = getGetSettingsMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "GetSettings"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("GetSettings"))
                  .build();
          }
        }
     }
     return getGetSettingsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getSetSettingsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetSettings",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getSetSettingsMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getSetSettingsMethod;
    if ((getSetSettingsMethod = PTZServiceGrpc.getSetSettingsMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getSetSettingsMethod = PTZServiceGrpc.getSetSettingsMethod) == null) {
          PTZServiceGrpc.getSetSettingsMethod = getSetSettingsMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "SetSettings"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("SetSettings"))
                  .build();
          }
        }
     }
     return getSetSettingsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> getGetHeadsMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetHeads",
      requestType = com.google.protobuf.Empty.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> getGetHeadsMethod() {
    io.grpc.MethodDescriptor<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> getGetHeadsMethod;
    if ((getGetHeadsMethod = PTZServiceGrpc.getGetHeadsMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getGetHeadsMethod = PTZServiceGrpc.getGetHeadsMethod) == null) {
          PTZServiceGrpc.getGetHeadsMethod = getGetHeadsMethod = 
              io.grpc.MethodDescriptor.<com.google.protobuf.Empty, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "GetHeads"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("GetHeads"))
                  .build();
          }
        }
     }
     return getGetHeadsMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getGetIOMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "GetIO",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getGetIOMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getGetIOMethod;
    if ((getGetIOMethod = PTZServiceGrpc.getGetIOMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getGetIOMethod = PTZServiceGrpc.getGetIOMethod) == null) {
          PTZServiceGrpc.getGetIOMethod = getGetIOMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "GetIO"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("GetIO"))
                  .build();
          }
        }
     }
     return getGetIOMethod;
  }

  private static volatile io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getSetIOMethod;

  @io.grpc.stub.annotations.RpcMethod(
      fullMethodName = SERVICE_NAME + '/' + "SetIO",
      requestType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.class,
      responseType = tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.class,
      methodType = io.grpc.MethodDescriptor.MethodType.UNARY)
  public static io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
      tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getSetIOMethod() {
    io.grpc.MethodDescriptor<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getSetIOMethod;
    if ((getSetIOMethod = PTZServiceGrpc.getSetIOMethod) == null) {
      synchronized (PTZServiceGrpc.class) {
        if ((getSetIOMethod = PTZServiceGrpc.getSetIOMethod) == null) {
          PTZServiceGrpc.getSetIOMethod = getSetIOMethod = 
              io.grpc.MethodDescriptor.<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar, tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "ptzp.PTZService", "SetIO"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar.getDefaultInstance()))
                  .setSchemaDescriptor(new PTZServiceMethodDescriptorSupplier("SetIO"))
                  .build();
          }
        }
     }
     return getSetIOMethod;
  }

  /**
   * Creates a new async stub that supports all call types for the service
   */
  public static PTZServiceStub newStub(io.grpc.Channel channel) {
    return new PTZServiceStub(channel);
  }

  /**
   * Creates a new blocking-style stub that supports unary and streaming output calls on the service
   */
  public static PTZServiceBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new PTZServiceBlockingStub(channel);
  }

  /**
   * Creates a new ListenableFuture-style stub that supports unary calls on the service
   */
  public static PTZServiceFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new PTZServiceFutureStub(channel);
  }

  /**
   */
  public static abstract class PTZServiceImplBase implements io.grpc.BindableService {

    /**
     */
    public void getPTZPosInfo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> responseObserver) {
      asyncUnimplementedUnaryCall(getGetPTZPosInfoMethod(), responseObserver);
    }

    /**
     */
    public void panLeft(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPanLeftMethod(), responseObserver);
    }

    /**
     */
    public void panRight(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPanRightMethod(), responseObserver);
    }

    /**
     */
    public void panStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPanStopMethod(), responseObserver);
    }

    /**
     */
    public void panTiltAbs(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPanTiltAbsMethod(), responseObserver);
    }

    /**
     */
    public void panTilt2Pos(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPanTilt2PosMethod(), responseObserver);
    }

    /**
     */
    public void tiltUp(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getTiltUpMethod(), responseObserver);
    }

    /**
     */
    public void tiltDown(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getTiltDownMethod(), responseObserver);
    }

    /**
     */
    public void tiltStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getTiltStopMethod(), responseObserver);
    }

    /**
     */
    public void zoomIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getZoomInMethod(), responseObserver);
    }

    /**
     */
    public void zoomOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getZoomOutMethod(), responseObserver);
    }

    /**
     */
    public void zoomStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getZoomStopMethod(), responseObserver);
    }

    /**
     */
    public void presetGo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPresetGoMethod(), responseObserver);
    }

    /**
     */
    public void presetSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPresetSaveMethod(), responseObserver);
    }

    /**
     */
    public void presetDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPresetDeleteMethod(), responseObserver);
    }

    /**
     */
    public void presetGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnimplementedUnaryCall(getPresetGetListMethod(), responseObserver);
    }

    /**
     */
    public void patrolSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolSaveMethod(), responseObserver);
    }

    /**
     */
    public void patrolDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolDeleteMethod(), responseObserver);
    }

    /**
     */
    public void patrolRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolRunMethod(), responseObserver);
    }

    /**
     */
    public void patrolStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolStopMethod(), responseObserver);
    }

    /**
     */
    public void patrolGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolGetListMethod(), responseObserver);
    }

    /**
     */
    public void patrolGetDetails(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> responseObserver) {
      asyncUnimplementedUnaryCall(getPatrolGetDetailsMethod(), responseObserver);
    }

    /**
     */
    public void patternRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternRunMethod(), responseObserver);
    }

    /**
     */
    public void patternStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternStopMethod(), responseObserver);
    }

    /**
     */
    public void patternStartRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternStartRecordingMethod(), responseObserver);
    }

    /**
     */
    public void patternStopRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternStopRecordingMethod(), responseObserver);
    }

    /**
     */
    public void patternDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternDeleteMethod(), responseObserver);
    }

    /**
     */
    public void patternGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnimplementedUnaryCall(getPatternGetListMethod(), responseObserver);
    }

    /**
     */
    public void focusIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getFocusInMethod(), responseObserver);
    }

    /**
     */
    public void focusOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getFocusOutMethod(), responseObserver);
    }

    /**
     */
    public void focusStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnimplementedUnaryCall(getFocusStopMethod(), responseObserver);
    }

    /**
     */
    public void getSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> responseObserver) {
      asyncUnimplementedUnaryCall(getGetSettingsMethod(), responseObserver);
    }

    /**
     */
    public void setSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> responseObserver) {
      asyncUnimplementedUnaryCall(getSetSettingsMethod(), responseObserver);
    }

    /**
     */
    public void getHeads(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> responseObserver) {
      asyncUnimplementedUnaryCall(getGetHeadsMethod(), responseObserver);
    }

    /**
     */
    public void getIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> responseObserver) {
      asyncUnimplementedUnaryCall(getGetIOMethod(), responseObserver);
    }

    /**
     */
    public void setIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> responseObserver) {
      asyncUnimplementedUnaryCall(getSetIOMethod(), responseObserver);
    }

    @java.lang.Override public final io.grpc.ServerServiceDefinition bindService() {
      return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor())
          .addMethod(
            getGetPTZPosInfoMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo>(
                  this, METHODID_GET_PTZPOS_INFO)))
          .addMethod(
            getPanLeftMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PAN_LEFT)))
          .addMethod(
            getPanRightMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PAN_RIGHT)))
          .addMethod(
            getPanStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PAN_STOP)))
          .addMethod(
            getPanTiltAbsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PAN_TILT_ABS)))
          .addMethod(
            getPanTilt2PosMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PAN_TILT2POS)))
          .addMethod(
            getTiltUpMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_TILT_UP)))
          .addMethod(
            getTiltDownMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_TILT_DOWN)))
          .addMethod(
            getTiltStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_TILT_STOP)))
          .addMethod(
            getZoomInMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_ZOOM_IN)))
          .addMethod(
            getZoomOutMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_ZOOM_OUT)))
          .addMethod(
            getZoomStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_ZOOM_STOP)))
          .addMethod(
            getPresetGoMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PRESET_GO)))
          .addMethod(
            getPresetSaveMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PRESET_SAVE)))
          .addMethod(
            getPresetDeleteMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PRESET_DELETE)))
          .addMethod(
            getPresetGetListMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>(
                  this, METHODID_PRESET_GET_LIST)))
          .addMethod(
            getPatrolSaveMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATROL_SAVE)))
          .addMethod(
            getPatrolDeleteMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATROL_DELETE)))
          .addMethod(
            getPatrolRunMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATROL_RUN)))
          .addMethod(
            getPatrolStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATROL_STOP)))
          .addMethod(
            getPatrolGetListMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>(
                  this, METHODID_PATROL_GET_LIST)))
          .addMethod(
            getPatrolGetDetailsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition>(
                  this, METHODID_PATROL_GET_DETAILS)))
          .addMethod(
            getPatternRunMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATTERN_RUN)))
          .addMethod(
            getPatternStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATTERN_STOP)))
          .addMethod(
            getPatternStartRecordingMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATTERN_START_RECORDING)))
          .addMethod(
            getPatternStopRecordingMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATTERN_STOP_RECORDING)))
          .addMethod(
            getPatternDeleteMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_PATTERN_DELETE)))
          .addMethod(
            getPatternGetListMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>(
                  this, METHODID_PATTERN_GET_LIST)))
          .addMethod(
            getFocusInMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_FOCUS_IN)))
          .addMethod(
            getFocusOutMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_FOCUS_OUT)))
          .addMethod(
            getFocusStopMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>(
                  this, METHODID_FOCUS_STOP)))
          .addMethod(
            getGetSettingsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>(
                  this, METHODID_GET_SETTINGS)))
          .addMethod(
            getSetSettingsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>(
                  this, METHODID_SET_SETTINGS)))
          .addMethod(
            getGetHeadsMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.Empty,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo>(
                  this, METHODID_GET_HEADS)))
          .addMethod(
            getGetIOMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>(
                  this, METHODID_GET_IO)))
          .addMethod(
            getSetIOMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar,
                tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>(
                  this, METHODID_SET_IO)))
          .build();
    }
  }

  /**
   */
  public static final class PTZServiceStub extends io.grpc.stub.AbstractStub<PTZServiceStub> {
    private PTZServiceStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PTZServiceStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PTZServiceStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PTZServiceStub(channel, callOptions);
    }

    /**
     */
    public void getPTZPosInfo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetPTZPosInfoMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void panLeft(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPanLeftMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void panRight(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPanRightMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void panStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPanStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void panTiltAbs(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPanTiltAbsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void panTilt2Pos(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPanTilt2PosMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void tiltUp(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getTiltUpMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void tiltDown(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getTiltDownMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void tiltStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getTiltStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void zoomIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getZoomInMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void zoomOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getZoomOutMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void zoomStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getZoomStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void presetGo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPresetGoMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void presetSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPresetSaveMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void presetDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPresetDeleteMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void presetGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPresetGetListMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolSaveMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolDeleteMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolRunMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolGetListMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patrolGetDetails(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatrolGetDetailsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternRunMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternStartRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternStartRecordingMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternStopRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternStopRecordingMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternDeleteMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void patternGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPatternGetListMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void focusIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getFocusInMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void focusOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getFocusOutMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void focusStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getFocusStopMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetSettingsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetSettingsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getHeads(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetHeadsMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void getIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getGetIOMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void setIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request,
        io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getSetIOMethod(), getCallOptions()), request, responseObserver);
    }
  }

  /**
   */
  public static final class PTZServiceBlockingStub extends io.grpc.stub.AbstractStub<PTZServiceBlockingStub> {
    private PTZServiceBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PTZServiceBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PTZServiceBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PTZServiceBlockingStub(channel, callOptions);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo getPTZPosInfo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getGetPTZPosInfoMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult panLeft(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getPanLeftMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult panRight(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getPanRightMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult panStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getPanStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult panTiltAbs(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getPanTiltAbsMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult panTilt2Pos(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getPanTilt2PosMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult tiltUp(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getTiltUpMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult tiltDown(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getTiltDownMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult tiltStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getTiltStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult zoomIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getZoomInMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult zoomOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getZoomOutMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult zoomStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getZoomStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult presetGo(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return blockingUnaryCall(
          getChannel(), getPresetGoMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult presetSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return blockingUnaryCall(
          getChannel(), getPresetSaveMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult presetDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return blockingUnaryCall(
          getChannel(), getPresetDeleteMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList presetGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return blockingUnaryCall(
          getChannel(), getPresetGetListMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patrolSave(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolSaveMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patrolDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolDeleteMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patrolRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolRunMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patrolStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList patrolGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolGetListMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition patrolGetDetails(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatrolGetDetailsMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patternRun(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternRunMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patternStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patternStartRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternStartRecordingMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patternStopRecording(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternStopRecordingMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult patternDelete(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternDeleteMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList patternGetList(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return blockingUnaryCall(
          getChannel(), getPatternGetListMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult focusIn(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getFocusInMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult focusOut(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getFocusOutMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult focusStop(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getFocusStopMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings getSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request) {
      return blockingUnaryCall(
          getChannel(), getGetSettingsMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings setSettings(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request) {
      return blockingUnaryCall(
          getChannel(), getSetSettingsMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo getHeads(com.google.protobuf.Empty request) {
      return blockingUnaryCall(
          getChannel(), getGetHeadsMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar getIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getGetIOMethod(), getCallOptions(), request);
    }

    /**
     */
    public tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar setIO(tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request) {
      return blockingUnaryCall(
          getChannel(), getSetIOMethod(), getCallOptions(), request);
    }
  }

  /**
   */
  public static final class PTZServiceFutureStub extends io.grpc.stub.AbstractStub<PTZServiceFutureStub> {
    private PTZServiceFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PTZServiceFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PTZServiceFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PTZServiceFutureStub(channel, callOptions);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo> getPTZPosInfo(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getGetPTZPosInfoMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> panLeft(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getPanLeftMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> panRight(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getPanRightMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> panStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getPanStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> panTiltAbs(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getPanTiltAbsMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> panTilt2Pos(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getPanTilt2PosMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> tiltUp(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getTiltUpMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> tiltDown(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getTiltDownMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> tiltStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getTiltStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> zoomIn(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getZoomInMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> zoomOut(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getZoomOutMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> zoomStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getZoomStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> presetGo(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPresetGoMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> presetSave(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPresetSaveMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> presetDelete(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPresetDeleteMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> presetGetList(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPresetGetListMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patrolSave(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolSaveMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patrolDelete(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolDeleteMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patrolRun(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolRunMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patrolStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> patrolGetList(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolGetListMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition> patrolGetDetails(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatrolGetDetailsMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patternRun(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternRunMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patternStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patternStartRecording(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternStartRecordingMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patternStopRecording(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternStopRecordingMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> patternDelete(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternDeleteMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList> patternGetList(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd request) {
      return futureUnaryCall(
          getChannel().newCall(getPatternGetListMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> focusIn(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getFocusInMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> focusOut(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getFocusOutMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult> focusStop(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getFocusStopMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> getSettings(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request) {
      return futureUnaryCall(
          getChannel().newCall(getGetSettingsMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings> setSettings(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings request) {
      return futureUnaryCall(
          getChannel().newCall(getSetSettingsMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo> getHeads(
        com.google.protobuf.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getGetHeadsMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> getIO(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getGetIOMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar> setIO(
        tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar request) {
      return futureUnaryCall(
          getChannel().newCall(getSetIOMethod(), getCallOptions()), request);
    }
  }

  private static final int METHODID_GET_PTZPOS_INFO = 0;
  private static final int METHODID_PAN_LEFT = 1;
  private static final int METHODID_PAN_RIGHT = 2;
  private static final int METHODID_PAN_STOP = 3;
  private static final int METHODID_PAN_TILT_ABS = 4;
  private static final int METHODID_PAN_TILT2POS = 5;
  private static final int METHODID_TILT_UP = 6;
  private static final int METHODID_TILT_DOWN = 7;
  private static final int METHODID_TILT_STOP = 8;
  private static final int METHODID_ZOOM_IN = 9;
  private static final int METHODID_ZOOM_OUT = 10;
  private static final int METHODID_ZOOM_STOP = 11;
  private static final int METHODID_PRESET_GO = 12;
  private static final int METHODID_PRESET_SAVE = 13;
  private static final int METHODID_PRESET_DELETE = 14;
  private static final int METHODID_PRESET_GET_LIST = 15;
  private static final int METHODID_PATROL_SAVE = 16;
  private static final int METHODID_PATROL_DELETE = 17;
  private static final int METHODID_PATROL_RUN = 18;
  private static final int METHODID_PATROL_STOP = 19;
  private static final int METHODID_PATROL_GET_LIST = 20;
  private static final int METHODID_PATROL_GET_DETAILS = 21;
  private static final int METHODID_PATTERN_RUN = 22;
  private static final int METHODID_PATTERN_STOP = 23;
  private static final int METHODID_PATTERN_START_RECORDING = 24;
  private static final int METHODID_PATTERN_STOP_RECORDING = 25;
  private static final int METHODID_PATTERN_DELETE = 26;
  private static final int METHODID_PATTERN_GET_LIST = 27;
  private static final int METHODID_FOCUS_IN = 28;
  private static final int METHODID_FOCUS_OUT = 29;
  private static final int METHODID_FOCUS_STOP = 30;
  private static final int METHODID_GET_SETTINGS = 31;
  private static final int METHODID_SET_SETTINGS = 32;
  private static final int METHODID_GET_HEADS = 33;
  private static final int METHODID_GET_IO = 34;
  private static final int METHODID_SET_IO = 35;

  private static final class MethodHandlers<Req, Resp> implements
      io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
    private final PTZServiceImplBase serviceImpl;
    private final int methodId;

    MethodHandlers(PTZServiceImplBase serviceImpl, int methodId) {
      this.serviceImpl = serviceImpl;
      this.methodId = methodId;
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_GET_PTZPOS_INFO:
          serviceImpl.getPTZPosInfo((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PTZPosInfo>) responseObserver);
          break;
        case METHODID_PAN_LEFT:
          serviceImpl.panLeft((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PAN_RIGHT:
          serviceImpl.panRight((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PAN_STOP:
          serviceImpl.panStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PAN_TILT_ABS:
          serviceImpl.panTiltAbs((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PAN_TILT2POS:
          serviceImpl.panTilt2Pos((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_TILT_UP:
          serviceImpl.tiltUp((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_TILT_DOWN:
          serviceImpl.tiltDown((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_TILT_STOP:
          serviceImpl.tiltStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_ZOOM_IN:
          serviceImpl.zoomIn((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_ZOOM_OUT:
          serviceImpl.zoomOut((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_ZOOM_STOP:
          serviceImpl.zoomStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PRESET_GO:
          serviceImpl.presetGo((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PRESET_SAVE:
          serviceImpl.presetSave((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PRESET_DELETE:
          serviceImpl.presetDelete((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PRESET_GET_LIST:
          serviceImpl.presetGetList((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>) responseObserver);
          break;
        case METHODID_PATROL_SAVE:
          serviceImpl.patrolSave((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATROL_DELETE:
          serviceImpl.patrolDelete((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATROL_RUN:
          serviceImpl.patrolRun((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATROL_STOP:
          serviceImpl.patrolStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATROL_GET_LIST:
          serviceImpl.patrolGetList((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>) responseObserver);
          break;
        case METHODID_PATROL_GET_DETAILS:
          serviceImpl.patrolGetDetails((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatrolDefinition>) responseObserver);
          break;
        case METHODID_PATTERN_RUN:
          serviceImpl.patternRun((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATTERN_STOP:
          serviceImpl.patternStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATTERN_START_RECORDING:
          serviceImpl.patternStartRecording((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATTERN_STOP_RECORDING:
          serviceImpl.patternStopRecording((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATTERN_DELETE:
          serviceImpl.patternDelete((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_PATTERN_GET_LIST:
          serviceImpl.patternGetList((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PatternCmd) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PresetList>) responseObserver);
          break;
        case METHODID_FOCUS_IN:
          serviceImpl.focusIn((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_FOCUS_OUT:
          serviceImpl.focusOut((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_FOCUS_STOP:
          serviceImpl.focusStop((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzCommandResult>) responseObserver);
          break;
        case METHODID_GET_SETTINGS:
          serviceImpl.getSettings((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>) responseObserver);
          break;
        case METHODID_SET_SETTINGS:
          serviceImpl.setSettings((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.Settings>) responseObserver);
          break;
        case METHODID_GET_HEADS:
          serviceImpl.getHeads((com.google.protobuf.Empty) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.PtzHeadInfo>) responseObserver);
          break;
        case METHODID_GET_IO:
          serviceImpl.getIO((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>) responseObserver);
          break;
        case METHODID_SET_IO:
          serviceImpl.setIO((tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar) request,
              (io.grpc.stub.StreamObserver<tr.com.aselsan.kardelen.orion.message.definition.Ptzp.IOCmdPar>) responseObserver);
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
        default:
          throw new AssertionError();
      }
    }
  }

  private static abstract class PTZServiceBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoFileDescriptorSupplier, io.grpc.protobuf.ProtoServiceDescriptorSupplier {
    PTZServiceBaseDescriptorSupplier() {}

    @java.lang.Override
    public com.google.protobuf.Descriptors.FileDescriptor getFileDescriptor() {
      return tr.com.aselsan.kardelen.orion.message.definition.Ptzp.getDescriptor();
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.ServiceDescriptor getServiceDescriptor() {
      return getFileDescriptor().findServiceByName("PTZService");
    }
  }

  private static final class PTZServiceFileDescriptorSupplier
      extends PTZServiceBaseDescriptorSupplier {
    PTZServiceFileDescriptorSupplier() {}
  }

  private static final class PTZServiceMethodDescriptorSupplier
      extends PTZServiceBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoMethodDescriptorSupplier {
    private final String methodName;

    PTZServiceMethodDescriptorSupplier(String methodName) {
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
      synchronized (PTZServiceGrpc.class) {
        result = serviceDescriptor;
        if (result == null) {
          serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME)
              .setSchemaDescriptor(new PTZServiceFileDescriptorSupplier())
              .addMethod(getGetPTZPosInfoMethod())
              .addMethod(getPanLeftMethod())
              .addMethod(getPanRightMethod())
              .addMethod(getPanStopMethod())
              .addMethod(getPanTiltAbsMethod())
              .addMethod(getPanTilt2PosMethod())
              .addMethod(getTiltUpMethod())
              .addMethod(getTiltDownMethod())
              .addMethod(getTiltStopMethod())
              .addMethod(getZoomInMethod())
              .addMethod(getZoomOutMethod())
              .addMethod(getZoomStopMethod())
              .addMethod(getPresetGoMethod())
              .addMethod(getPresetSaveMethod())
              .addMethod(getPresetDeleteMethod())
              .addMethod(getPresetGetListMethod())
              .addMethod(getPatrolSaveMethod())
              .addMethod(getPatrolDeleteMethod())
              .addMethod(getPatrolRunMethod())
              .addMethod(getPatrolStopMethod())
              .addMethod(getPatrolGetListMethod())
              .addMethod(getPatrolGetDetailsMethod())
              .addMethod(getPatternRunMethod())
              .addMethod(getPatternStopMethod())
              .addMethod(getPatternStartRecordingMethod())
              .addMethod(getPatternStopRecordingMethod())
              .addMethod(getPatternDeleteMethod())
              .addMethod(getPatternGetListMethod())
              .addMethod(getFocusInMethod())
              .addMethod(getFocusOutMethod())
              .addMethod(getFocusStopMethod())
              .addMethod(getGetSettingsMethod())
              .addMethod(getSetSettingsMethod())
              .addMethod(getGetHeadsMethod())
              .addMethod(getGetIOMethod())
              .addMethod(getSetIOMethod())
              .build();
        }
      }
    }
    return result;
  }
}
