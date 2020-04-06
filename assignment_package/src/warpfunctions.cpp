#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>
#include "globals.h"

glm::vec3 WarpFunctions::squareToDiskUniform(const glm::vec2 &sample)
{
    float r = std::sqrt(sample.x);
    float theta = 2.f * Pi * sample.y;
    return glm::vec3(r * cosf(theta), r * sinf(theta), 0.f);
}

glm::vec3 WarpFunctions::squareToDiskConcentric(const glm::vec2 &sample)
{
    //Peter Shirley's warping method
    float phi, r;
    float a = 2.f * sample.x - 1;
    float b = 2.f * sample.y - 1;

    if (a > -b) {
        if (a > b) {
            r = a;
            phi = Pi / 4 * (b / a);
        } else {
            r = b;
            phi = Pi / 4 * (2.f - (a / b));
        }
    } else {
        if (a < b) {
            r = -a;
            phi = Pi / 4 * (4.f + (b / a));
        } else {
            r = -b;
            if (b != 0) {
                phi = Pi / 4 * (6.f - (a / b));
            } else {
                phi = 0;
            }
        }
    }

    return glm::vec3(r * cosf(phi), r * sinf(phi), 0.f);
}

float WarpFunctions::squareToDiskPDF(const glm::vec3 &sample)
{
    return InvPi;
}

glm::vec3 WarpFunctions::squareToSphereUniform(const glm::vec2 &sample)
{
    float z = 1.f - 2.f * sample.x;
    float x = cosf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    float y = sinf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToSphereUniformPDF(const glm::vec3 &sample)
{
    return Inv4Pi;
}

glm::vec3 WarpFunctions::squareToSphereCapUniform(const glm::vec2 &sample, float thetaMin)
{
    float z = 1.f - 2.f * sample.x * (180.f - thetaMin) / 180.f;
    float x = cosf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    float y = sinf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const glm::vec3 &sample, float thetaMin)
{
    return Inv2Pi / (1.f - cosf(glm::radians(180.f - thetaMin)));
}

glm::vec3 WarpFunctions::squareToHemisphereUniform(const glm::vec2 &sample)
{
    float z = sample.x;
    float x = cosf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    float y = sinf(2.f * Pi * sample.y) * std::sqrt(1.f - z * z);
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const glm::vec3 &sample)
{
    return Inv2Pi;
}

glm::vec3 WarpFunctions::squareToHemisphereCosine(const glm::vec2 &sample)
{
    glm::vec3 disk = squareToDiskConcentric(sample);
    disk.z = sqrt(1.f - disk.x * disk.x - disk.y * disk.y);
    return disk;
}

float WarpFunctions::squareToHemisphereCosinePDF(const glm::vec3 &sample)
{
    return sample.z / Pi;
}
