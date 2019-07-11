### XPCF new features (refer to chapters for details)
1. aliases support in registry/configuration files with autoalias mode	
2. component dependency injection either at compile time or at configuration time
3. module introspection and validation
4. new component properties xml declaration semantic
5. QT Creator wizards for module, component, interface creation
 
#### Aliases:
- an \<aliases\> section can be declared in any xpcf registry/configuration node to declare named alias to uuids for an interface, a component or a module.
- aliases can be used in \<factory\>...\</factory\> or \<properties\>...\</properties\> sections to refer indifferently to components, interfaces either through their uuids or alias.
  Note: an alias name must be unique in a ComponentManager context.
- autoalias mode: makes an alias between an interface/component name and its uuid. Provides the ability to use the alias in place of the uuid in factory bindings and in properties configuration (only supported with new <properties> semantic).
autoalias creates an alias when the first occurence of a name is met (particularly important for interfaces).
This mode is enabled when autoAlias xml attribute is set to "true" in \<xpcf-registry autoAlias="..."\> or \<xpcf-configuration autoAlias="..."\>
Note:
- when a name is encountered twice, the second occurence is ignored (it should always be the same alias which is true for interfaces)
- any ambiguity must be resolved with explicit aliasing through an xml \<aliases\> node
- a potential evolution will be to "namespace" the component name with the module name to reduce naming conflicts
 
#### Component dependency injection:
- compile-time injection is provided through IInjectable interface
-> ComponentBase implements IInjectable and provides injection capabilities to every xpcf components
- injection errors are handled with InjectionException, InjectableNotFoundException and InjectableDeclarationException exceptions
- declarative injection (configuration time injection) is provided in registry files through \<factory\>...\</factory\> declarations (refer to doc/xpcf-registry-sample.xml for the semantic)
- autobind mode is always enabled. While parsing the <module> nodes, it automatically "wires" (binds) components to their declared interfaces.
  autobind also works upon module introspection made at compile time using IComponentManager::loadModules or IComponentManager::loadModuleMetadata.
Note: as autobind is a default behavior, only redundant/specific binds must be declared in \<factory ... /\> node.
Otherwise, default binds are created from components definitions.
Note: Autobinding ignores IComponentIntrospect, IConfigurable and IInjectable XPCF interfaces. Autobinding handles wiring from user's interfaces to components.
 
IComponentManager provides:

- global "inject" methods helper
- dependency injection through default binding (structured injection will come later, semantic is described in xpcf-registry-sample.xml: node \<injects\>)
- named binding injection ability either from configuration (using \<factory\>...\</factory\> node) or at compile time
- injection from bindings upon createComponent from component type trait or uuid
- concrete component instance resolution from its interface ( or service ) type traits or uuid (and optional name)
Upon creation of the concrete instance injection of the component injectables is made when needed through "resolve" methods, and each injectable is configured.
Finally the concrete instance is configured and returned to the caller.
 
#### Module:
- module validation: checks the shared library is a valid XPCF module (i.e. it provides the needed XPCF expected hooks)
- module introspection: introspects a shared library to retrieve its metadata used to feed the ComponentManager
 
IModuleManager:

- introspectModule: module introspection to get the module metadata
 
IComponentManager:

- loadModules and loadModuleMetadata methods to introspect a module and feed the ComponentManager with the metadata retrieved
 
#### Registry/configuration file semantic:
This new semantic support indifferently aliased name or uuids in component attribute (refer to doc/xpcf-registry-sample.xml for the detailed semantic).
Component properties configuration semantic is \<properties\>\<configure component="[uuid or alias]" ...\>\<property\>...\</property\>	\</properties\>.
 
IConfigurable:

- implement declareProperty* semantic to mimic declareInjectable and ease properties declarations.
 
IComponentManager:

- improve configuration file load : allow pure configuration files without any module/component declarations
 
#### QT Creator wizards:
- qtcreator wizards for xpcf module, components and interfaces creation
- wizards installation scripts
 
Tests:

- add configuration based and compile time injection tests

### XPCF evolutions
- use const & for parameters in xpcf interfaces in most cases in order to avoid useless copies and to get homogeneous interfaces
- update builddefs-qmake to v3.9.8
 
#### ComponentManager:
- split ComponentManager registry code into AliasManager, Factory, Registry : add interface and component behavior to use injection between the services
- add registry file loading from .xpcf home folder when registry file path is relative and is not found from the application working dir
 
#### ComponentBase:
- add declareInterface\<I\> method to get a common naming with declareInjectable and declareProperty.
Note: addInterface\<I\> is now deprecated.
 
#### ComponentFactory:
- add template implementation for ComponentFactory::createInstance to ease instance creation declaration
 
#### Sample code updates:
- add sample runner application to test various xpcf samples implementing IMessage
- add "Hello world" samples to show various xpcf use case
- improve sample component code : add second interface to Jackson guitar component
- all code samples use latest xpcf evolutions around injection, properties to illustrate their use
 
#### XPCF dependencies:
- migrate to boost:1.70.0 conan dependency.
- change dependencies path to remaken default path

#### Readme.md:
- add "How to build XPCF" chapter

 
