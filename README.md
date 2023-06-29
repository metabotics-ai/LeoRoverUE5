# LeoRoverUE5
This project implements a virtual model of a LeoRover in Unreal Engine 5 and enables communication via ROS 2.

![leorover_showcase](./Resources/leorover_showcase.gif)

## Prerequisite
This project is tesed under the following conditions:
- Ubutnu 22.04
- Unreal Engine 5.2.x
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

## ROS Topics Published

| ROS Topic            | Interface                                                                                                                           |
|----------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| `/image_raw`             | [sensor_msgs/Image](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Image.msg)                         |
| `/odom`  | [nav_msgs/Odometry](https://github.com/ros2/common_interfaces/blob/humble/nav_msgs/msg/Odometry.msg)                    |
| `/clock`        | [rosgraph_msgs/Clock](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/PointCloud2.msg)                    |


## ROS 2 Topics Subscribed

| ROS Topic           | Interface                                                                                                                         |
|---------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| `/cmd_vel`        | [geometry_msgs/Twist](https://github.com/ros2/common_interfaces/blob/humble/geometry_msgs/msg/Twist.msg)                  |

