^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package class_loader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.5.0 (2020-02-07)
------------------
* Declare specific boost dependencies. (`#136 <https://github.com/ros/class_loader/issues/136>`_)
* Contributors: Mikael Arguedas

0.4.2 (2020-02-07)
------------------
* Add Python 3 support to header update scripts. (`#122 <https://github.com/ros/class_loader/issues/122>`_)
* Set test dll runtime output to CATKIN_PACKAGE_BIN_DESTINATION on Windows. (`#113 <https://github.com/ros/class_loader/issues/113>`_)
* Improve warning message formatting and detail. (`#108 <https://github.com/ros/class_loader/issues/108>`_)
* Add export to variable in order to allow builds on Windows. (`#102 <https://github.com/ros/class_loader/issues/102>`_)
* Ignore warnings about import/exports when deriving from std classes on MSVC. (`#116 <https://github.com/ros/class_loader/issues/116>`_)
* Use #pragma message() to print out warnings in MSVC. (`#114 <https://github.com/ros/class_loader/issues/114>`_)
* Avoid including Poco headers globally. (`#115 <https://github.com/ros/class_loader/issues/115>`_)
* Make Steven! Ragnarok the maintainer (`#107 <https://github.com/ros/class_loader/issues/107>`_)
* Fix non-defined CMake variable (forward-port to Melodic). (`#97 <https://github.com/ros/class_loader/issues/97>`_)
* Contributors: James Xu, Johnson Shih, Markus Grimm, Mikael Arguedas, Robert Haschke

0.4.1 (2018-04-27)
------------------
* Provide std::shared_ptr interface (`#95 <https://github.com/ros/class_loader/issues/95>`_)
* Windows compat and style fixups (`#90 <https://github.com/ros/class_loader/issues/90>`_)
  * add visibility macros to public functions
  * rename private namespace 'class_loader_private' to 'impl' to match ros2 branch
* use new headers to build library (`#93 <https://github.com/ros/class_loader/issues/93>`_)
* Contributors: Mikael Arguedas

0.4.0 (2018-02-15)
------------------
* Stop checking for c++11 support (`#87 <https://github.com/ros/class_loader/pull/87>`_)
  all Melodic targeted platforms use gnu++14 so checking and forcing -std=c++11 doesn't make sense anymore
* [ABI breaking] use std::string references for exceptions (`#86 <https://github.com/ros/class_loader/issues/86>`_)
* deprecate .h headers in favor of .hpp headers (`#81 <https://github.com/ros/class_loader/pull/81>`_)
* provide a script with exhaustive rules for header replacement
* comply with package format2 xsd (`#83 <https://github.com/ros/class_loader/issues/83>`_)
* [ABI breaking] Exceptions fixups (`#82 <https://github.com/ros/class_loader/issues/82>`_)
  * inline exceptions
  * use throw statement rather than function
* [linter] add nolint for global std::string used for testing (`#79 <https://github.com/ros/class_loader/issues/79>`_)
* use auto for all for loops iterating on vectors/maps (`#78 <https://github.com/ros/class_loader/issues/78>`_)
* Add systemLibraryFormat and systemLibraryPrefix functions (`#77 <https://github.com/ros/class_loader/issues/77>`_)
* [ABI breaking] Bring melodic-devel closer to ros2 branch (`#76 <https://github.com/ros/class_loader/issues/76>`_)
  * comply with extra and pedantic compiler flags
  * use c++11 nullptr instead of NULL
  * make ABI breaking change for explicit constructors
  * make linters happy
  * no need to support console_bridge < 0.3.0 anymore
  * remove obsolete todo
  * add virtual destructor in test
  * vector size() returns size_t
  * simplify branching
* [fix warnings] c++11 requires at least one argument for ... (`#71 <https://github.com/ros/class_loader/issues/71>`_)
* [linter] Use std::string::empty instead comparing with an empty string (`#69 <https://github.com/ros/class_loader/issues/69>`_)
* [linter] wrap console bridge invocation lines (`#68 <https://github.com/ros/class_loader/issues/68>`_)
* OSRF and not willow in licence header (`#67 <https://github.com/ros/class_loader/issues/67>`_)
* Contributors: David Wagner, Mikael Arguedas

0.3.8 (2017-11-16)
------------------
* Fix console_bridge marcos definition (`#66 <https://github.com/ros/class_loader/issues/66>`_)
* Style overhaul (`#64 <https://github.com/ros/class_loader/issues/64>`_) (`#62 <https://github.com/ros/class_loader/issues/62>`_)
* Add copyright notice to unique_ptr_test.cpp (`#65 <https://github.com/ros/class_loader/issues/65>`_)
* Contributors: Maarten de Vries, Mikael Arguedas

0.3.7 (2017-07-27)
------------------
* switch to package format 2 (`#56 <https://github.com/ros/class_loader/issues/56>`_)
* remove trailing whitespaces (`#55 <https://github.com/ros/class_loader/issues/55>`_)
* use CONSOLE_BRIDGE_X logging macros (`#52 <https://github.com/ros/class_loader/issues/52>`_)
* Contributors: Mikael Arguedas, jmachowinski

