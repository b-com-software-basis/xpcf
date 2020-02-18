### XPCF evolutions
- Update xpcf version to 2.3.1
- Update builddefs-qmake to v3.13.0

#### XPCF improvements:
- ```Factory.cpp```: fix windows regression due to planned injection: remove ```ScopeExit``` calls
- ```packagedependencies.txt```: fix conan repository name
- ```interfaces/xpccf/corehelpers.h```: add SWIG define around  code
- ```README.md```: 
	- add ```Aliases``` and ```Component dependency injection``` sections
	- add xpcf xml configuration file documentation
	- improve ```How to build XPCF``` section 

#### QT Creator wizards for module, component, interface creation improvements:
- install wizards in xpcf package directory with xpcf library upon ```make install```
- update wizards installation scripts
- fix missing macro declaration in XPCF component wizard
- fix bug in QT module wizard
- Improve module and application wizards: add dependencies configuration options (shared or static dependencies, whether to install the dependencies or not with the built project)
