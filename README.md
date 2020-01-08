# A simple linux kernel module for Mastermind game
This project contains a simple kernel module which enables the user to play the mastermind game (albeit a very dumbed down version). It is meant to be only for educational and learning purposes. I wanted to see how to create a linux kernel module and this educational assignment helped in achieving that.

## Compiling and running
To compile, clone the repository and inside the main folder, run following command:

```sudo make```

This will generate the required kernel modules, which can be inserted using:

```sudo insmod mastermind.ko```

You can then run the test file provided to test out the functionality:

```./mastermind-test```

## Removing from system
To remove from system, simply run:

```sudo rmmod mastermind```

Then clean the build files using:

```sudo make clean```

## Requirements
It was tested on Linux Kernel 5.2 source tree. However, it should work on most newer versions of the kernel. You need to have linux kernel headers installed for linux kernel mod development.

### For Debian based distributions:
```sudo apt install linux-headers-$(uname -r)```
### For Redhat distributions:
```sudo yum -y install kernel-devel kernel-headers```

or 

```sudo dnf -y install kernel-devel kernel-headers```
