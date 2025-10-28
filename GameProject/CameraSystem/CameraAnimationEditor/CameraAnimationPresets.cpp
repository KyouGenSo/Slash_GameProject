#ifdef _DEBUG

#include "CameraAnimationPresets.h"
#include <imgui.h>
#include <json.hpp>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <set>

using json = nlohmann::json;

CameraAnimationPresets::CameraAnimationPresets() {
    // 初期化はInitializeで行う
}

CameraAnimationPresets::~CameraAnimationPresets() {
    builtinPresets_.clear();
    customPresets_.clear();
}

void CameraAnimationPresets::Initialize() {
    InitializeBuiltinPresets();
}

void CameraAnimationPresets::InitializeBuiltinPresets() {
    // フェードインプリセット
    {
        Preset preset;
        preset.name = "Fade In";
        preset.description = "Smooth fade in from black";
        preset.type = PresetType::FADE_IN;
        preset.keyframes = GenerateFadeIn();
        preset.duration = 2.0f;
        preset.isUserDefined = false;
        preset.category = "Basic";
        preset.tags = "fade,transition,basic";
        builtinPresets_[PresetType::FADE_IN] = preset;
    }

    // ズームインプリセット
    {
        Preset preset;
        preset.name = "Zoom In";
        preset.description = "Zoom in to target";
        preset.type = PresetType::ZOOM_IN;
        preset.keyframes = GenerateZoomIn();
        preset.duration = 1.5f;
        preset.isUserDefined = false;
        preset.category = "Basic";
        preset.tags = "zoom,basic";
        builtinPresets_[PresetType::ZOOM_IN] = preset;
    }

    // オービット左プリセット
    {
        Preset preset;
        preset.name = "Orbit Left";
        preset.description = "Orbit camera to the left";
        preset.type = PresetType::ORBIT_LEFT;
        preset.keyframes = GenerateOrbit(true);
        preset.duration = 3.0f;
        preset.isUserDefined = false;
        preset.category = "Camera Movement";
        preset.tags = "orbit,rotation,movement";
        builtinPresets_[PresetType::ORBIT_LEFT] = preset;
    }

    // 軽いシェイクプリセット
    {
        Preset preset;
        preset.name = "Light Shake";
        preset.description = "Light camera shake effect";
        preset.type = PresetType::SHAKE_LIGHT;
        preset.keyframes = GenerateShake(0.2f);
        preset.duration = 0.5f;
        preset.isUserDefined = false;
        preset.category = "Effects";
        preset.tags = "shake,effect,impact";
        builtinPresets_[PresetType::SHAKE_LIGHT] = preset;
    }
}

void CameraAnimationPresets::Draw() {
    ImGui::Text("Animation Presets");
    ImGui::Separator();

    // フィルター
    ImGui::InputText("Filter", filterText_, sizeof(filterText_));

    // カテゴリー選択
    std::vector<std::string> categories = GetCategories();
    if (ImGui::BeginCombo("Category", selectedCategory_.c_str())) {
        for (const auto& cat : categories) {
            bool isSelected = (selectedCategory_ == cat);
            if (ImGui::Selectable(cat.c_str(), isSelected)) {
                selectedCategory_ = cat;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    // プリセットリストとプレビューを左右に分割
    float availWidth = ImGui::GetContentRegionAvail().x;
    float listWidth = availWidth * 0.5f;

    // 左側：プリセットリスト
    ImGui::BeginChild("PresetList", ImVec2(listWidth - 5, 300), true);
    DrawPresetList();
    ImGui::EndChild();

    ImGui::SameLine();

    // 右側：プレビュー
    ImGui::BeginChild("PresetPreview", ImVec2(0, 300), true);
    DrawPresetPreview();
    ImGui::EndChild();

    ImGui::Separator();

    // カスタムプリセット作成
    if (ImGui::CollapsingHeader("Create Custom Preset")) {
        DrawCustomPresetCreator();
    }
}

void CameraAnimationPresets::DrawPresetList() {
    ImGui::Text("Presets:");
    ImGui::Separator();

    // ビルトインプリセット
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "Built-in");
    for (auto& [type, preset] : builtinPresets_) {
        // カテゴリーフィルター
        if (selectedCategory_ != "All" && preset.category != selectedCategory_) {
            continue;
        }

        // テキストフィルター
        if (strlen(filterText_) > 0) {
            std::string filter = filterText_;
            std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
            std::string name = preset.name;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::string tags = preset.tags;
            std::transform(tags.begin(), tags.end(), tags.begin(), ::tolower);

            if (name.find(filter) == std::string::npos &&
                tags.find(filter) == std::string::npos) {
                continue;
            }
        }

        bool isSelected = (selectedPreset_ == &preset);
        if (ImGui::Selectable(preset.name.c_str(), isSelected)) {
            selectedPreset_ = &preset;
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", preset.description.c_str());
        }
    }

    // カスタムプリセット
    if (!customPresets_.empty()) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.7f, 1.0f), "Custom");

        for (auto& [name, preset] : customPresets_) {
            // フィルター処理（同上）
            if (selectedCategory_ != "All" && preset.category != selectedCategory_) {
                continue;
            }

            bool isSelected = (selectedPreset_ == &preset);
            if (ImGui::Selectable(name.c_str(), isSelected)) {
                selectedPreset_ = &preset;
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", preset.description.c_str());
            }
        }
    }
}

