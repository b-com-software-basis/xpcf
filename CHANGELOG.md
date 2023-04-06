### XPCF evolutions
- Update xpcf version to 2.6.3
- Configuration error occures by Exception in Factory::loadLibrary.

### XPCF remoting features
add new ```grpc``` interfaces in ```interfaces/xpcf/remoting``` to manage ```grpc``` components in ```xpcf```

#### xpcf\_grpc\_server application
The purpose is to implement a grpc server entry point which will handle componentized grpc service through ```xpcf/remoting/IGrpcService``` interface. The server address is manually configured in the code.

add document describing xpcf remoting architecture model

#### xpcf\_grpc\_gen tool
```xpcf_grpc_gen``` tool purpose is to parse xpcf interfaces to generate two components:a proxy and a server component. Each component is responsible to encrypt/decrypt types towards grpc types.

```xpcf_grpc_gen``` design consists in:

- parsing xpcf interfaces interpreting additional attributes using cppast (and under the hood libclang-parser)
- provide several backends to allow either protobuf, flatbuffers or other serialisation protocol (including intrusive protocols based on inheritance)

```xpcf_grpc_gen``` supports the following custom attributes:

- ```[[xpcf::in]]```, ```[[xpcf::out]]```, ```[[xpcf::inout]]```: attribute to set at the parameter level to disambiguate mixed or inout types that can't be deduced as in, out or inout
- ```[[grpc::server_streaming]]```, ```[[grpc::client_streaming]]``` or ```[[grpc::streaming]]```: attribute set at the method level to indicate the kind of streaming
- ```[[grpc::request("requestMessageName")]]``` and ```[[grpc::response("responseMessageName")]]```: to optionally set grpc request and response message name at the method level
	- **Note**: However, if you set the request and response across several methods, and use the same messages for several methods, you are responsible to ensure that message content are consistent

When no attribute is provided:

- request message content is built from input and input/output parameters
- response message content is built from output and input/output parameters

```xpcf_grpc_gen``` aims to support:

- analyze template container types: due to the restrictions of the various targeted IDL (no maps, or maps with only base type as keys, no vector of vector ...), use a dedicated set of regexp to find the signature semantic. Any other case will be tougher to serialize, and can in most cases need a refactor, and may come from a non effective design model.
- protobuf type conversion and message generation

```xpcf_grpc_gen``` provides:

- both template instantiation parsing mode:
	- cppast partial parsing with unexposed arguments
	- cppast full parsing with exposed arguments
- ```GRPCFlatBufferGenerator``` xpcf component
- ```GRPCProtoGenerator``` xpcf component
- ```IRPCGenerator``` interface binding toward ```GRPCProtoGenerator```

```xpcf_grpc_gen``` uses:

- cppast
- static xpcf build (provides a standalone application)
Note : grpc++ is still dynamically linked,the grpc++.pc file doesn't provide a solution to use the static version of grpc. Searching for a solution led to meson build system, it as the same limitation : the static keyword for a dependency declaration isn't supported with all backends.


