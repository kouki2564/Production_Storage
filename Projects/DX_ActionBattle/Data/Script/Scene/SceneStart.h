#pragma once
#include "SceneStateBase.h"
class SceneStart :
    public SceneStateBase
{
public:
    SceneStart()
    {
        m_state = SceneState::START;
        isMove = false;
        isPause = false;
        isProgress = true;
    }
};

