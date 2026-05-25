// Actividad Integradora - 6 Movimientos en 3D
#include "raylib.h"
#include "rlgl.h"
#include <cmath>

int main()
{
    const int SW = 1280;
    const int SH = 720;
    const int CW = SW / 2;
    const int CH = SH / 3;

    InitWindow(SW, SH, "Practica - 6 Movimientos en 3D");
    SetTargetFPS(60);

    Camera3D cam = { 0 };
    cam.position   = { 6.0f, 5.0f, 6.0f };
    cam.target     = { 0.0f, 1.0f, 0.0f };
    cam.up         = { 0.0f, 1.0f, 0.0f };
    cam.fovy       = 45.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    RenderTexture2D rt[6];
    for (int i = 0; i < 6; i++)
        rt[i] = LoadRenderTexture(CW - 2, CH - 2);

    float trasX     = -2.5f;
    float angulo    = 0.0f;
    float reboteY   = 1.0f;
    float velRebote = 0.07f;

    Color bgColors[6] = {
        { 220, 235, 255, 255 },
        { 255, 230, 210, 255 },
        { 210, 255, 220, 255 },
        { 255, 215, 230, 255 },
        { 230, 220, 255, 255 },
        { 215, 245, 255, 255 },
    };

    int panelX[6] = { 0, CW,  0, CW,    0,   CW };
    int panelY[6] = { 0,  0, CH, CH, CH*2, CH*2 };

    const char* labels[6] = {
        "1. Traslacion", "2. Rotacion",    "3. Rebote",
        "4. Senoidal",   "5. Trayectoria", "6. Orbita"
    };

    while (!WindowShouldClose())
    {
        float t = (float)GetTime();

        trasX += 0.04f;
        if (trasX > 2.5f) trasX = -2.5f;
        angulo += 1.5f;
        reboteY += velRebote;
        if (reboteY > 3.5f || reboteY < 0.5f) velRebote *= -1.0f;

        // Panel 1 — Traslacion
        BeginTextureMode(rt[0]);
            ClearBackground(bgColors[0]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                DrawCube({ trasX, 1.0f, 0.0f }, 1.5f, 1.5f, 1.5f, BLUE);
                DrawCubeWires({ trasX, 1.0f, 0.0f }, 1.5f, 1.5f, 1.5f, DARKBLUE);
            EndMode3D();
            DrawText("Cubo se desplaza en X", 8, 8, 14, DARKBLUE);
        EndTextureMode();

        // Panel 2 — Rotacion
        BeginTextureMode(rt[1]);
            ClearBackground(bgColors[1]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                rlPushMatrix();
                    rlTranslatef(0.0f, 1.0f, 0.0f);
                    rlRotatef(angulo, 0.0f, 1.0f, 0.0f);
                    DrawCube({ 0.0f, 0.0f, 0.0f }, 1.5f, 1.5f, 1.5f, ORANGE);
                    DrawCubeWires({ 0.0f, 0.0f, 0.0f }, 1.5f, 1.5f, 1.5f, BROWN);
                rlPopMatrix();
            EndMode3D();
            DrawText("Cubo rota en eje Y", 8, 8, 14, BROWN);
        EndTextureMode();

        // Panel 3 — Rebote
        BeginTextureMode(rt[2]);
            ClearBackground(bgColors[2]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                DrawCube({ 0.0f, 0.05f, 0.0f }, 3.0f, 0.1f, 3.0f, LIGHTGRAY);
                DrawCylinder({ 0.0f, 0.0f, 0.0f }, 0.05f, 0.05f, reboteY, 8, GRAY);
                DrawSphereEx({ 0.0f, reboteY, 0.0f }, 0.5f, 12, 12, RED);
            EndMode3D();
            DrawText("Esfera rebota verticalmente", 8, 8, 14, MAROON);
        EndTextureMode();

        // Panel 4 — Senoidal
        BeginTextureMode(rt[3]);
            ClearBackground(bgColors[3]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                for (int i = -8; i < 8; i++) {
                    float xa = (float)i * 0.4f;
                    float xb = (float)(i+1) * 0.4f;
                    DrawLine3D({ xa, 1.5f + sinf((t+xa)*2.5f)*1.2f, 0.0f },
                               { xb, 1.5f + sinf((t+xb)*2.5f)*1.2f, 0.0f }, PINK);
                }
                DrawSphereEx({ 0.0f, 1.5f + sinf(t*2.5f)*1.2f, 0.0f }, 0.5f, 12, 12, PINK);
            EndMode3D();
            DrawText("Esfera sigue funcion seno", 8, 8, 14, MAROON);
        EndTextureMode();

        // Panel 5 — Trayectoria
        BeginTextureMode(rt[4]);
            ClearBackground(bgColors[4]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                for (int i = 0; i < 80; i++) {
                    float x1 = -2.5f + i * (5.0f/80.0f);
                    float x2 = -2.5f + (i+1) * (5.0f/80.0f);
                    DrawLine3D({ x1, 1.0f, sinf(x1*3.0f)*1.5f },
                               { x2, 1.0f, sinf(x2*3.0f)*1.5f }, PURPLE);
                }
                float trajX = -2.5f + fmodf(t*1.2f, 5.0f);
                DrawSphereEx({ trajX, 1.0f, sinf(trajX*3.0f)*1.5f }, 0.5f, 12, 12, VIOLET);
            EndMode3D();
            DrawText("Esfera sigue trayectoria ondulada", 8, 8, 14, PURPLE);
        EndTextureMode();

        // Panel 6 — Orbita
        BeginTextureMode(rt[5]);
            ClearBackground(bgColors[5]);
            BeginMode3D(cam);
                DrawGrid(10, 1.0f);
                DrawSphereEx({ 0.0f, 1.0f, 0.0f }, 0.35f, 12, 12, GOLD);
                DrawSphereWires({ 0.0f, 1.0f, 0.0f }, 2.2f, 3, 24, Fade(SKYBLUE, 0.4f));
                DrawSphereEx({ cosf(t*1.2f)*2.2f, 1.0f, sinf(t*1.2f)*2.2f }, 0.45f, 12, 12, SKYBLUE);
                DrawLine3D({ 0.0f, 1.0f, 0.0f }, { cosf(t*1.2f)*2.2f, 1.0f, sinf(t*1.2f)*2.2f }, SKYBLUE);
            EndMode3D();
            DrawText("Esfera orbita alrededor del centro", 8, 8, 14, DARKBLUE);
        EndTextureMode();

        // Dibujar en pantalla
        BeginDrawing();
        ClearBackground(DARKGRAY);

        for (int i = 0; i < 6; i++) {
            DrawRectangleLines(panelX[i], panelY[i], CW, CH, BLACK);
            DrawTextureRec(rt[i].texture,
                { 0, 0, (float)(CW-2), -(float)(CH-2) },
                { (float)(panelX[i]+1), (float)(panelY[i]+1) },
                WHITE);
            DrawRectangle(panelX[i]+1, panelY[i]+1, CW-2, 24, Fade(BLACK, 0.45f));
            DrawText(labels[i], panelX[i]+8, panelY[i]+5, 16, WHITE);
        }

        DrawRectangle(0, SH-28, SW, 28, Fade(BLACK, 0.7f));
        DrawText("6 Tipos de Movimiento 3D  |  ESC para salir", 20, SH-20, 16, WHITE);

        EndDrawing();
    }

    for (int i = 0; i < 6; i++)
        UnloadRenderTexture(rt[i]);

    CloseWindow();
    return 0;
}