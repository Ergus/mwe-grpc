# Copyright (C) 2024  Jimmy Aguilar Mena

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.5)

project(MWEGRPC LANGUAGES CXX)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Generate C++ lib
add_library(ProtoTest prototest.proto)
target_link_libraries(ProtoTest PUBLIC gRPC::grpc++)
protobuf_generate(TARGET ProtoTest)

protobuf_generate(
    TARGET ProtoTest
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
    PLUGIN_OPTIONS generate_mock_code=true
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)

# Build C++ server
add_executable(protoserver protoserver.cpp)
target_link_libraries(protoserver ProtoTest)

# Build C++ client
add_executable(protoclient protoclient.cpp)
target_link_libraries(protoclient ProtoTest)
set_property(TARGET protoclient PROPERTY CXX_STANDARD 20) # Needed for atomic wait

