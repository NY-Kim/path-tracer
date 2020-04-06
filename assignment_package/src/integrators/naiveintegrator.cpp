#include "naiveintegrator.h"
#include "globals.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    if (depth < 0) {
        return Color3f(0.f);
    }

    Intersection isect;
    if (!scene.Intersect(ray, &isect)) {
        return Color3f(0.f);
    }

    Color3f le = isect.Le(-ray.direction);
    if (!isect.ProduceBSDF()) {
        return le;
    }

    Vector3f wo = -ray.direction;
    Vector3f wi;
    float pdf;
    BxDFType flags = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
    Color3f f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(), &pdf, BSDF_ALL, &flags);

    if (!IsBlack(f) && pdf > 0.f) {
        return le + (f * Li(isect.SpawnRay(wi), scene, sampler, depth - 1) * AbsDot(wi, isect.bsdf->normal)) / pdf;
    }

    return Color3f(0.f);
}