void CameraAnimationPresets::DrawPresetPreview() {
    ImGui::Text("Preview:");
    ImGui::Separator();

    if (!selectedPreset_) {
        ImGui::TextDisabled("No preset selected");
        return;
    }

    // プリセット情報
    ImGui::Text("Name: %s", selectedPreset_->name.c_str());
    ImGui::Text("Duration: %.2f sec", selectedPreset_->duration);
    ImGui::Text("Keyframes: %zu", selectedPreset_->keyframes.size());
    ImGui::Text("Category: %s", selectedPreset_->category.c_str());

    ImGui::Separator();
    ImGui::TextWrapped("Description: %s", selectedPreset_->description.c_str());

    ImGui::Separator();

    // キーフレームリスト
    ImGui::Text("Keyframes:");
    if (ImGui::BeginChild("KeyframePreview", ImVec2(0, 100), true)) {
        for (size_t i = 0; i < selectedPreset_->keyframes.size(); ++i) {
            const auto& kf = selectedPreset_->keyframes[i];
            ImGui::Text("%zu: %.2fs - Pos(%.1f, %.1f, %.1f)",
                i, kf.time,
                kf.position.x, kf.position.y, kf.position.z);
        }
    }
    ImGui::EndChild();

    // 適用ボタン
    if (ImGui::Button("Apply Preset")) {
        // エディターで処理される
    }

    ImGui::SameLine();

    // カスタムプリセットの場合は削除ボタン
    if (selectedPreset_->isUserDefined) {
        if (ImGui::Button("Delete")) {
            RemoveCustomPreset(selectedPreset_->name);
            selectedPreset_ = nullptr;
        }
    }
}

