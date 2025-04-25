#pragma once

enum class SceneState
{
    START,
    MAIN,
    END,
    PAUSE
};

class SceneStateBase
{
public:
    SceneStateBase(){}
    virtual ~SceneStateBase(){}

    SceneState GetState() { return m_state; }

    // 各種フラグ
    // 動作可能かどうか
    bool isMove = false;
    // ポーズ画面かどうか
    bool isPause = false;
    // ゲーム進行するかどうか
    bool isProgress = false;

protected:
    SceneState m_state;

};

