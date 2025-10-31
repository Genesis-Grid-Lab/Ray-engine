#pragma once
#include "raymath.h"
#include "repch.h"

inline static void DrawCameraFrustum(const Camera3D &cam, float nearDist, float farDist, Color col)
{
    if (cam.projection != CAMERA_PERSPECTIVE) {
        // Simple ortho box (approx) — draw a box centered on camera target with size derived from fovy.
        float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
        float halfH = tanf((cam.fovy * DEG2RAD * 0.5f)) * farDist; // use farDist as extent
        float halfW = halfH * aspect;
        Vector3 center = cam.target;
        // Compute basis
        Vector3 forward = Vector3Normalize(Vector3Subtract(cam.target, cam.position));
        Vector3 right   = Vector3Normalize(Vector3CrossProduct(forward, cam.up));
        Vector3 upv     = Vector3CrossProduct(right, forward);
        // Build 8 box corners around center
        Vector3 corners[8];
        int idx = 0;
        for (int z = -1; z <= 1; z += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int x = -1; x <= 1; x += 2) {
                    Vector3 p = center;
                    p = Vector3Add(p, Vector3Scale(right, halfW * (float)x));
                    p = Vector3Add(p, Vector3Scale(upv,    halfH * (float)y));
                    p = Vector3Add(p, Vector3Scale(forward, (float)z * halfH)); // depth extent approx
                    corners[idx++] = p;
                }
            }
        }
        // draw edges
        for (int i = 0; i < 4; ++i) {
            DrawLine3D(corners[i], corners[i+4], col); // connect near <-> far (our ordering)
            DrawLine3D(corners[i], corners[(i+1)%4], col); // near quad
            DrawLine3D(corners[i+4], corners[4 + ((i+1)%4)], col); // far quad
        }
        return;
    }

    // Perspective frustum
    const float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();

    // Build camera basis vectors
    Vector3 forward = Vector3Normalize(Vector3Subtract(cam.target, cam.position)); // camera forward
    Vector3 right   = Vector3Normalize(Vector3CrossProduct(forward, cam.up));
    Vector3 upv     = Vector3Normalize(Vector3CrossProduct(right, forward)); // re-orthogonalized up

    // Compute heights and widths for near/far planes
    float tanHalfFovy = tanf((cam.fovy * DEG2RAD) * 0.5f);
    float nearH = 2.0f * tanHalfFovy * nearDist;
    float nearW = nearH * aspect;
    float farH  = 2.0f * tanHalfFovy * farDist;
    float farW  = farH * aspect;

    // Plane centers
    Vector3 nearCenter = Vector3Add(cam.position, Vector3Scale(forward, nearDist));
    Vector3 farCenter  = Vector3Add(cam.position, Vector3Scale(forward, farDist));

    // Near plane corners (clockwise or CCW)
    Vector3 nearTopLeft     = Vector3Add(Vector3Add(nearCenter, Vector3Scale(upv,  nearH * 0.5f)), Vector3Scale(right, -nearW * 0.5f));
    Vector3 nearTopRight    = Vector3Add(Vector3Add(nearCenter, Vector3Scale(upv,  nearH * 0.5f)), Vector3Scale(right,  nearW * 0.5f));
    Vector3 nearBottomLeft  = Vector3Add(Vector3Add(nearCenter, Vector3Scale(upv, -nearH * 0.5f)), Vector3Scale(right, -nearW * 0.5f));
    Vector3 nearBottomRight = Vector3Add(Vector3Add(nearCenter, Vector3Scale(upv, -nearH * 0.5f)), Vector3Scale(right,  nearW * 0.5f));

    // Far plane corners
    Vector3 farTopLeft     = Vector3Add(Vector3Add(farCenter, Vector3Scale(upv,  farH * 0.5f)), Vector3Scale(right, -farW * 0.5f));
    Vector3 farTopRight    = Vector3Add(Vector3Add(farCenter, Vector3Scale(upv,  farH * 0.5f)), Vector3Scale(right,  farW * 0.5f));
    Vector3 farBottomLeft  = Vector3Add(Vector3Add(farCenter, Vector3Scale(upv, -farH * 0.5f)), Vector3Scale(right, -farW * 0.5f));
    Vector3 farBottomRight = Vector3Add(Vector3Add(farCenter, Vector3Scale(upv, -farH * 0.5f)), Vector3Scale(right,  farW * 0.5f));

    // Draw near plane edges
    DrawLine3D(nearTopLeft, nearTopRight, col);
    DrawLine3D(nearTopRight, nearBottomRight, col);
    DrawLine3D(nearBottomRight, nearBottomLeft, col);
    DrawLine3D(nearBottomLeft, nearTopLeft, col);

    // Draw far plane edges
    DrawLine3D(farTopLeft, farTopRight, col);
    DrawLine3D(farTopRight, farBottomRight, col);
    DrawLine3D(farBottomRight, farBottomLeft, col);
    DrawLine3D(farBottomLeft, farTopLeft, col);

    // Connect near->far
    DrawLine3D(nearTopLeft, farTopLeft, col);
    DrawLine3D(nearTopRight, farTopRight, col);
    DrawLine3D(nearBottomRight, farBottomRight, col);
    DrawLine3D(nearBottomLeft, farBottomLeft, col);

    // Optionally draw camera position and plane centers
    DrawSphere(cam.position, 0.05f, col);
    DrawSphere(nearCenter, 0.02f, col);
    DrawSphere(farCenter, 0.02f, col);
}
