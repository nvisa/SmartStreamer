#include "diagnosticserviceimpl.h"

#include <lmm/baselmmelement.h>
#include <lmm/baselmmpipeline.h>

DiagnosticServiceImpl::DiagnosticServiceImpl(BaseStreamer *streamer)
{
	serviceStreamer = streamer;
}

grpc::Status DiagnosticServiceImpl::GetDiagnosticStatus(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, AlgorithmCommunication::DiagnosticInfo *response)
{
	(void)context;
	(void)request;

	if (serviceStreamer->getPipelineCount() > 0) {
		BaseLmmPipeline *p = serviceStreamer->getPipeline(0);
		for (int i = 0; i < p->getPipeCount(); i++) {
			BaseLmmElement *pipe = p->getPipe(i);

			AlgorithmCommunication::Element *e = new AlgorithmCommunication::Element();
			e->set_inputqueuecount(pipe->getInputQueueCount());
			e->set_outputqueuecount(pipe->getOutputQueueCount());

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

				auto nodeQueue = response->add_nodes();
				nodeQueue->set_type(AlgorithmCommunication::Node_NodeType_QUEUE);
				nodeQueue->set_allocated_queue(q);
			}
		}
	}

	return grpc::Status::OK;
}

grpc::Status DiagnosticServiceImpl::GetDiagnosticStatusStream(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, ::grpc::ServerWriter<AlgorithmCommunication::DiagnosticInfo> *writer)
{
	(void)context;
	(void)request;

	while (true) {
		AlgorithmCommunication::DiagnosticInfo response;

		if (serviceStreamer->getPipelineCount() > 0) {
			BaseLmmPipeline *p = serviceStreamer->getPipeline(0);
			for (int i = 0; i < p->getPipeCount(); i++) {
				BaseLmmElement *pipe = p->getPipe(i);

				AlgorithmCommunication::Element *e = new AlgorithmCommunication::Element();
				e->set_inputqueuecount(pipe->getInputQueueCount());
				e->set_outputqueuecount(pipe->getOutputQueueCount());

				auto nodeElement = response.add_nodes();
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

					auto nodeQueue = response.add_nodes();
					nodeQueue->set_type(AlgorithmCommunication::Node_NodeType_QUEUE);
					nodeQueue->set_allocated_queue(q);
				}
			}
		}

		if (!writer->Write(response))
			break;
		else
			QThread::msleep(500);
	}

	return grpc::Status::OK;
}
