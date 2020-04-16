This document is a declaration of software quality for the `class_loader` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `class_loader` Quality Declaration

The package `class_loader` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`class_loader` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#versioning), and is at or above a stable version. **The current version is 1.4.0**.

### Version Stability [1.ii]

### Public API Declaration [1.iii]

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`class_loader` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`class_loader` will maintain ABI stability within a ROS distribution.

### Public API Declaration

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

## Change Control Process [2]

`class_loader` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#change_control_process).

### Change Requests [2.i]

All changes occur through a pull request

### Contributor Origin [2.ii]

Must have confirmation of contributor origin (e.g. DCO, CLA, etc.).

### Peer Review Policy [2.iii]

All pull request have two peer reviews

### Continuous Integration [2.iv]

all pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

### Documentation Policy [2.v]

All pull request must resolve related documentation changes before merging

## Documentation [3]

### Feature Documentation [3.i]

`class_loader` has a [feature list](http://wiki.ros.org/class_loader) and each item in the list links to the corresponding feature documentation.
There is documentation for all of the features, and new features require documentation before being added.

### Public API Documentation [3.ii]

`class_loader` has embedded API documentation and it is generated using doxygen and is [hosted](http://wiki.ros.org/class_loader) along side the feature documentation.
There is documentation for all of the public API, and new additions to the public API require documentation before being added.

### License [3.iii]

The license for `class_loader` is BSD-3-Clause, and a summary is in each source file, the type is declared in the `package.xml` manifest file, and a full copy of the license is in the [LICENSE](./LICENSE) file.

There is an automated test which runs a linter (ament_copyright) that ensures each file has a license statement.

### Copyright Statement [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `class_loader`.

There is an automated test which runs a linter (ament_copyright) that ensures each file has at least one copyright statement.

## Testing [4]

### Feature Testing [4.i]

Each feature in `class_loader` has corresponding tests which simulate typical usage, and they are located in the `test` directory.
New features are required to have tests before being added.

### Public API Testing [4.ii]

Each part of the public API have tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`class_loader` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#coverage), and opts to use branch coverage instead of line coverage.

This includes:

- tracking and reporting branch coverage statistics
- achieving and maintaining branch coverage at or above 95%
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Current coverage statistics can be viewed here:

![](https://codecov.io/gh/ahcorde/class_loader/branch/ahcorde_code_coverage/graphs/sunburst.svg)

### Performance [4.iv]

`class_loader` follows the recommendations for performance testing of C++ code in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#performance-testing), and opts to do performance analysis on each release rather than each change.

### Linters and Static Analysis [4.v]

`class_loader` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#linters-and-static-analysis).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]

### Optional Direct Runtime ROS Dependencies [5.ii]

### Direct Runtime non-ROS Dependency [5.iii]

`class_loader` has run-time and build-time dependencies that need to be considered for this declaration.

 - console_bridge_vendor
 - libconsole-bridge-dev

It has one "buildtool" dependency, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

## Platform Support [6]

`class_loader` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

TODO make additional statements about non-tier 1 platforms?
