#include "orennayarbrdf.h"
#include <warpfunctions.h>

Color3f OrenNayarBRDF::f(const Vector3f &wo, const Vector3f &wi) const {

    float sinThetaI = std::sqrt(std::max(0.f, 1.f - wi.z * wi.z));
    float sinThetaO = std::sqrt(std::max(0.f, 1.f - wo.z * wo.z));

    float maxCos = 0;
    if (sinThetaI > 1e-4 && sinThetaO > 1e-4) {
        float sinPhiI = (sinThetaI == 0) ? 0 : glm::clamp(wi.y / sinThetaI, -1.f, 1.f);
        float cosPhiI = (sinThetaI == 0) ? 0 : glm::clamp(wi.x / sinThetaI, -1.f, 1.f);
        float sinPhiO = (sinThetaO == 0) ? 0 : glm::clamp(wo.y / sinThetaO, -1.f, 1.f);
        float cosPhiO = (sinThetaO == 0) ? 0 : glm::clamp(wo.x / sinThetaO, -1.f, 1.f);
        float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = std::max(0.f, dCos);
    }

    // Compute sine and tangent terms of Oren-Nayar model
    float sinAlpha, tanBeta;
    if (AbsCosTheta(wi) > AbsCosTheta(wo)) {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    } else {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }
    return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
}

Color3f OrenNayarBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    *wi = WarpFunctions::squareToHemisphereCosine(u);
    if (wo.z < 0) {
        wi->z *= -1;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float OrenNayarBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? WarpFunctions::squareToHemisphereCosinePDF(wi) : 0.f;

}
