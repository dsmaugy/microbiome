/*
  ==============================================================================

    MicrobiomeEngine.cpp
    Created: 18 Mar 2024 11:27:56am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeEngine.h"

#define DEFAULT_COLONIES 3

MicrobiomeEngine::MicrobiomeEngine() 
{
    activeColonies = DEFAULT_COLONIES;
}


void MicrobiomeEngine::prepare(const EngineParams& params)
{
    blockSize = params.blockSize;

    ColonyParams colonyParams;
    colonyParams.blockSize = blockSize;

    for (int i = 0; i < MAX_COLONY; i++) {
        colony[i].prepare(colonyParams);
    }   
}