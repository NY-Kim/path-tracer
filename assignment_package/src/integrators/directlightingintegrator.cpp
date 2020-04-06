#include "directlightingintegrator.h"
#include "globals.h"
#include "../scene/lights/diffusearealight.h"
#include <iostream>

Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    if (depth <= 0) {
        return Color3f(0.f);
    }

    Intersection isect;
    if (!scene.Intersect(ray, &isect)) {
        return Color3f(0.f);;
    }

    Color3f le = isect.Le(-ray.direction);
    if (!isect.ProduceBSDF()) {
        return le;
    }

    int n = scene.lights.size();
    int i = std::min((int)(sampler->Get1D() * n), n - 1);
    std::shared_ptr<Light> light = scene.lights[i];

    Vector3f wo_g = -ray.direction;
    Vector3f wi_g;
    float pdf_g;

    Color3f li_g = light->Sample_Li(isect, sampler->Get2D(), &wi_g, &pdf_g);
    Color3f f_g = isect.bsdf->f(wo_g, wi_g);
    pdf_g /= float(n);

    float w_g = PowerHeuristic(1, pdf_g, 1, isect.bsdf->Pdf(wo_g, wi_g));

    Color3f g = Color3f(0.f);
    Intersection shadow_test_g;
    if (scene.Intersect(isect.SpawnRay(wi_g), &shadow_test_g)) {
        if (shadow_test_g.objectHit->areaLight == light && pdf_g > 0.f) {
            g = f_g * li_g * AbsDot(wi_g, isect.normalGeometric) / pdf_g;
        }
    }

    Vector3f wo_f = -ray.direction;
    Vector3f wi_f;
    float pdf_f;
    Color3f f_f = isect.bsdf->Sample_f(wo_f, &wi_f, sampler->Get2D(), &pdf_f, BSDF_ALL);

    Color3f f = Color3f(0.f);
    Intersection shadow_test_f;
    if (scene.Intersect(isect.SpawnRay(wi_f), &shadow_test_f)) {
        if (shadow_test_f.objectHit->areaLight == light && pdf_f > 0.f) {
            Color3f li_f = dynamic_cast<DiffuseAreaLight*>(light.get())->L(shadow_test_f, -wi_f);
            f = f_f * li_f * AbsDot(wi_f, isect.normalGeometric) / pdf_f;
        }
    }

    float w_f = PowerHeuristic(1, pdf_f, 1, dynamic_cast<DiffuseAreaLight*>(light.get())->Pdf_Li(isect, wi_f));

    return le + w_g * g + w_f * f;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.f || gPdf == 0.f) {
        return 0.f;
    }
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.f || gPdf == 0.f) {
        return 0.f;
    }
    float f = nf * fPdf;
    float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}
