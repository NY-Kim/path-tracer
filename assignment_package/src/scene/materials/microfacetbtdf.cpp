#include "microfacetbtdf.h"

Color3f MicrofacetBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    if (SameHemisphere(wo, wi)) {
        return Color3f(0.f);
    }

    float cosThetaO = CosTheta(wo);
    float cosThetaI = CosTheta(wi);
    if (cosThetaI == 0 || cosThetaO == 0) {
        return Color3f(0.f);
    }

    float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
    Vector3f wh = glm::normalize(wo + wi * eta);
    if (wh.z < 0) {
        wh = -wh;
    }

    Color3f F = fresnel->Evaluate(glm::dot(wo, wh));

    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);

    return (Color3f(1.f) - F) * T * std::abs(distribution->D(wh) * distribution->G(wo, wi)
           * eta * eta * AbsDot(wi, wh) * AbsDot(wo, wh) / (cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}

Color3f MicrofacetBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    if (wo.z == 0) {
        return Color3f(0.f);
    }
    Vector3f wh = distribution->Sample_wh(wo, xi);
    if (glm::dot(wo, wh) < 0) {
        return Color3f(0.f);
    }

    float eta = CosTheta(wo) > 0 ? (etaA / etaB) : (etaB / etaA);
    if (!Refract(wo, (Normal3f)wh, eta, wi)) {
        return Color3f(0.f);
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float MicrofacetBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    if (SameHemisphere(wo, wi)) {
        return 0.f;
    }

    float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
    Vector3f wh = glm::normalize(wo + wi * eta);

    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);
    float dwh_dwi = std::abs((eta * eta * glm::dot(wi, wh)) / (sqrtDenom * sqrtDenom));
    return distribution->Pdf(wo, wh) * dwh_dwi;
}
