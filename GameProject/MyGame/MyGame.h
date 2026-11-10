#pragma once
#include"TakoFramework.h"
#include"Vector2.h"

/// <summary>
/// 3D アクションゲームのメインクラス
/// </summary>
class MyGame : public Tako::TakoFramework
{
public: //メンバー関数
    void Initialize() override;

    void Finalize() override;

    void Update() override;

    void Draw() override;
};