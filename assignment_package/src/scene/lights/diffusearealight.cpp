#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    return glm::dot(isect.normalGeometric, w) > 0.f ? emittedLight : Color3f(0.f);
}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const {
    Intersection intersection = shape->Sample(ref, xi, pdf);

    if (*pdf == 0.0f || glm::length(ref.point - intersection.point) == 0)
    {
        return Color3f(0.0f);
    }

    *wi = glm::normalize(intersection.point - ref.point);

    return L(intersection, -*wi);
}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    Ray ray = ref.SpawnRay(wi);
    Intersection isect;
    if (!shape->Intersect(ray, &isect)) {
        return 0.f;
    }

    if (AbsDot(isect.normalGeometric, -wi) == 0) {
        return 0.f;
    }

    return glm::distance2(ref.point, isect.point) / AbsDot(isect.normalGeometric, -wi) / shape->Area();
}
