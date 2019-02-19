all:
	@echo please start config or camback
OrionCommunication:
	@echo ok
	@protoc --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin OrionCommunication.proto
	@protoc --cpp_out=. OrionCommunication.proto
AlgorithmCommunication:
	@echo ok
	@protoc --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin AlgorithmCommunication.proto
	@protoc --cpp_out=. AlgorithmCommunication.proto
