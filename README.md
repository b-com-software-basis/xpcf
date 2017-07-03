XPCF Cross-Platform Component Framework
=========================================
XPCF is a lightweight cross platform component framework (it is similar to the well known COM model).

It is designed to provide dependency injection, with clarity, simplicity and light code base in mind.

It implements the abstractfactory and the factory design patterns (through the ComponentManager and ComponentFactory classes).

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

NOTE: Variant parameters for configuration are a set of parameters. Each parameter is a pair of type, value. Supported types include long, unsigned long, double, string or another subset of parameters. Each parameter has an access definition : in, out or inout.

## Making a module

The shared library must export the API XPCF_getComponent.

~~~
long XPCF_getComponent(const boost::uuids::uuid &, SRef<IComponentIntrospect>&)
~~~

You must return an IComponentIntrospect when called.

## ContractID verses CID

CID is a number identifying a specific implementation

ContractID defines a set of functionalities which may contain one or more CIDs

ContractID concept may be implemented through the IFeatures interface

Purpose of Contract IDs

- implementation independence for clients

- functional versioning
