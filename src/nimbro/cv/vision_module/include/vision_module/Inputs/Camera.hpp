//Camera.hpp
// Created on: Apr 19, 2015
//     Author: Hafez Farazi <farazi@ais.uni-bonn.de>

#pragma once
#include <opencv2/opencv.hpp>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <fcntl.h>
#include <ros/ros.h>
#include <vision_module/Tools/Parameters.hpp>
#include <vision_module/Tools/MatPublisher.hpp>
#include <vision_module/Inputs/ICamera.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
using namespace cv;

/**
 * @class Camera
 * @brief This class is responsible for image acquisition process \n  As an output this class provides the raw image in /vision/takenImg topic
 **/
class Camera: public ICamera
{
private:
	VideoCapture *cap;
	char devStr[255];
	char paramStr[512];
	char paramDefStr[512];
	int devNumber;
	MatPublisher takenImg_pub;
public:
	inline Camera() :
			takenImg_pub("/vision/takenImg")
	{
		char buf[512];
		int count = readlink("/dev/eyeRight", buf, sizeof(buf));
		if (count >= 0)
		{
			buf[count] = '\0';
			devNumber=(int)(buf[count-1]-'0');
			ROS_INFO("Right Camera Found at %d",devNumber);
		}
		else
		{
			ROS_ERROR("No Camera Was Found");
		}

		sprintf(devStr, "/dev/video%d", devNumber);
		sprintf(paramStr,
				"v4l2ctrl -d /dev/video%d -l /nimbro/share/launch/config/vision/logitechConfig.txt ",
				devNumber);
		sprintf(paramDefStr,
				"v4l2ctrl -d /dev/video%d -l /nimbro/share/launch/config/vision/logitechConfig_default.txt ",
				devNumber);
		cap = new VideoCapture(devNumber); // open the camera
	}

	inline void DeInitCameraDevice()
	{
		char paramStrTmp[512];
		sprintf(paramStrTmp,
				"v4l2ctrl -d /dev/video%d -l /nimbro/share/launch/config/vision/logitechConfig_off.txt ",
				devNumber);
		if (-1 == system(paramStrTmp))
		{
			ROS_WARN_THROTTLE(10, "Cant find or set v4l2-ctrl values!");
		}

	}

	/*! @fn virtual void ~Camera()
	 *   @brief Destructor
	 */
	inline virtual ~Camera()
	{
		// the camera will be deinitialized
		delete cap;
	}
	inline bool IsReady()
	{
		return true;
	}
	inline bool IsDummy()
	{
		return false;
	}
	bool InitCameraDevice(bool);
	/**
	 * @fn TakeCapture
	 * @brief for capture new image
	 * @return How much this capture is reliable
	 */
	double TakeCapture();
};
