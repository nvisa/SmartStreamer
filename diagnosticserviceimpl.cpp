#include "diagnosticserviceimpl.h"

#include <lmm/baselmmelement.h>
#include <lmm/baselmmpipeline.h>

static AlgorithmCommunication::DiagnosticInfo* createResponse(BaseStreamer *streamer);

DiagnosticServiceImpl::DiagnosticServiceImpl(BaseStreamer *streamer)
{
	serviceStreamer = streamer;
}

grpc::Status DiagnosticServiceImpl::GetDiagnosticStatus(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, AlgorithmCommunication::DiagnosticInfo *response)
{
	(void)context;
	(void)request;

	AlgorithmCommunication::DiagnosticInfo *resp = createResponse(serviceStreamer);
	*response = *resp;

	return grpc::Status::OK;
}

grpc::Status DiagnosticServiceImpl::GetDiagnosticStatusStream(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, ::grpc::ServerWriter<AlgorithmCommunication::DiagnosticInfo> *writer)
{
	(void)context;
	(void)request;

	while (true) {
		AlgorithmCommunication::DiagnosticInfo *resp = createResponse(serviceStreamer);

		if (!writer->Write(*resp))
			break;
		else
			QThread::msleep(500);
	}

	return grpc::Status::OK;
}

static AlgorithmCommunication::DiagnosticInfo* createResponse(BaseStreamer *streamer)
{
	AlgorithmCommunication::DiagnosticInfo *response = new AlgorithmCommunication::DiagnosticInfo();

	if (streamer->getPipelineCount() > 0) {
		BaseLmmPipeline *p = streamer->getPipeline(0);
		response->set_latency(p->getLatency());
		// TODO: Api çağrıldığında kitleniyor
		//response->set_totalusememory(p->getTotalMemoryUsage());

		for (int i = 0; i < p->getPipeCount(); i++) {
			BaseLmmElement *pipe = p->getPipe(i);

			AlgorithmCommunication::Element *e = new AlgorithmCommunication::Element();
			e->set_inputqueuecount(pipe->getInputQueueCount());
			e->set_outputqueuecount(pipe->getOutputQueueCount());
			e->set_name(pipe->objectName().toStdString());

			auto nodeElement = response->add_nodes();
			nodeElement->set_type(AlgorithmCommunication::Node_NodeType_ELEMENT);
			nodeElement->set_allocated_element(e);

			for (int j = 0; j < pipe->getOutputQueueCount(); j++) {
				ElementIOQueue *eq = pipe->getOutputQueue(j);

				AlgorithmCommunication::ElementQueue *q = new AlgorithmCommunication::ElementQueue();
				q->set_bitrate(eq->getBitrate());
				q->set_buffercount(eq->getBufferCount());
				q->set_elapsedtime(eq->getElapsedSinceLastAdd());
				q->set_fps(eq->getFps());
				q->set_receivedcount(eq->getReceivedCount());
				q->set_sendcount(eq->getSentCount());
				q->set_totalsize(eq->getTotalSize());

				auto nodeQueue = response->add_nodes();
				nodeQueue->set_type(AlgorithmCommunication::Node_NodeType_QUEUE);
				nodeQueue->set_allocated_queue(q);
			}
		}
	} else {
		return NULL;
	}

	return response;
}
