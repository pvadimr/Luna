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
#include "TheoraAudioInterface.h"

TheoraAudioInterface::TheoraAudioInterface(TheoraVideoClip* owner,int nChannels,int freq)
{
	mFreq=freq;
	mNumChannels=nChannels;
	mClip=owner;
}

TheoraAudioInterface::~TheoraAudioInterface()
{

}
#endif
