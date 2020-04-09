#pragma once

#include "light.h"

class SpotLight : public Light
{
public:
    SpotLight(const Transform &t, const Point3f &pLight,const Color3f& I, float totalWidth, float falloffStart)
        : Light(t), pLight(pLight), I(I),
          cosTotalWidth(std::cos(glm::radians(totalWidth))), cosFalloffStart(std::cos(glm::radians(falloffStart))) {}

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;
    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                         Vector3f *wi, Float *pdf) const;
    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;
    float Falloff(const Vector3f &w) const;

    const Point3f pLight;
    const Color3f I;
    const Float cosTotalWidth, cosFalloffStart;
};

