/************************************************************************************
This source file is part of the Theora Video Playback Library
For latest info, see http://libtheoraplayer.sourceforge.net/
*************************************************************************************
Copyright (c) 2008-2010 Kresimir Spes (kreso@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
*************************************************************************************/
#include "../../../config.h"
#ifdef VIDEO
#ifdef _WIN32
#pragma warning( disable: 4251 ) // MSVC++
#endif
#include "TheoraWorkerThread.h"
#include "TheoraVideoManager.h"
#include "TheoraVideoClip.h"
#include "TheoraUtil.h"


TheoraWorkerThread::TheoraWorkerThread() : TheoraThread()
{
	mClip=NULL;
}

TheoraWorkerThread::~TheoraWorkerThread()
{

}

void TheoraWorkerThread::executeThread()
{
	mThreadRunning = true;
	while (mThreadRunning)
	{
		mClip=TheoraVideoManager::getSingleton().requestWork(this);
		if (!mClip)
		{
			_psleep(250);
			continue;
		}


		// if user requested seeking, do that then.
		if (mClip->mSeekPos >= 0) mClip->doSeek();

		mClip->decodeNextFrame();

		mClip->mAssignedWorkerThread=NULL;
		mClip=0;
		_psleep(1);
	}
}
#endif
