### XPCF evolutions
- Update xpcf version to 2.4.0
- Update builddefs/qmake to v4.6.1

#### XPCF improvements:
- properties: the ability to provide specific properties set in injection configuration is handled. Properties can be specified with the properties attribute in \<bind ... /\> or \<component ... /\ nodes. 

- multibind: add multibind support. This feature provides the ability to bind an interface to several implementations instances (for instance, to declare a set of services implementing the same IRemote interface ...). Note that instances declared with the "Singleton" scope are shared among all binds.

For both improvements, refer to [xpcf-registry-sample](https://github.com/b-com-software-basis/xpcf/blob/develop/doc/xpcf-registry-sample.xml) for configuration samples.

#### XPCF bugfixes:
- Fix[ModuleMetadata]: getFullPath error on windows
