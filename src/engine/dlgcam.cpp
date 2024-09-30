// dlgcam.cpp - Dialog Camera package
//
// Author: Tim Stark
// Date: Sep 25, 2024

#include "main/core.h"
#include "api/ofsapi.h"
#include "api/graphics.h"
#include "main/app.h"
#include "main/guimgr.h"
#include "universe/psystem.h"
#include "engine/player.h"
#include "engine/celestial.h"
#include "engine/dlgcam.h"

DialogCamera::DialogCamera(cstr_t &name)
: GUIElement(name, typeid(DialogCamera))
{
    bEnabled = false;
}

void DialogCamera::show()
{

    cchar_t *tabs[] = { "Target", "Track" };

    void (DialogCamera::* funcs[])() = {
        &DialogCamera::showTargetTab,
        &DialogCamera::showTrackTab
    };

    ofsLogger->debug("show() here\n");

    player = ofsAppCore->getPlayer();
    cam = player->getCamera();

    ImGui::SetNextWindowPos(ImVec2(380, 310), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(480, 340), ImGuiCond_FirstUseEver);
    ImGui::Begin("Player Control", &bEnabled);
    ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tabFlags)) {
        for (size_t idx = 0; idx < ARRAY_SIZE(tabs); idx++) {
            if (ImGui::BeginTabItem(tabs[idx])) {
                (this->*funcs[idx])();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void DialogCamera::addCelestial(const Celestial *cbody)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (selectedTarget == cbody->getsName())
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    if (cbody->hasSecondaries()) {
        bool nodeOpen = ImGui::TreeNodeEx(cbody->getcName(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selectedTarget = cbody->getsName();
        if (nodeOpen) {
            for (int idx = 0; idx < cbody->getSecondarySize(); idx++)
                addCelestial(cbody->getSecondary(idx));
            ImGui::TreePop();
        }
    } else {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(cbody->getcName(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selectedTarget = cbody->getsName();
    }
}

void DialogCamera::showTargetTab()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    static float sz1 = 0.0;
    float sz2;

    ImGui::BeginChild("##Target", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);

    pSystem *sys = player->getSystem();
    for (int idx = 0; idx < sys->getStarsSize(); idx++)
        addCelestial(sys->getStar(idx));

    ImGui::Text("Target: %s", selectedTarget.c_str());
    ImVec2 szButton(ImVec2(ImGui::GetContentRegionAvail().x, 20));
    if (ImGui::Button("Apply", szButton)) {
        Celestial *cbody = dynamic_cast<Celestial *>(sys->find(selectedTarget));
        Celestial *star = dynamic_cast<Celestial *>(cbody->getStar());
        player->attach(cbody, camTargetSync, star);
        player->look(cbody);
        cam->setPosition({ 0, 0, cbody->getRadius() * 4.0});
    }
    ImGui::EndChild();
}

void DialogCamera::showTrackTab()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    static float sz1 = 0.0;
    float sz2;

    // ImGui::Splitter(true, 0.5f, 8.0f, &sz1, &sz2, 8, 8, ImGui::GetContentRegionAvail().y);
    // ImGui::BeginChild("ChildL", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);
    // ImGui::Text("Moveable Target Modes");
    // ImVec2 szButton(ImVec2(ImGui::GetContentRegionAvail().x, 20));
    // if (ImGui::Button("Target Relative", szButton)) {

    // }
    // if (ImGui::Button("Target Unlocked", szButton)) {

    // }
    // if (ImGui::Button("Target Sync", szButton)) {

    // }
    // if (ImGui::Button("Global Frame", szButton)) {

    // }
    // ImGui::EndChild();

    // ImGui::SameLine();
    // ImGui::BeginChild("ChildR", ImVec2(sz1, ImGui::GetContentRegionAvail().y), false, windowFlags);



    // ImGui::EndChild();
}