void CameraAnimationPresets::DrawCustomPresetCreator() {
    ImGui::InputText("Name", newPresetName_, sizeof(newPresetName_));
    ImGui::InputTextMultiline("Description", newPresetDescription_, sizeof(newPresetDescription_));

    ImGui::Separator();

    // パラメータ設定
    ImGui::DragFloat("Duration", &presetDuration_, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("Intensity", &presetIntensity_, 0.01f, 0.0f, 2.0f);
    ImGui::DragInt("Steps", &presetSteps_, 1, 2, 100);

    // プリセットタイプ選択
    static int selectedType = 0;
    const char* typeNames[] = {
        "Linear Motion", "Circular Motion", "Shake", "Custom"
    };
    ImGui::Combo("Type", &selectedType, typeNames, 4);

    if (ImGui::Button("Generate")) {
        // 簡単な生成ロジック
        tempKeyframes_.clear();

        for (int i = 0; i <= presetSteps_; ++i) {
            float t = static_cast<float>(i) / presetSteps_;
            CameraKeyframe kf;
            kf.time = t * presetDuration_;

            switch (selectedType) {
                case 0: // Linear Motion
                    kf.position = Vector3(t * 10.0f, 0, 0);
                    kf.rotation = Vector3(0, 0, 0);
                    break;

                case 1: // Circular Motion
                    kf.position = Vector3(
                        std::cos(t * 6.28318f) * 5.0f,
                        0,
                        std::sin(t * 6.28318f) * 5.0f
                    );
                    kf.rotation = Vector3(0, t * 6.28318f, 0);
                    break;

                case 2: // Shake
                    kf.position = Vector3(
                        (rand() / float(RAND_MAX) - 0.5f) * presetIntensity_,
                        (rand() / float(RAND_MAX) - 0.5f) * presetIntensity_,
                        0
                    );
                    kf.rotation = Vector3(0, 0, 0);
                    break;

                case 3: // Custom
                    kf.position = Vector3(0, 0, 0);
                    kf.rotation = Vector3(0, 0, 0);
                    break;
            }

            kf.fov = 0.45f;
            kf.interpolation = CameraKeyframe::InterpolationType::LINEAR;
            tempKeyframes_.push_back(kf);
        }
    }

    if (!tempKeyframes_.empty()) {
        ImGui::Text("Generated %zu keyframes", tempKeyframes_.size());

        if (ImGui::Button("Save as Custom Preset")) {
            if (strlen(newPresetName_) > 0) {
                AddCustomPreset(newPresetName_, tempKeyframes_);
                tempKeyframes_.clear();
                newPresetName_[0] = '\0';
                newPresetDescription_[0] = '\0';
            }
        }
    }
}

std::vector<CameraKeyframe> CameraAnimationPresets::GenerateFadeIn() const {
    std::vector<CameraKeyframe> keyframes;

    // 開始位置（遠く）
    CameraKeyframe kf1;
    kf1.time = 0.0f;
    kf1.position = Vector3(0, 5, 20);
    kf1.rotation = Vector3(-0.1f, 0, 0);
    kf1.fov = 0.6f;
    kf1.interpolation = CameraKeyframe::InterpolationType::EASE_OUT;
    keyframes.push_back(kf1);

    // 終了位置（近く）
    CameraKeyframe kf2;
    kf2.time = 2.0f;
    kf2.position = Vector3(0, 2, 10);
    kf2.rotation = Vector3(-0.2f, 0, 0);
    kf2.fov = 0.45f;
    kf2.interpolation = CameraKeyframe::InterpolationType::LINEAR;
    keyframes.push_back(kf2);

    return keyframes;
}

std::vector<CameraKeyframe> CameraAnimationPresets::GenerateZoomIn() const {
    std::vector<CameraKeyframe> keyframes;

    CameraKeyframe kf1;
    kf1.time = 0.0f;
    kf1.position = Vector3(0, 0, 10);
    kf1.rotation = Vector3(0, 0, 0);
    kf1.fov = 0.8f;
    kf1.interpolation = CameraKeyframe::InterpolationType::EASE_IN_OUT;
    keyframes.push_back(kf1);

    CameraKeyframe kf2;
    kf2.time = 1.5f;
    kf2.position = Vector3(0, 0, 5);
    kf2.rotation = Vector3(0, 0, 0);
    kf2.fov = 0.4f;
    kf2.interpolation = CameraKeyframe::InterpolationType::LINEAR;
    keyframes.push_back(kf2);

    return keyframes;
}

std::vector<CameraKeyframe> CameraAnimationPresets::GenerateOrbit(bool leftDirection) const {
    std::vector<CameraKeyframe> keyframes;
    const int steps = 20;
    const float radius = 10.0f;

    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float angle = t * 3.14159f * 2.0f;  // Full circle

        if (!leftDirection) {
            angle = -angle;
        }

        CameraKeyframe kf;
        kf.time = t * 3.0f;
        kf.position = Vector3(
            std::sin(angle) * radius,
            2.0f,
            std::cos(angle) * radius
        );
        kf.rotation = Vector3(
            -0.1f,
            angle + 3.14159f,  // Look at center
            0
        );
        kf.fov = 0.45f;
        kf.interpolation = CameraKeyframe::InterpolationType::LINEAR;
        keyframes.push_back(kf);
    }

    return keyframes;
}

