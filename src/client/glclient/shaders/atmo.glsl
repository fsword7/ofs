struct AtmParm {
    float topRadius;
    float bottomRadius;
};

const float m = 1.0;

const float m2 = m*m;
const float m3 = m*m*m;

float clampConsine(float mu)
{
    return clamp(mu, -1.0, 1.0);
}

float clampDistance(float d)
{
    return max(d, 0.0 * m);
}

float clampRadius(in AtmParm atm, float r)
{
    return clamp(r, atm.bottomRadius, atm.topRadius);
}

float distanceToTopAtmosphere(in AtmParm atm, float r, float mu)
{
    float discriminant = r*r * (mu*mu - 1.0) +
        atm.topRadius*atm.topRadius;
    return clampDistance(-r * mu + sqrt(discriminant));
}

float distanceToBottomAtmosphere(in AtmParm atm, float r, float mu)
{
    float discriminant = r*r * (mu*mu - 1.0) +
        atm.bottomRadius*atm.bottomRadius;
    return clampDistance(-r * mu - sqrt(discriminant));
}

bool intersectGround(in AtmParm atm, float r, float mu)
{
    return mu < 1.0 && r*r * (mu*mu-1.0) +
        atm.bottomRadius*atm.bottomRadius >= 0.0 * m2;
}

