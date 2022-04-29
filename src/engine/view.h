// view.h - controllable window view package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

class View
{
public:
    enum viewType
    {
        viewMainWindow    = 1,
        viewMultiFunction = 2,
    };

    View(viewType type, Player *player, Scene *scene,
        float wx, float wy, float width, float height)
    : type(type), player(player), scene(scene),
      width(width), height(height), x(wx), y(wy)
    { }
    ~View() = default;


    void map(float wx, float wy, float &vx, float &vy) const;

private:
    viewType type;

protected:
    // Hierarchy view tree
    View *parent = nullptr;
    View *silding = nullptr;
    View *child = nullptr;

    Player *player = nullptr;
    Scene *scene = nullptr;

    // View screen parameter
    float width = 1.0f;
    float height = 1.0f;
    float x = 0.0f, y = 0.0f;
};