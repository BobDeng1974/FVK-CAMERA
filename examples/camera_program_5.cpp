/*********************************************************************************
created:	2017/07/28   02:47AM
filename: 	camera_program_5.cpp
file base:	camera_program_5
file ext:	cpp
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	Simplest program to run a camera device. It uses OpenCV window to show
the grabbed image frames.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include <fvk/camera/fvkCamera.h>
#include <fvk/camera/fvkCameraList.h>
#include <fvk/camera/fvkImageProcessing.h>

using namespace R3D;

// a derived class in order to extend the fvkCamera class. 
class MyCamera : public fvkCamera
{
public:
	// Default constructor.
	// first argument is the id of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// second argument is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	MyCamera(const int _device_index, const cv::Size& _frame_size, const int _api = static_cast<int>(cv::VideoCaptureAPIs::CAP_ANY)) : 
		fvkCamera(_device_index, _frame_size, _api)
	{
	}

protected:
	// An overridden function that is being calling in the continuous running camera thread.
	// _frame is the grabbed frame.
	virtual void present(cv::Mat& _frame) override
	{
		auto m = _frame.clone();			// always create a clone to process the frame.
		 cv::cvtColor(m, m, CV_BGR2GRAY);	// just doing the simple image processing that converts to gray-scale image.
		cv::imshow("FVK Camera", m);		// show the given frame in OpenCV window.
		_frame = m;
	}
};

int main()
{
	// create a list of the cameras.
	fvkCameraList<MyCamera> clist;

	// add a new camera in the list by specifying the camera id and it's desired resolution.
	auto cam = clist.add(0, cv::Size(640, 480));

	// try to connect the camera device and initialize it.
	if (cam->connect())
	{
		// start the camera threads (frame grabbing + processing thread).
		if (cam->start())
		{
			std::cout << "camera has started!" << std::endl;
		}
	}

	cam->imageProcessing().setNegativeModeEnabled(true);	// do some image processing on the captured frame.

	// create a window using OpenCV.
	cv::namedWindow("FVK Camera");

	// OpenCV event loop that will prevent to exit the main loop.
	cv::waitKey(0);

	// stop all threads and disconnect the device.
	cam->disconnect();

	return 1;
}