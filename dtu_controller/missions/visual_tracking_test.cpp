/** @file visual_tracking_test.cpp
 *  @version 3.3
 *  @date July, 2020
 *
 *  @brief
 *  Test missions for tracking a feature selected by the user
 *
 *  @copyright 2020 Ananda Nielsen, DTU. All rights reserved.
 *
 */

// Outside includes
#include <ros/ros.h>
#include "dji_sdk/dji_sdk.h"

#include <std_msgs/Bool.h>

#include <std_msgs/UInt8.h>

#include <geometry_msgs/Point.h>

// Own includes
#include "type_defines.h"
#include "general_functions.h"

float zError = 0;
float yError = 0;
int newError = 0;
int oldError = 0;
std_msgs::UInt8 trackerEnabled;

geometry_msgs::Twist currentPosition;

ros::Subscriber targetErrorSub_2;
ros::Subscriber currentPoseSub_2;
ros::Subscriber targetMsgSub;

// void targetErrorCallback( const geometry_msgs::Point pointMsg );
// void currentPoseCallback( const geometry_msgs::Twist twistMsg );

void targetErrorCallback_2( const geometry_msgs::Point pointMsg )
{
  yError = (float)pointMsg.y;
  zError = (float)pointMsg.z;
  // ROS_INFO("Got msg point");
  newError++;
}

void currentPoseCallback_2( const geometry_msgs::Twist twistMsg )
{
  currentPosition = twistMsg;

  // ROS_INFO("Got msg TWIST");
}

void targetMsgCallback( const std_msgs::UInt8 msg )
{
  
  trackerEnabled = msg;
  // ROS_INFO("Got msg TWIST");
}

int main(int argc, char** argv)
{
  // Setup the node
  ros::init(argc, argv, "visual_tracking_mission");
  ros::NodeHandle nh;

  ros::Duration(0.1).sleep();
  ControllerInterface controllerInterface;
  controllerInterface.init_interface( nh );

  ros::Duration(4).sleep();
  set_local_frame(nh);
  ros::Duration(1).sleep();

  targetErrorSub_2 = nh.subscribe<geometry_msgs::Point>("/visual_tracker/distance_error", 1, &targetErrorCallback_2);
  currentPoseSub_2 = nh.subscribe<geometry_msgs::Twist>("current_frame_pose", 1, &currentPoseCallback_2);

  targetMsgSub = nh.subscribe<std_msgs::UInt8>("/visual_tracker/target_tracking_msg", 1, &targetMsgCallback);

  ROS_INFO("Ready to take over. Waiting for command");
  
  /******************************************************************/

  // boost::shared_ptr<std_msgs::UInt8 const> sharedTrackEnable;
  //std_msgs::UInt8 trackerEnabled;
  trackerEnabled.data = 0;

  while(ros::ok()) 
  {
    // sharedTrackEnable = ros::topic::waitForMessage<std_msgs::UInt8>("/target_tracking_msg", nh);
    // if(sharedTrackEnable != NULL){
    //     trackerEnabled = *sharedTrackEnable;
    // }

    if( trackerEnabled.data == (uint8_t) 1 )
    {
      ROS_INFO("User selected a target. Starting tracking");
      if( set_control_authority(nh, true) ) ROS_INFO("Got control authorithy");
      else return 0;
      
      ROS_INFO("Stabilizing initial position (3s)");

      controllerInterface.holdPosition( nh );
      ros::Duration(0.01).sleep();
      controllerInterface.set_control_status( RUNNING );
      ros::Duration(3).sleep();
      break;
    }
    ros::Duration(0.01).sleep();
    ros::spinOnce();
  }

  float xTarget = currentPosition.linear.x;

  ROS_INFO("Starting the target tracking");
  while( ros::ok() )
  {
    // sharedTrackEnable = ros::topic::waitForMessage<std_msgs::UInt8>("/target_tracking_msg", nh);
    // if(sharedTrackEnable != NULL){
    //     trackerEnabled = *sharedTrackEnable;
    // }

    // ROS_INFO("Trak = %d", trackerEnabled.data);

    if( trackerEnabled.data >= (uint8_t) 2 )
    {
      ROS_INFO_ONCE("Starting the approach!");
      xTarget = -((float)trackerEnabled.data-1);
    }
    if( trackerEnabled.data == (uint8_t) 0 )
    {
      ROS_INFO("Stopping control");
      break;
    }
    // ROS_INFO("%d %d", newError, oldError);
    if( newError > oldError )
    {
      // controllerInterface.set_reference(xTarget, currentPosition.linear.y - yError, currentPosition.linear.z - zError, 0);
      controllerInterface.set_reference(xTarget, 0, 0, 0);
      // ROS_INFO("%f, %f, %f, %f",xTarget, currentPosition.linear.y - yError, currentPosition.linear.z - zError, 0.0);
      oldError = newError;
    }
    // ros::Duration(0.01).sleep();
    ros::spinOnce();
  }

  if( set_control_authority(nh, false) ) ROS_INFO("Released control authorithy");
  else return 0;

  return 0;
}
