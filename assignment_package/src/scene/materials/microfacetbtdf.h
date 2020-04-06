#pragma once
#include "bsdf.h"
#include "fresnel.h"
#include "microfacet.h"

class MicrofacetBTDF : public BxDF
{
public:
    MicrofacetBTDF(const Color3f &R, MicrofacetDistribution* distribution, Fresnel* fresnel, float etaA, float etaB)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), T(T), distribution(distribution), fresnel(fresnel) {}

    virtual ~MicrofacetBTDF(){delete fresnel; delete distribution;}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &xi, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


  private:
    const Color3f T;
    const MicrofacetDistribution* distribution;
    const Fresnel* fresnel;
    float etaA, etaB;
};
