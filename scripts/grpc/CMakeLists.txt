cmake_minimum_required(VERSION 2.8.12)
project(grpc_wrapper)

message(STATUS "GRPC ssl mode ${gRPC_SSL_PROVIDER}")
if (${gRPC_SSL_PROVIDER} STREQUAL "package")
	include(_build_openssl/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

add_subdirectory("grpc")
