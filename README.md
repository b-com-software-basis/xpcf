XPCF Cross-Platform Component Framework
=========================================
[![GitHub version](https://badge.fury.io/gh/b-com-software-basis%2Fxpcf.svg)](https://badge.fury.io/gh/b-com-software-basis%2Fxpcf)
[![LoC](https://tokei.rs/b1/github/b-com-software-basis/xpcf)](https://github.com/b-com-software-basis/xpcf)

XPCF is a lightweight cross platform component framework (it is similar to the well known COM model).

It is designed to provide dependency injection, with clarity, simplicity and light code base in mind.

It implements the abstractfactory and the factory design patterns (through the ComponentManager and ComponentFactory classes).

It also provides a safe toolkit to avoid common c++ design issues such as :

- ensure use of one unique shared_ptr type (even on platforms with several STLs : the SRef class is guaranteed to be from one source only through boost binary)
- ensure correct memory handling (avoid memory leaks)
- provide parallel helpers such as tasks, fifo, circular buffers, delays..
- a common configuration design : configuring a component doesn't require to write serialization code

## Changelog

[Learn about the latest improvements][changelog].

## Definitions

### Module

A module consists in a shared library hosting a set of factories to create components/services

Provides:

- introspection to figure out which components are available in the **module**

- separate implementation from interface : creates the concrete implementation and binds it to the IComponentIntrospect interface

- component creation

- service instanciation

###  Component/Service interface

A component/service interface in the component framework context is :

- based on C++ virtual class

- uniquely identified by an UUID (128-bit number guaranteed to be unique based on timestamps, HW addresses, random seeds)

- mandatory derives from ***IComponentIntrospect*** (the base interface of all XPCF interfaces)

### IComponentIntrospect interface

- is a special interface

- does not derive from any other interface

Provides:

- component introspection to query interfaces implemented by the component

- functional reference counting

Special Notes about queryInterface :

- queryInterface from interface A to interface B, must be able to queryInterface back from interface B to interface A

- queryInterface from interface A to the IComponentIntrospect interface, must equal the queryInterface from interface B to the  IComponentIntrospect interface


### Components and Services

An implementation of one or more interfaces identified by a Component UUID

NOTE: The introspection interface is the same for both components and services

#### component

- a class that can have several instances over the application lifetime

- each instance is unloaded (destroyed) when no more reference exists upon one of its interfaces

#### service

- a component singleton that only has one instance at any given time

- is never unloaded

## Schema

~~~
 ___________________________________________________
| Module #1                                      |
| entry point => XPCF_getComponent                  |
|           ______________________________          |
|          | Component #UUIDC1            |         |
|          |        ___________________   |         |
|          | I0 -> | I0 implementation |  |         |
|          |        -------------------   |         |
|          |                              |         |
|          |        ___________________   |         |
|          | I1 -> | I1 implementation |  |         |
|          |        -------------------   |         |
|           ------------------------------          |
|                                                   |
|           ______________________________          |
|          | Component #UUIDC2            |         |
|          |        ___________________   |         |
|          | I0 -> | I0 implementation |  |         |
|          |        -------------------   |         |
|          |                              |         |
|          |        ___________________   |         |
|          | I2 -> | I2 implementation |  |         |
|          |        -------------------   |         |
|           ------------------------------          |
|                                                   |
| entry point => XPCF_getService                    |
|           ______________________________          |
|          | Service #UUIDS1              |         |
|          |        ___________________   |         |
|          | I3 -> | I3 implementation |  |         |
|          |        -------------------   |         |
|           ------------------------------          |
 ---------------------------------------------------
~~~

## Framework functionalities
This framework provides the following concepts :

- an "in-code" dependency injection factory to create components and bind them to a specific interface

- a component abstract factory to load components at runtime from modules (named ModuleManager)

- increase modularity with better separation of concerns through interface discovery (using component introspection)

- a centralized description of modules, components and interfaces (with a dedicated configuration file)

- module introspection to figure out the components contained and the interfaces they implement

- secure memory management using smart pointers for components  (prevents from memory leaks, dangling null pointer)

- component and service configuration at creation through abstract variant parameters

- support for both components and services

- support for Domain Specific Language through the "IFeatures" interface (for instance, a component can declare "opencv" and "cuda" features as it is an opencv/cuda accelerated implementation of a keypointdetector)

**Note**: Variant parameters for configuration are a set of parameters. Each parameter is a pair of type, value. Supported types include long, unsigned long, double, string or another subset of parameters. Each parameter has an access definition : in, out or inout.

## Aliases
- an ```\<aliases\>``` section can be declared in any ```xpcf-registry``` or ```xpcf-configuration``` node to declare named alias to uuids for an interface, a component or a module.
- aliases can be used in ```<factory>...</factory>``` or ```<properties>...</properties>``` sections to refer indifferently to components, interfaces either through their uuids or alias.
  Note: an alias name must be unique in a ```ComponentManager``` context.
- autoalias mode: makes an alias between an interface/component name and its uuid. Provides the ability to use the alias in place of the uuid in factory bindings and in properties configuration (only supported with new ```<properties>``` semantic).
autoalias creates an alias when the first occurence of a name is met (particularly important for interfaces).
This mode is enabled when ```autoAlias``` xml attribute is set to "true" in ```<xpcf-registry autoAlias="...">``` or ```<xpcf-configuration autoAlias="...">```
Note:
- when a name is encountered twice, the second occurence is ignored (it should always be the same alias which is true for interfaces)
- any ambiguity must be resolved with explicit aliasing through an xml ```<aliases>``` node
- a potential evolution will be to "namespace" the component name with the module name to reduce naming conflicts

## Component dependency injection
- compile-time injection is provided through ```IInjectable``` interface
- local components can be used during component injection once registered to XPCF using ```bindLocal``` or ```bind``` methods (components defined in the library's own code or in libraries linked, but not from dynamically loaded modules). The bind method accepts a factory method to create the concrete component instance
-> ```ComponentBase``` implements ```IInjectable``` and provides injection capabilities to every xpcf components
- recursive dependency injection and configuration is supported
- injection errors are handled with ```InjectionException```, ```InjectableNotFoundException``` and ```InjectableDeclarationException``` exceptions
- declarative injection (configuration time injection) is provided in registry files through ```<factory>...</factory>``` declarations (refer to ````doc/xpcf-registry-sample.xml``` for the semantic)
- autobind mode is always enabled. While parsing the ```<module>``` nodes, it automatically "wires" (binds) components to their declared interfaces.
  autobind also works upon module introspection made at compile time using ```IComponentManager::loadModules``` or ```IComponentManager::loadModuleMetadata```.
Note: as autobind is a default behavior, only redundant/specific binds must be declared in ```<factory ... />``` node.
Otherwise, default binds are created from components definitions.
Note: Autobinding ignores ```IComponentIntrospect```, ```IConfigurable``` and ```IInjectable``` XPCF interfaces. Autobinding handles wiring from user's interfaces to components.

```IComponentManager``` provides:

- global "inject" methods helper
- dependency injection through default binding while parsing the ```<module>``` nodes, or through ```<factory><bindings>...``` declarations. 
- structured injection. See the semantic in xpcf-registry-sample.xml: node ```<injects>```.
- named binding injection ability either from configuration (using ```<factory>...</factory>``` node) or at compile time
- injection from bindings upon ```createComponent``` from component type trait or uuid
- concrete component instance resolution from its interface ( or service ) type traits or uuid (and optional name)
Upon creation of the concrete instance injection of the component injectables is made when needed through "resolve" methods, and each injectable is configured.
Finally the concrete instance is configured and returned to the caller.

 
## XPCF Qt Creator wizards

To help developers create efficiently XPCF based applications, several Qt Creator wizards are provided.
Wizards are located in the XPCF release in xpcf/version/wizards, or in the [xpcf github repository](https://github.com/b-com-software-basis/xpcf) in the wizards folder.
To install the wizards, first install Qt Creator, then launch install.bat (windows platform) or install.sh (linux or macosX platforms).

### XPCF Module wizard
This wizard creates a complete Qt Project to build a shared library containing a xpcf module. It contains a skeleton module_main.cpp file with the main XPCF entry point. It depends on qmake rules (from the [builddefs-qmake github repository](https://github.com/b-com-software-basis/builddefs-qmake), installed with the remaken package manager tool from [remaken github repository](https://github.com/b-com-software-basis/remaken) ).

### XPCF Interface wizard
This wizard creates a xpcf interface header file. This file is the public API. Any component interested to provide this interface functionality will use this header file.  The developer only need to add its methods to the interface.

### XPCF Component wizard
This wizard creates a xpcf component header and source file. The developer only need to override and implement the methods defined in the interfaces provided by the component.
The component header file doesn't need to be exposed. Only the interfaces implemented are part of the API.

### XPCF Application wizard
This wizard creates a complete Qt Project to build an application that use xpcf module and components.


## XPCF configuration file format
XPCF components can be found, configured and injected with a xml configuration file.

###Node tree
The supported xml structure is (in each xml node, **"..."** symbolizes attributes):

~~~
<xpcf-registry>    
    <module ... > <!-- module declaration -->   
        <component ... > <!-- component declaration --> 
            <interface ... />  <!-- interface declaration --> 
        </component>
        <!-- ... other component declarations -->            
    </module>    

    <aliases>   
        <alias ... /> 
        <!-- ... other alias declarations -->   
    </aliases>   

    <factory>
        <!-- default bindings -->   
        <bindings> 
            <bind ... />
            <!-- ... other binding declarations -->
        </bindings>
        <!-- custom bindings : explicit injection for a particular component class -->       
        <injects> 
            <inject ... >
                <bind ... />
                <!-- ... other binding declarations -->
            </inject>
            <!-- ... other inject declarations -->
        </injects> 
    </factory>   
    
    <properties>   <!-- properties declaration -->
        <configure ... > <!-- component configuration declaration --> 
            <property ... /> <!-- single value property declaration -->
            <property ...> <!-- multi value property (vector) declaration -->
                <value>...</value>
                <!-- ... other value declarations -->
            </property> 
            <!-- ... other property declarations -->               
        </configure>      
        <!-- ... other component configuration declarations -->              
    </properties>   
</xpcf-registry>
~~~


### Node description

| Node | Section (parent node) |Attributes | Semantic|
|---|---|---|---|
|xpcf-registry | document root | **autoAlias** = [true, false]<br>-> set whether XPCF must automatically create aliases while parsing the file between :<br>component UUID <-> component name<br> interface UUID <-> interface name | This node declares an xpcf registry file, containing both components and modules structure and their properties| 
|xpcf-configuration | document root | | This node declares an xpcf configuration file only, containing components properties| 
|module | xpcf-registry | **uuid** = module is referenced with an uuid. It must be declared here. <br>**name** = name of the module used<br>**path** = path to the module used (can contain environment variables)<br>**description** = the module function description | declares an xpcf module|
|component | module |  **uuid** = component is referenced with an uuid. It must be declared here. <br>**name** = name of the component used<br>**description** = the component description| declares an xpcf component inside a module|
|interface | component | **uuid** = interface is referenced with an uuid. It must be declared here. <br>**name** = name of the interface used<br>**description** = the interface description | declares an xpcf interface implemented by a component|
|||||
|aliases | xpcf-registry | | declares an aliases section. When **autoAlias = true** in **\<xpcf-registry\>**, an alias is created for each component/interface in **\<module\>** node when the first occurence of a name is met. Hence the **\<aliases\>** section must be used to resolve any name ambiguity, or to simplify a non intuitive name. |
|alias | aliases | **name** = name of the alias <br> **type**=[component, interface]<br> **uuid** = the uuid of the component/interface targeted by the **alias**| declares an alias. Alias can be ...|
|factory | xpcf-registry | | declares the factory section |
|bindings | factory | | declares the bindings section - this section is needed only to overload autobinds made while parsing the **&lt;module&gt;** node|
|bind | bindings | **interface** = interface **alias** or **uuid**<br>**to** = component **alias** or **uuid**<br>[optional] **name** = name of the binding| declares a default or named bind between an interface and a component|
|injects | factory | | declares the injects section - used for structured (also called planned) injection for specific component class|
|inject | injects | **to** = component **alias** or **uuid**| declares an injection pattern for a specific component class|
|bind | inject |**interface** = interface **alias** or **uuid**<br>**to** = component **alias** or **uuid**<br>[optional] **name** = name of the binding| declares a specific (scope bind) bind between an interface and a component for the specific component class declared in **&lt;inject&gt;**|
|||||
|properties | xpcf-registry| | declares the components properties section. This section defines component parameters values for configurable components.|
|configure | properties | [optional] **uuid** = the component uuid or <br>  **component** = a component alias name| declares the configuration for a specific component |
|property | configure | **name** = property name <br>**type** = [int, uint, long, ulong, string, wstring, float, double, structure] <br>**value** = property value (must be a valid value for the type chosen) <br> **value** can also be declared as sub-nodes for vector properties | declares a component property|
|value | property | | declares a component property value|

**Note :**
> Aliasing order : an alias declared in the <aliases> section is added to the list of existing aliases, or replace any existing alias with the same name. <aliases> section overload autoAlias already declared. 

> Binding order : 
> - autobind : during the parsing of the **\<module\>** nodes, a bind is added the first time an interface is met. If the same interface is also declared for another component, the first bind is maintained. where a [interface, component] pair is met. 
> - factory bindings : when an explicit bind is defined, it replaces any previous autobind made while parsing the **\<module\>** nodes. 

### Sample configuration file

~~~
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<xpcf-registry autoAlias="true">
    <module uuid="{3b899ff0-e098-4218-bdaa-82abdec22960}" name="xpcfModule" description="an sample module with some components" path="$REMAKEN_RULES_ROOT/xpcfModule/2.3.1/lib/x86_64/shared">
	    <component uuid="ae9d10bb-4bd1-462b-8c01-43f1815c6ae0" name="feature1Component - not configurable" description="feature1Component is not Configurable, it implements I0 and I1 interfaces">
	        <interface uuid="125f2007-1bf9-421d-9367-fbdc1210d006" name="XPCF::IComponentIntrospect" description="Component introspection interface."/>
	        <interface uuid="46333fd5-9eeb-4c9a-9912-b7df96ccf5fc" name="I0" description="provides I0 features"/>
	        <interface uuid="3bc8f5ea-ee24-473e-8afd-4f5b1b21c018" name="I1" description="provides I1 features"/>
	    </component>
	    <component uuid="63ff193d-93e6-4ede-9947-22f864ac843f" name="feature2Component - configurable" description="feature2Component implements IConfigurable, I2 and I1 interfaces">
	        <interface uuid="125f2007-1bf9-421d-9367-fbdc1210d006" name="XPCF::IComponentIntrospect" description="Component introspection interface."/>
	        <interface uuid="98dba14f-6ef9-462e-a387-34756b4cba80" name="XPCF::IConfigurable" description="Provides generic configuration ability to any component through metadata parameters"/>
	        <interface uuid="3bc8f5ea-ee24-473e-8afd-4f5b1b21c018" name="I1" description="provides I1 features"/>
	        <interface uuid="41348765-1017-47a7-ab9f-6b59d39e4b95" name="I2" description="provides I2 features"/>
	    </component>
	    <component uuid="{7cc64f89-9a81-4fc8-9695-fde123fae225}" name="technicalComponent - not configurable" description="component provides technical feature to functional components">
	        <interface uuid="125f2007-1bf9-421d-9367-fbdc1210d006" name="XPCF::IComponentIntrospect" description="Component introspection interface."/>
	        <interface uuid="41348765-1017-47a7-ab9f-6b59d39e4b95" name="I2" description="provides I2 features"/>
	        <interface uuid="6279bb1d-69b5-42bd-9da1-743c8922bcb9" name="I3" description="provides I3 features"/>
	    </component>
    </module>
    <aliases>
            <alias name="feature1Component" type="component" uuid="ae9d10bb-4bd1-462b-8c01-43f1815c6ae0"/>
            <alias name="feature2Component" type="component" uuid="63ff193d-93e6-4ede-9947-22f864ac843f"/>
            <alias name="technicalComponent" type="component" uuid="7cc64f89-9a81-4fc8-9695-fde123fae225"/>
    </aliases>
    <factory>
        <!-- default bindings -->
        <bindings>
            <bind interface="I1" to="feature1Component" />
            <bind interface="3bc8f5ea-ee24-473e-8afd-4f5b1b21c018" to="ae9d10bb-4bd1-462b-8c01-43f1815c6ae0" />
            <bind name="feature2Component_I1" interface="I1" to="feature2Component"/>
        </bindings>
        <!-- custom bindings : explicit injection for a particular component class -->
        <injects>
            <inject to="feature1Component">
                    <bind interface="I2" to="technicalComponent" />
            </inject>
        </injects>
    </factory>
    <properties>
        <configure component="feature2Component">
                <property name="parameter1" type="double" value="2.0"/>
                <property name="vector_parameter1" type="double" access="ro">
                    <value>0.2</value>
                    <value>0.5</value>
                </property>
                <property name="structured_parameter"  type = "structure">
                    <property name="cols" type="long" value="2"/>
                    <property name="rows" type="long" value="2"/><!-- rowmajormode only-->
                    <property name="values" type="double">
                        <value>1.1</value>
                        <value>0.7</value>
                        <value>0.3</value>
                        <value>2.7</value>
                    </property>
                </property>
        </configure>
    </properties>
</xpcf-registry>
~~~



## How to build XPCF
In order to build XPCF:

- Install [conan](https://conan.io/)
- Install [QT](https://www.qt.io/) to get QT Creator
- Clone xpcf repository with submodules (```git clone --recursive https://github.com/b-com-software-basis/xpcf.git```). If you have already cloned the repository but not the submodules, run ```git submodule update --init --recursive``` within your xpcf repository folder.
- Open xpcf.pro file in QT Creator and configure the project with the compilation kit of your OS (gcc on Linux, clang on mac os X, msvc 2017 for windows)
- Run qmake - this can take a while as it should compile boost from conan.
- Add an install target for make in "compilation steps" from the "projects" tab in Qt Creator

You can now build xpcf using Build inside Qt Creator.

The compilation result will be located in ```$HOME/.remaken/packages/[platform-compiler-version]/xpcf/XPCF\_VERSION```
(```%USERPROFILE%\\.remaken\packages\\[platform-compiler-version]\xpcf\XPCF\_VERSION``` on windows)


**Note**: To automatically include and link with xpcf headers and library, use the pkgconfig file bcom-xpcf.pc located at the root of the above folder.

## Creating your first component
Components are hosted in modules (i.e. shared libraries with special hook methods).
Hence prior to the creation of components, start by creating a module.

To create an XPCF module, interface or component it is recommended to use the QT
Creator wizards located in ```wizards/qtcreator``` folder in xpcf repository.

To install the wizards, please refer to [INSTALL.txt](https://github.com/b-com-software-basis/xpcf/blob/master/wizards/qtcreator/INSTALL.txt).

[changelog]: https://github.com/b-com-software-basis/xpcf/blob/master/CHANGELOG.md
