### XPCF evolutions
Update xpcf version to 2.3.0 
Update builddefs-qmake to v3.9.9

#### XPCF improvements:
- improve documentation 
- SWIG : wrap template and [[deprecated]] attribute definitions for compatibility
- move ScopeExit class to XPCF helpers
- provide the ability to inject local components through bind (components defined in the library's own code or in libraries linked, but not from dynamically loaded modules). The bind method accepts a factory method to create the concrete instance.
- recursive configuration is handled (usefull for injection patterns).
- recursive dependency injection is supported 
- improve separation of concerns : move properties xml parsing and configuration filepath handling from ComponentManager to PropertyManager
- improve error handling : wrap XPCF exception to XpcfErrorCode 

XPCF Factory : 
- improve initialisation : use custom local binds for AliasManager and Registry 
- start explicit bindings implementation for a specific component type
- singleton resolving : singleton is now referenced for a particular implementation, not for an interface !

#### XPCF Code cleanup :
- remove useless inline keyword
- remove useless std::placeholders
- improve encapsulation of IConfigurable : move templates methods to declare properties in ConfigurableBase.h 

#### XPCF threading interface improvements:
- improve threading abstraction
- fix SharedBuffer regression : remove template non type parameter for buffer size 
- add MultiConsumer class : allows several consumers to get data from a unique synchronized queue. Each item of the source queue is read once all consumers have read the previous value.
- synchronized queue finalization handling upon stop to unblock joining thread
- add join in BaseTask destruction 
- add the ability to provide a stopPredicate function upon DelegateTask construction

#### QT Creator wizards for module, component, interface creation improvements:
- add findremakenrules.pri to deduce remaken rules installed path in Qt wizards
- fix missing macro declaration in XPCF component wizard
- fix bug in QT module wizard

Work in progress: explicit factory bindings.