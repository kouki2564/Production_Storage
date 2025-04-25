#pragma once
#include "SceneStateBase.h"
class ScenePause :
    public SceneStateBase
{
public:
    ScenePause()
    {
        m_state = SceneState::PAUSE;
        isMove = false;
        isPause = true;
        isProgress = false;
    }
};

