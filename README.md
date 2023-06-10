# LeoRoverUE5
This project implements a virtual model of a LeoRover in Unreal Engine 5 and enables communication via ROS 2.


## Prerequisite
This project is tesed under the following conditions:
- Ubutnu 22.04
- Unreal Engine 5.2.0
- ROS 2 Humble (binary install)

## Installation
### Unreal Engine 5
You can download pre-compiled UE5 for linux [here](https://www.unrealengine.com/en-US/linux). After unzipping the downloaded file,  set the environment variable for the UE5 root directory.
```
export UE5_ROOT=</path/to/ue5/root>
```
Clone this repo.
```
git clone https://github.com/metabotics-ai/LeoRoverUE5.git
```

### ROS 2 Humble
See [this](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html) for installing ROS 2 Humble.

### Plugins
This project depends on the following plugins for interfacing with ROS 2.
- [RapyutaSimulationPlugins](https://github.com/rapyuta-robotics/RapyutaSimulationPlugins)
- [rclUE](https://github.com/rapyuta-robotics/rclUE)

You should download *'RapyutaSimulationPlugins'* and *'rclUE'* plugins for this project. You can simply import these plugin using *'leorover.repos'* file via [vcstool](https://pypi.org/project/vcstool/) as below:
```
vcs import < leorover.repos
```

To compile the rclUE plugin in UE5.2.0, you will need to cherry-pick the following three commits in this order.
```
cd Plugins/rclUE
git cherry-pick e3de971f3a931a295f7440a96f7adb36ba42927d
git cherry-pick 502176ff2ce470a94842db8e4af7cf104f177390
git cherry-pick 1dd989596ac5cd5555e520a5b28524a1c1e4cfcc
```

## Build & Run
We are providing three scripts for compile and run this project. First, generate the 'Makefile' from 'uproject' file:
```
./update_project_files.sh
```
Then compile the project:
```
./make_editor.sh
```
Finally, the following command run the Unreal Editor:
```
./run_editor.sh
```

> Note: *'cleanup.sh'* is a convenient script for deleting project files.
> ```
> ./cleanup.sh
> ```

## Quick Start
We provide the 'Showcase' level for a quick start, and when you start the project, you will see the following scene.