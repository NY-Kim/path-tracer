#include "spotlight.h"

Color3f SpotLight::L(const Intersection &isect, const Vector3f &w) const {
    return glm::dot(isect.normalGeometric, w) > 0.f ? I : Color3f(0.f);
}

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.f;
    return I * Falloff(-*wi) / glm::distance2(pLight, ref.point);
}

float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0.f;
}

float SpotLight::Falloff(const Vector3f &w) const {
    Vector3f wl = glm::normalize(Vector3f(transform.invT() * glm::vec4(w, 0)));
    float cosTheta = wl.z;
    if (cosTheta < cosTotalWidth) {
        return 0;
    }
    if (cosTheta > cosFalloffStart) {
        return 1;
    }

    float delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return (delta * delta) * (delta * delta);
}
