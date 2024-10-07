// dlgcam.cpp - Dialog Camera package
//
// Author: Tim Stark
// Date: Sep 25, 2024

#include "main/core.h"
#include "api/ofsapi.h"
#include "api/graphics.h"
#include "main/app.h"
#include "main/guimgr.h"
#include "universe/body.h"
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

    cchar_t *tabs[] = { "Target", "Track", "Ground" };

    void (DialogCamera::* funcs[])() = {
        &DialogCamera::showTargetTab,
        &DialogCamera::showTrackTab,
        &DialogCamera::showGroundTab
    };

    Celestial *cbody = nullptr;

    player = ofsAppCore->getPlayer();
    cam = player->getCamera();

    // extMode = player->getCameraMode();
    // cbody = player->getReferenceObject();
    // if (cbody != nullptr)
    //     selectedTarget = cbody->getsName();
    // cbody = player->getSyncObject();
    // if (cbody != nullptr)
    //     selectedSyncTarget = cbody->getsName();

    // extMode = camGlobalFrame;
    // selectedTarget = "";
    // selectedSyncTarget = "";
    // selectedSite = "";
    // selectedPlanetSite = nullptr;

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

    // ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
    // ImGui::BeginChild("##Bottom", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);

    // ImGui::Separator();
    // ImGui::Text("Target: %s", selectedTarget.c_str());
    // if (ImGui::Button("Apply")) {

    // }

    // ImGui::EndChild();

    ImGui::End();
}

void DialogCamera::addCelestial(const Celestial *cbody, str_t &selected)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (selected == cbody->getsName())
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    if (cbody->hasSecondaries()) {
        bool nodeOpen = ImGui::TreeNodeEx(cbody->getcName(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selected = cbody->getsName();
        if (nodeOpen) {
            for (int idx = 0; idx < cbody->getSecondarySize(); idx++)
                addCelestial(cbody->getSecondary(idx), selected);
            ImGui::TreePop();
        }
    } else {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(cbody->getcName(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selected = cbody->getsName();
    }
}

void DialogCamera::showApplyButton()
{
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::Text("Target: %s", selectedTarget.c_str());
    if (extMode == camTargetSync)
        ImGui::Text("Sync: %s", selectedSyncTarget.c_str());
    if (!selectedSite.empty())
        ImGui::Text("Ground: %s (%s)", selectedSite.c_str(), selectedPlanetSite->getcName());
    ImVec2 szButton(ImVec2(ImGui::GetContentRegionAvail().x, 20));
    if (ImGui::Button("Apply")) {
        if (!selectedSite.empty()) {
            GroundPOI *poi = nullptr;
            poiList_t poiList = selectedPlanetSite->getGroundPOI();
            for (int idx = 0; idx < poiList.size(); idx++)
                if (selectedSite == poiList[idx]->name) {
                    poi = poiList[idx];
                    break;
                }
            player->setGroundObserver(selectedPlanetSite, poi->loc, poi->dir);
        } else {
            Celestial *sync = nullptr;
            Celestial *cbody = dynamic_cast<Celestial *>(sys->find(selectedTarget));
            if (extMode == camTargetSync)
                Celestial *sync = dynamic_cast<Celestial *>(sys->find(selectedSyncTarget));
            cam->setPosition({ 0, 0, cbody->getRadius() * 4.0});
            player->attach(cbody, extMode, sync);
            player->look(cbody);
        }
    }
}

void DialogCamera::showTargetTab()
{
    sz1 = 0.0;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("##Target", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);

    ImGui::Text("Target Object:");
    sys = player->getSystem();
    for (int idx = 0; idx < sys->getStarsSize(); idx++)
        addCelestial(sys->getStar(idx), selectedTarget);

    if (!selectedTarget.empty()) {
        selectedSite = "";
        selectedPlanetSite = nullptr;
    }

    showApplyButton();

    ImGui::EndChild();
}

void DialogCamera::showTrackTab()
{
    sz1 = 0.0;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::BeginChild("ChildL", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);
    ImGui::Text("Moveable Target Modes");
    ImVec2 szButton(ImVec2(ImGui::GetContentRegionAvail().x, 20));
    if (ImGui::Button("Target Relative", szButton)) {
        extMode = camTargetRelative;
    }
    if (ImGui::Button("Target Unlocked", szButton)) {
        extMode = camTargetUnlocked;
    }
    if (ImGui::Button("Target Sync", szButton)) {
        extMode = camTargetSync;
    }
    if (ImGui::Button("Global Frame", szButton)) {
        extMode = camGlobalFrame;
    }

    if (extMode == camTargetSync) {
        ImGui::Text("Target Sync Object:");
        sys = player->getSystem();
        for (int idx = 0; idx < sys->getStarsSize(); idx++)
            addCelestial(sys->getStar(idx), selectedSyncTarget);
    }

    sys = player->getSystem();
    showApplyButton();

    ImGui::EndChild();
}

void DialogCamera::showGroundTab()
{
    // ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

    // ImGui::BeginChild("##Ground", ImVec2::(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);
    CelestialPlanet *cbody = dynamic_cast<CelestialPlanet *>(sys->find(selectedTarget));
    if (cbody == nullptr)
        return;

    sz1 = 0.0;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("##Ground", ImVec2(sz1, ImGui::GetContentRegionAvail().y), true, windowFlags);

    poiList_t &poiList = cbody->getGroundPOI();
    for (int idx = 0; idx < poiList.size(); idx++)
    {
        const GroundPOI *poi = poiList[idx];
        bool isSelected = selectedSite == poi->name;
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (isSelected)
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        ImGui::TreeNodeEx(poi->name.c_str(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            selectedSite = poi->name;
            selectedPlanetSite = cbody;
        }
    }

    sys = player->getSystem();
    showApplyButton();

    ImGui::EndChild();
}