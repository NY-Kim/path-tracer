#include "pointlight.h"

Color3f PointLight::L(const Intersection &isect, const Vector3f &w) const {
    return I;
}

Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const {
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.f;
    return I / glm::distance2(pLight, ref.point);
}

float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0.f;
}
