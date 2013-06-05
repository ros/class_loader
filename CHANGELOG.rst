^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package class_loader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
* improve check for Poco foundation and headers (`#7 <https://github.com/ros/class_loader/issues/7>`_)
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.28...groovy-devel>`_

0.1.28 (2013-03-21)
-------------------
* use find_package for Poco/dl instead to make it work on other platforms
* update Poco cmake file to include libdl on non-windows systems
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.27...0.1.28>`_

0.1.27 (2013-01-25)
-------------------
* change warning message for managed/unmanaged instance mixture in lazy loading mode
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.26...0.1.27>`_

0.1.26 (2013-01-17)
-------------------
* fix all instances marked as unmanaged
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.25...0.1.26>`_

0.1.25 (2013-01-16)
-------------------
* fix redundant destructor definition being pulled into plugin library for metaobjects instead of being contained with libclass_loader.so
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.24...0.1.25>`_

0.1.24 (2013-01-14 15:27)
-------------------------
* fix syntax error for logInform
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.23...0.1.24>`_

0.1.23 (2013-01-14 15:23)
-------------------------
* downgrade some warning messages to be info/debug
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.22...0.1.23>`_

0.1.22 (2013-01-14 15:01)
-------------------------
* add safety checks for mixing of managed/unmanaged mixing as well as pointer equivalency check between graveyard and newly created metaobjects
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.21...0.1.22>`_

0.1.21 (2013-01-13)
-------------------
* fix compile issue on OSX in dependent packages (`#3 <https://github.com/ros/class_loader/issues/3>`_)
* add more debug information
* for a complete list of changes see the `commit log <https://github.com/ros/class_loader/compare/0.1.20...0.1.21>`_

0.1.20 (2012-12-21 16:04)
-------------------------
* first public release for Groovy
