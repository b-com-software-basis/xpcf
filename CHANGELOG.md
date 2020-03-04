### XPCF evolutions
- Update xpcf version to 2.3.2
- Update builddefs-qmake to v4.0.0: provide compilation defined flags to enable/disable dependencies, replace BCOMDEVROOT with REMAKEN_PKG_ROOT

#### XPCF improvements:

- add ```SWIG``` interface files
- update inheritance declaration for ```cppast``` syntax (```public virtual``` is not supported by libclang-parser hence replaced with ```virtual public``` instead)
- ```xpcf-std``` target: add ```xpcf-std.pro``` project file to build xpcf based on c++17 std features (shared_ptr, any .. are used from std instead of boost, hence xpcf headers are fully c++ compliant with no need for boost includes. Moreover, boost dependency is statically linked, and boost is not exposed ```in xpcf-std``` interfaces).
- ```PropertyManager```: allows to try to get the config path, but don't fail when the component has been registered to the ComponentManager with bindLocal
- ```xpcf.pro``` and ```xpcf-std.pro```: 
	- add ```XPCF_WITH_LOGS``` define to enable logs from ```xpcf``` creation, configuration ...
	- set ```QMAKE_MACOSX_DEPLOYMENT_TARGET``` to version ```10.14``` as minimum target version : Apple clang doesn't support  std::any before Mojave

