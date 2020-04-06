#include "fresnel.h"
#include <algorithm>

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);
    bool entering = cosThetaI > 0.f;
    float v_etaI = etaI;
    float v_etaT = etaT;
    if (!entering) {
       std::swap(v_etaI, v_etaT);
       cosThetaI = std::abs(cosThetaI);
   }

    float sinThetaI = std::sqrt(std::max(0.f, 1.f - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;

    if (sinThetaT >= 1) {
        return Color3f(1.0f);
    }

    float cosThetaT = std::sqrt(std::max(0.f, 1 - sinThetaT * sinThetaT));

    float Rparl = ((v_etaT * cosThetaI) - (v_etaI * cosThetaT)) / ((v_etaT * cosThetaI) + (v_etaI * cosThetaT));
    float Rperp = ((v_etaI * cosThetaI) - (v_etaT * cosThetaT)) / ((v_etaI * cosThetaI) + (v_etaT * cosThetaT));

    return Color3f((Rparl * Rparl + Rperp * Rperp) / 2.f);
}

Color3f FresnelConductor::Evaluate(float cosThetaI) const
{
    cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);
    Color3f eta = etaT / etaI;
    Color3f etaK = k / etaI;

    float cosThetaI2 = cosThetaI * cosThetaI;
    float sinThetaI2 = 1. - cosThetaI2;
    Color3f eta2 = eta * eta;
    Color3f etaK2 = etaK * etaK;

    Color3f t0 = eta2 - etaK2 - sinThetaI2;
    Color3f a2plusb2 = glm::sqrt(t0 * t0 + 4.f * eta2 * etaK2);
    Color3f t1 = a2plusb2 + cosThetaI2;
    Color3f a = glm::sqrt(0.5f * (a2plusb2 + t0));
    Color3f t2 = 2.f * cosThetaI * a;
    Color3f Rs = (t1 - t2) / (t1 + t2);

    Color3f t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    Color3f t4 = t2 * sinThetaI2;
    Color3f Rp = Rs * (t3 - t4) / (t3 + t4);

    return 0.5f * (Rp + Rs);
}
