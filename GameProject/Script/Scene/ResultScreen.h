#pragma once
#include "Script.h"

#include <string>

/// <summary>
/// リザルト画面の進行。同じ Actor の UILayoutComponent の題字の画像を差し替え、入力で次のシーンへ移る。
/// クリアとゲームオーバーは同じレイアウトを使い、題字の画像だけをシーンごとに変える
/// </summary>
TCLASS()
class ResultScreen : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    void BeginPlay() override;

    void Tick(float deltaTime) override;

private: //メンバー変数
    TPROPERTY(Edit, Asset = "Texture", Tooltip = "題字の画像（空ならレイアウトのまま）")
    std::string titleTexture_;
    TPROPERTY(Edit, Tooltip = "画像を差し替える要素の名前")
    std::string titleElement_ = "Title";
    TPROPERTY(Edit, Asset = "Scene")
    std::string nextScene_    = "title";
};
