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

## How to build XPCF
In order to build XPCF:

- Install conan from https://conan.io/
- Install QT to get QT Creator from https://www.qt.io/
- Clone xpcf repository
- Open xpcf.pro file in QT Creator and configure the project with the compilation kit of your OS (gcc on Linux, clang on mac os X, msvc 2017 for windows)
- Run qmake - this can take a while as it should compile boost from conan.
- Add an install target for make in "compilation steps" from the "projects" tab in Qt Creator

You can now build xpcf using Build inside Qt Creator.

The compilation result will be located in $HOME/.remaken/packages/[platform-compiler-version]/xpcf/XPCF\_VERSION
(%USERPROFILE%\\.remaken\packages\\[platform-compiler-version]\xpcf\XPCF\_VERSION on windows)


**Note**: To automatically include and link with xpcf headers and library, use the pkgconfig file bcom-xpcf.pc located at the root of the above folder.

## Creating your first component
Components are hosted in modules (i.e. shared libraries with special hook methods).
Hence prior to the creation of components, start by creating a module.

To create an XPCF module, interface or component it is recommended to use the QT
Creator wizards located in wizards/qtcreator folder in xpcf repository.

To install the wizards, please refer to wizards/qtcreator/INSTALL.txt file.

[changelog]: https://github.com/b-com-software-basis/xpcf/blob/master/CHANGELOG.md
