^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package class_loader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
