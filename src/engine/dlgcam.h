// dlgcam.h - Dialog Camera package
//
// Author: Tim Stark
// Date: Sep 25, 2024

class Player;
class pSystem;

class DialogCamera : public GUIElement
{
public:
    DialogCamera(cstr_t &name);
    
    void show() override;

private:
    void addCelestial(const Celestial *cbody, str_t &selected);

    void showApplyButton();

    void showTargetTab();
    void showTrackTab();
    void showGroundTab();

private:
    Player *player = nullptr;
    Camera *cam = nullptr;
    pSystem *sys = nullptr;

    cameraMode extMode;

    str_t selectedTarget;
    str_t selectedSyncTarget;
    str_t selectedSite;
    CelestialPlanet *selectedPlanetSite = nullptr;

    float sz1;
};