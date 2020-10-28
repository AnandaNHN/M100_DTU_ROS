/** @file joy_roll_thrust_controller_node.h
 *  @version 3.3
 *  @date June, 2020
 *
 *  @brief
 *  demo sample of joy stick with rpyrate zvel control with ability to switch to roll thrust command scehme for wall interaction
 *
 *  @copyright 2020 Ananda Nielsen, DTU. All rights reserved.
 *
 */

#ifndef CONTACT_CONTROLLER_NODE_H
#define CONTACT_CONTROLLER_NODE_H

// ROS includes
#include <ros/ros.h>
#include <geometry_msgs/QuaternionStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <sensor_msgs/NavSatFix.h>
#include <std_msgs/UInt8.h>
#include "sensor_msgs/Imu.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Float32.h"

// DJI SDK includes
#include <dji_sdk/DroneTaskControl.h>
#include <dji_sdk/SDKControlAuthority.h>
#include <dji_sdk/DroneArmControl.h>
#include <dji_sdk/QueryDroneVersion.h>
#include <dji_sdk/SetLocalPosRef.h>


#include <tf/tf.h>
#include <sensor_msgs/Joy.h>

#define C_PI (double)3.141592653589793
#define DEG2RAD(DEG) ((DEG) * ((C_PI) / (180.0)))

/*!
 * @brief a bare bone state machine to track the stage of the mission
 */


void controlCallback( const sensor_msgs::Joy joy_msg );
void attitudeCallback( const geometry_msgs::QuaternionStamped quaternion );
void imuCallback( const sensor_msgs::Imu imuMsgs );
void rodCallback(const std_msgs::Bool::ConstPtr& msg);

void timerCallback( const ros::TimerEvent& );


bool obtain_control();
bool release_control();
bool arm_motors();
bool disarm_motors();

#endif // DEMO_FLIGHT_CONTROL_H