std::vector<CameraKeyframe> CameraAnimationPresets::GenerateShake(float intensity) const {
    std::vector<CameraKeyframe> keyframes;
    const int steps = 10;
    const float duration = 0.5f;

    // 初期位置
    CameraKeyframe kf0;
    kf0.time = 0.0f;
    kf0.position = Vector3(0, 0, 10);
    kf0.rotation = Vector3(0, 0, 0);
    kf0.fov = 0.45f;
    kf0.interpolation = CameraKeyframe::InterpolationType::LINEAR;
    keyframes.push_back(kf0);

    // シェイク
    for (int i = 1; i < steps; ++i) {
        float t = static_cast<float>(i) / steps;

        CameraKeyframe kf;
        kf.time = t * duration;
        kf.position = Vector3(
            (rand() / float(RAND_MAX) - 0.5f) * intensity,
            (rand() / float(RAND_MAX) - 0.5f) * intensity * 0.5f,
            10.0f + (rand() / float(RAND_MAX) - 0.5f) * intensity * 0.2f
        );
        kf.rotation = Vector3(
            (rand() / float(RAND_MAX) - 0.5f) * intensity * 0.05f,
            (rand() / float(RAND_MAX) - 0.5f) * intensity * 0.05f,
            (rand() / float(RAND_MAX) - 0.5f) * intensity * 0.02f
        );
        kf.fov = 0.45f;
        kf.interpolation = CameraKeyframe::InterpolationType::LINEAR;
        keyframes.push_back(kf);
    }

    // 終了位置（初期位置に戻る）
    CameraKeyframe kfEnd = kf0;
    kfEnd.time = duration;
    keyframes.push_back(kfEnd);

    return keyframes;
}

std::vector<CameraKeyframe> CameraAnimationPresets::GetPreset(PresetType type) const {
    auto it = builtinPresets_.find(type);
    if (it != builtinPresets_.end()) {
        return it->second.keyframes;
    }
    return {};
}

std::vector<CameraKeyframe> CameraAnimationPresets::GetPresetByName(const std::string& name) const {
    // ビルトインプリセットから検索
    for (const auto& [type, preset] : builtinPresets_) {
        if (preset.name == name) {
            return preset.keyframes;
        }
    }

    // カスタムプリセットから検索
    auto it = customPresets_.find(name);
    if (it != customPresets_.end()) {
        return it->second.keyframes;
    }

    return {};
}

std::vector<CameraKeyframe> CameraAnimationPresets::GetSelectedPreset() const {
    if (selectedPreset_) {
        return selectedPreset_->keyframes;
    }
    return {};
}

void CameraAnimationPresets::AddCustomPreset(const std::string& name, const std::vector<CameraKeyframe>& keyframes) {
    Preset preset;
    preset.name = name;
    preset.description = newPresetDescription_;
    preset.type = PresetType::USER_DEFINED;
    preset.keyframes = keyframes;
    preset.duration = keyframes.empty() ? 0 : keyframes.back().time;
    preset.isUserDefined = true;
    preset.category = "Custom";
    preset.tags = "custom,user";

    customPresets_[name] = preset;
}

bool CameraAnimationPresets::RemoveCustomPreset(const std::string& name) {
    auto it = customPresets_.find(name);
    if (it != customPresets_.end()) {
        customPresets_.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> CameraAnimationPresets::GetCategories() const {
    std::vector<std::string> categories = { "All" };
    std::set<std::string> uniqueCategories;

    for (const auto& [type, preset] : builtinPresets_) {
        uniqueCategories.insert(preset.category);
    }
    for (const auto& [name, preset] : customPresets_) {
        uniqueCategories.insert(preset.category);
    }

    for (const auto& cat : uniqueCategories) {
        categories.push_back(cat);
    }

    return categories;
}

bool CameraAnimationPresets::SavePresets(const std::string& filepath) const {
    // TODO: JSONシリアライゼーション実装
    return false;
}

bool CameraAnimationPresets::LoadPresets(const std::string& filepath) {
    // TODO: JSONデシリアライゼーション実装
    return false;
}

#endif // _DEBUG