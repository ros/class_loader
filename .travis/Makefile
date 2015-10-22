all: class_loader

UNAME := $(shell uname -s)

install_deps:
ifeq ($(UNAME),Darwin)
	brew update
	brew outdated python3 || brew upgrade python3 || brew install python3
	brew outdated poco || brew upgrade poco || brew install poco
	sudo -H python3 -m pip install -U empy
else
	sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu trusty main" > /etc/apt/sources.list.d/ros-latest.list'
	wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
	sudo apt-get update -qq
	sudo apt-get install cmake python3 python3-pip python3-empy python3-setuptools wget libpoco-dev
endif
	python3 --version