0.3.6 (2016-10-24)
------------------
* Made changes to two locking mechanisms inside class loader core's loadLibrary() function: 1) I added a lock to the 'addClassLoaderOwnerFor...' function to protect it against a race condition with the unloadLibrary() function. 2) I also raised the loader lock to cover the whole function. Previously the check to see if a library is already loaded and the actual loading of the library was not atomic. Multiple threads could create shared library objects, for example.
* Contributors: Jonathan Meyer

0.3.5 (2016-09-20)
------------------
* Add ClassLoader::createUniqueInstance (`#38 <https://github.com/ros/class_loader/issues/38>`_)
  * Wrap comments on createInstance and friend.
  * Delegate createInstance and createUnmanagedInstance to private impl.
  * Add ClassLoader::createUniqueInstance.
  * MultiLibraryClassLoader: Factor out getClassLoaderForClass.
  * MultiLibraryClassLoader: Add unique_ptr API.
  * Add tests for unique_ptr API.
* Contributors: Maarten de Vries

0.3.4 (2016-06-22)
------------------
* cleanup: don't use active_class_loaders\_[library_path] for existence test (`#35 <https://github.com/ros/class_loader/issues/35>`_)
  * cleanup: don't use active_class_loaders\_[library_path] for existence test
  - this accumulates map entries with NULL pointer
  - fixing it, allows some cleanup
  * list headers in CodeBlocks / QtCreator
  * explicitly list all headers
* Merge pull request `#34 <https://github.com/ros/class_loader/issues/34>`_ from rhaschke/fix-on-demand-unloading
  fix on demand unloading
* Merge pull request `#32 <https://github.com/ros/class_loader/issues/32>`_ from saarnold/fixed_unset_variable_evaluation
  fixed evaluation of undefined variable
* fixed evaluation of undefined variable
* not unloading the ClassLoaders (to avoid the SEVERE WARNING) doesn't work either
* bugfix: enable on-demand loading/unloading with MultiClassLoader
  - enforce loading of library in loadLibrary(), otherwise we cannot know
  - don't unload libraries in destructor when on-demand-unloading is enabled
* extra utest: MultiClassLoaderTest.lazyLoad succeeds two times in a row?
* added MultiLibraryClassLoader unittest
* Contributors: Mikael Arguedas, Robert Haschke, Sascha Arnold

0.3.3 (2016-03-10)
------------------
* update maintainer
* Merge pull request `#26 <https://github.com/ros/class_loader/issues/26>`_ from goldhoorn/indigo-devel
  Added option to disable the catkin build
* Added option to disable the catkin build
* Contributors: Esteve Fernandez, Matthias Goldhoorn, Mikael Arguedas

0.3.2 (2015-04-22)
------------------
* Fixed wrong handling of false statement (pkg-config was not installed)
* Make catkin optional again
* Contributors: Esteve Fernandez, Janosch Machowinski, Matthias Goldhoorn

0.3.1 (2014-12-23)
------------------
* Depend on boost
* Use FindPoco.cmake from ros/cmake_modules
*  Honor BUILD_SHARED_LIBS and do not force building shared libraries.
* Contributors: Esteve Fernandez, Gary Servin, Scott K Logan

0.3.0 (2014-06-25)
------------------
* Use system-provided console-bridge
* Contributors: Esteve Fernandez

0.2.5 (2014-03-04)
------------------
* Changed format of debug messages so that rosconsole_bridge can correctly parse the prefix
* Improved debug output

0.2.4 (2014-02-12)
------------------
* fix race condition with multi threaded library loading (`#16 <https://github.com/ros/class_loader/issues/16>`_)

0.2.3 (2013-08-21)
------------------
* fix missing class name in logWarn output

0.2.2 (2013-07-14)
------------------
* check for CATKIN_ENABLE_TESTING (`#10 <https://github.com/ros/class_loader/issues/10>`_)
* fix find Poco to return full lib path (`#8 <https://github.com/ros/class_loader/issues/8>`_)
* add missing runtime destination for library under Windows
* add Boosst component system

0.2.1 (2013-06-06)
------------------
* improve check for Poco foundation and headers (`#7 <https://github.com/ros/class_loader/issues/7>`_)

0.2.0 (2013-03-13)
------------------
* use find_package for Poco/dl instead to make it work on other platforms
* update Poco cmake file to include libdl on non-windows systems
* No longer CATKIN_DEPEND on console_bridge

0.1.27 (2013-01-25)
-------------------
* change warning message for managed/unmanaged instance mixture in lazy loading mode

0.1.26 (2013-01-17)
-------------------
* fix all instances marked as unmanaged

0.1.25 (2013-01-16)
-------------------
* fix redundant destructor definition being pulled into plugin library for metaobjects instead of being contained with libclass_loader.so

0.1.24 (2013-01-14 15:27)
-------------------------
* fix syntax error for logInform

0.1.23 (2013-01-14 15:23)
-------------------------
* downgrade some warning messages to be info/debug

0.1.22 (2013-01-14 15:01)
-------------------------
* add safety checks for mixing of managed/unmanaged mixing as well as pointer equivalency check between graveyard and newly created metaobjects

0.1.21 (2013-01-13)
-------------------
* fix compile issue on OSX in dependent packages (`#3 <https://github.com/ros/class_loader/issues/3>`_)
* add more debug information

0.1.20 (2012-12-21 16:04)
-------------------------
* first public release for Groovy
