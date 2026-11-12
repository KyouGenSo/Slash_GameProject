#include "BossAreaBounds.h"
#include "Script/Object/Boss/Boss.h"
#include "Source/Common/GameConst.h"

#include <algorithm>

namespace BossMovement {

    AreaBounds CalcStageBounds() {
        return {
            GameConst::StageXMin() + GameConst::kAreaMargin,
            GameConst::StageXMax() - GameConst::kAreaMargin,
            GameConst::StageZMin() + GameConst::kAreaMargin,
            GameConst::StageZMax() - GameConst::kAreaMargin,
        };
    }

    AreaBounds CalcAreaBounds(const Boss* boss) {
        AreaBounds bounds = CalcStageBounds();

        if (boss && boss->GetPhase() == 2) {
            bounds.xMin += GameConst::BossPhase2AreaSize();
            bounds.xMax -= GameConst::BossPhase2AreaSize();
            bounds.zMin += GameConst::BossPhase2AreaSize();
            bounds.zMax -= GameConst::BossPhase2AreaSize();
        }

        return bounds;
    }

    Tako::Vector3 ClampToBounds(const Tako::Vector3& position, const AreaBounds& bounds) {
        Tako::Vector3 result = position;
        result.x = std::clamp(result.x, bounds.xMin, bounds.xMax);
        result.z = std::clamp(result.z, bounds.zMin, bounds.zMax);
        result.y = position.y;
        return result;
    }

}
