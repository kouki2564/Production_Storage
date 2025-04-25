#pragma once
#include "SceneStateBase.h"
class SceneMain :
    public SceneStateBase
{
public:
    SceneMain()
    {
        m_state = SceneState::MAIN;
        isMove = true;
        isPause = false;
        isProgress = true;
    }
};

