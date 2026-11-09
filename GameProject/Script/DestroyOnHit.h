#pragma once
#include "Component.h"

#include <string>

/// <summary>
/// 同じ Actor の ColliderComponent が他の Actor に当たったら、この Actor を消す
/// </summary>
TCLASS()
class DestroyOnHit : public Tako::Component {
    GENERATED_BODY()

public: //メンバー関数
    void OnCollisionEnter(Tako::Actor* other) override;

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "空ならどの Actor でも消える。名前を書くと、その名前の Actor に当たったときだけ消える")
    std::string otherName_;
};
