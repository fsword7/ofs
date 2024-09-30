// dlgcam.h - Dialog Camera package
//
// Author: Tim Stark
// Date: Sep 25, 2024

class Player;

class DialogCamera : public GUIElement
{
public:
    DialogCamera(cstr_t &name);
    
    void show() override;

private:
    void addCelestial(const Celestial *cbody);

    void showTargetTab();
    void showTrackTab();

private:
    Player *player = nullptr;
    Camera *cam = nullptr;

    str_t selectedTarget;
};