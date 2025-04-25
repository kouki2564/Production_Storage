#pragma once
#include "SceneStateBase.h"
class SceneEnd :
    public SceneStateBase
{
public:
    SceneEnd()
    {
        m_state = SceneState::END;
        isMove = false;
        isPause = false;
        isProgress = true;
    }
};

