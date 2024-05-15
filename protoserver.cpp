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

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "prototest.grpc.pb.h"
#include <iostream>

class testServiceImpl final : public testService::Service {

	grpc::Status testFunction(
		grpc::ServerContext* context,
		const testRequest* request,
		testResponse* response
	) override {
		int value1 = request->value1();
		int value2 = request->value2();

		std::cerr << "Service received: " << value1 << " and " << value2 << std::endl;

		response->set_result(value1 + value2);

		return grpc::Status::OK;
	}
};


int main(int argc, char *argv[])
{
	std::string server_address("0.0.0.0:50051");
	testServiceImpl service;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	server->Wait();


    return 0;
}
