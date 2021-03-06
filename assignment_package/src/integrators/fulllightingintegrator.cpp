#include "fulllightingintegrator.h"
#include "globals.h"
#include "../scene/lights/diffusearealight.h"
#include <iostream>

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    Color3f L = Color3f(0.0f);
    Color3f beta = Color3f(1.0f);
    bool specularBounce = false;

    Ray r = Ray(ray);
    for (int bounces = 0; bounces < depth; ++bounces) {
        Intersection isect;
        bool foundIntersection = scene.Intersect(r, &isect);

        if (bounces == 0 || specularBounce) {
            if (foundIntersection) {
                L += beta * isect.Le(-r.direction);
            } else {
                for (const auto &light : scene.lights) {
                    L += beta * light->Le(r);
                }
            }
        }

        if (!foundIntersection || bounces >= depth) {
            break;
        }

        if (!isect.ProduceBSDF()) {
            break;
        }

        int n = scene.lights.size();
        int i = std::min((int)(sampler->Get1D() * n), n - 1);
        std::shared_ptr<Light> light = scene.lights[i];

        Vector3f wo = -r.direction;
        Vector3f wi_g;
        float pdf_g;

        Color3f li_g = light->Sample_Li(isect, sampler->Get2D(), &wi_g, &pdf_g);
        Color3f f_g = isect.bsdf->f(wo, wi_g);
        pdf_g /= float(n);

        float w_g = PowerHeuristic2(1, pdf_g, 1, isect.bsdf->Pdf(wo, wi_g));

        Color3f g = f_g * li_g * AbsDot(wi_g, isect.normalGeometric) / pdf_g;
        Intersection shadow_test_g;
        if (scene.Intersect(isect.SpawnRay(wi_g), &shadow_test_g)) {
            if (shadow_test_g.objectHit->areaLight != light || pdf_g == 0.f) {
                g = Color3f(0.f);
            }
        }

        Vector3f wi_f;
        float pdf_f;
        Color3f f_f = isect.bsdf->Sample_f(wo, &wi_f, sampler->Get2D(), &pdf_f, BSDF_ALL);

        Color3f f = Color3f(0.f);
        Intersection shadow_test_f;
        if (scene.Intersect(isect.SpawnRay(wi_f), &shadow_test_f)) {
            if (shadow_test_f.objectHit->areaLight == light && pdf_f > 0.f) {
                Color3f li_f = light.get()->L(shadow_test_f, -wi_f);
                f = f_f * li_f * AbsDot(wi_f, isect.normalGeometric) / pdf_f;
            }
        }

        float w_f = PowerHeuristic2(1, pdf_f, 1, light.get()->Pdf_Li(isect, wi_f));

        L += beta * (w_g * g + w_f * f);

        Vector3f wi_global;
        float pdf_global;
        BxDFType flags;
        Color3f f_global = isect.bsdf->Sample_f(wo, &wi_global, sampler->Get2D(), &pdf_global, BSDF_ALL, &flags);
        if (pdf_global > 0.f) {
            beta *= f_global * AbsDot(wi_global, isect.normalGeometric) / pdf_global;
        }
        specularBounce = (flags & BSDF_SPECULAR) != 0;
        r = isect.SpawnRay(wi_global);


        if (bounces > 3) {
            float max = glm::max(beta.x, glm::max(beta.y, beta.z));
            if (max < sampler->Get1D()) {
                break;
            }
            beta /= max;
        }
    }

    return L;
}

float BalanceHeuristic2(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.f || gPdf == 0.f) {
        return 0.f;
    }
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic2(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.f || gPdf == 0.f) {
        return 0.f;
    }
    float f = nf * fPdf;
    float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}
