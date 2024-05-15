/*
 * Copyright (C) 2024  Jimmy Aguilar Mena
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grpcpp/grpcpp.h>

#include "prototest.grpc.pb.h"

#include <atomic>
#include <iostream>

class protoClient {
public:

	protoClient(std::shared_ptr<grpc::Channel> channel)
      : _stub(testService::NewStub(channel))
	{}

	int testService_async(int val1, int val2)
	{
		// Data we are sending to the server.
		testRequest request;
		request.set_value1(val1);
		request.set_value2(val2);

		// Container for the data we expect from the server.
		testResponse response;

		// Context for the client. It could be used to convey extra information
		// to the server and/or tweak certain RPC behaviors.
		grpc::ClientContext context;

		// The actual RPC.
		std::atomic<bool> done = false;
		grpc::Status status;
		_stub->async()->testFunction(
			&context, &request, &response,
			[&done, &status](grpc::Status s) {
				status = std::move(s);
				done.store(true);
				done.notify_one();
			});

		done.wait(false);

		// Act upon its status.
		if (!status.ok()) {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
			throw std::runtime_error("Error calling grpc service.");
		}

		return response.result();
	}

	int testService_sync(int val1, int val2)
	{
		// Data we are sending to the server.
		testRequest request;
		request.set_value1(val1);
		request.set_value2(val2);

		// Container for the data we expect from the server.
		testResponse response;

		// Context for the client. It could be used to convey extra information
		// to the server and/or tweak certain RPC behaviors.
		grpc::ClientContext context;

		grpc::Status status = _stub->testFunction(&context, request, &response);

		if (!status.ok()) {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
			throw std::runtime_error("Error calling grpc service.");
		}

		return response.result();
	}

private:
	std::unique_ptr<testService::Stub> _stub;
};

int main(int argc, char** argv) {

	int val1 = std::atoi(argv[1]);
	int val2 = std::atoi(argv[2]);

	protoClient client(
		grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())
	);

	int reply1 = client.testService_sync(val1, val2);
	std::cout << "Sum sync received: " << reply1 << std::endl;

	int reply2 = client.testService_async(val1, val2);
	std::cout << "Sum async received: " << reply2 << std::endl;

  return 0;
}
