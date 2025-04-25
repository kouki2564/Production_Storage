#pragma once
#include "SceneBase.h"

class DebugScene :
    public SceneBase
{
public:
    DebugScene();
    ~DebugScene();

    void InitScene();
    void UpdateScene();

private:
    // 画像データの準備
    void InitImage();

    void ObjectsUpdate();

    void CollisionUpdate();

    void CameraUpdate();
    

    // 配置するオブジェクト
    std::map<std::string, std::shared_ptr<ObjectBase>> m_objects;
    std::map<std::string, std::shared_ptr<Stage>> m_stage;

};

