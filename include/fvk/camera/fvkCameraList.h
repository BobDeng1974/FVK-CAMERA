#pragma once
#ifndef fvkCameraList_h__
#define fvkCameraList_h__

/*********************************************************************************
created:	2017/07/31   03:32PM
filename: 	fvkCameraList.h
file base:	fvkCameraList
file ext:	h
author:		Furqan Ullah (Post-doc, Ph.D.)
website:    http://real3d.pk
CopyRight:	All Rights Reserved

purpose:	class that gives a list of camera devices with add, remove, find features.

/**********************************************************************************
*	Fast Visualization Kit (FVK)
*	Copyright (C) 2017 REAL3D
*
* This file and its content is protected by a software license.
* You should have received a copy of this license with this file.
* If not, please contact Dr. Furqan Ullah immediately:
**********************************************************************************/

#include "fvkExport.h"
#include <vector>
#include <algorithm>

namespace R3D
{

template <typename CAMERA>
class FVK_EXPORT fvkCameraList
{

public:
	// Description:
	// Default constructor to create a camera object.
	fvkCameraList()
	{
	}
	// Description:
	// Default destructor will call clear() function which stops all the running threads and releases all the camera.
	~fvkCameraList()
	{
		clear();
	}
	// Description:
	// Function to remove all cameras from the list.
	// It stops all the running threads and releases all the camera.
	void clear()
	{
		for (auto& it : m_cameras)
			delete it;
		m_cameras.clear();
	}
	// Description:
	// Function to add a new camera to the list. 
	// It returns NULL if a camera with the same device id is already in the list.
	// _device is the id of the opened video capturing device (i.e. a camera index).
	// If there is a single camera connected, just pass 0.
	// In case of multiple cameras, try to pass 1 or 2 or 3, so on...
	// _resolution is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	auto add(int _device_id, cv::Size _resolution = cv::Size(-1, -1))
	{
		if (getBy(_device_id))
			return static_cast<CAMERA*>(nullptr);

		CAMERA* p = new CAMERA(_device_id, _resolution);
		m_cameras.push_back(p);
		return p;
	}
	// Description:
	// Function to add a new camera to the list. 
	// It returns NULL if a camera with the same device id is already in the list.
	// _video_file is the location of the video file (eg. video.avi) or image sequence 
	// (eg. img_%02d.jpg, which will read samples like img_00.jpg, img_01.jpg, img_02.jpg, ...)
	// _resolution is the desired camera frame width and height.
	// resolution Size(-1, -1) will do the auto-selection for the frame's width and height.
	auto add(const std::string& _video_file, cv::Size _resolution = cv::Size(-1, -1), int _api = cv::VideoCaptureAPIs::CAP_ANY)
	{
		CAMERA* p = new CAMERA(_video_file, _resolution, _api);
		m_cameras.push_back(p);
		return p;
	}
	// Description:
	// Function to add a new camera to the list. 
	// It returns NULL if a camera with the same device id is already in the list.
	// Function to add a camera object by specifying the camera and processing threads.
	// This function is created for the derived classes of both fvkCameraThread and fvkCameraProcessingThread.
	auto add(CAMERA* _p)
	{
		if (!_p) return nullptr;

		if (getBy(_p->getDeviceIndex()))
			return static_cast<CAMERA*>(nullptr);

		m_cameras.push_back(_p);
		return _p;
	}

	// Description:
	// Function to get a pointer to camera by index of the camera list.
	auto get(const  std::size_t _index) const
	{
		if (_index < 0 || _index >= m_cameras.size())
			return nullptr;
		return m_cameras[_index]; 
	}
	// Description:
	// Function to get a pointer to camera device by it's id.
	auto getBy(const int _device_index) const
	{
		auto it = std::find_if(m_cameras.begin(), m_cameras.end(),
			[&_device_index](const CAMERA* _p)
		{
			return _p->getDeviceIndex() == _device_index;
		});
		if (it != m_cameras.end())
			return (*it);
		return static_cast<CAMERA*>(nullptr);
	}

	// Description:
	// Function to remove a camera from the list.
	// It returns true on success.
	auto remove(const int _device_index)
	{
		auto p = getBy(_device_index);
		if (p)
		{
			m_cameras.erase(std::remove(m_cameras.begin(), m_cameras.end(), p), m_cameras.end());
			delete p;
			p = nullptr;
			return true;
		}
		return false;
	}
	// Description:
	// Function to get the total number of cameras in the list.
	auto getSize() const { return m_cameras.size(); }

private:
	std::vector<CAMERA*> m_cameras;
};

}

#endif // fvkCameraList_h__