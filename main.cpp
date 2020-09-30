#include <math.h>
#include <raylib.h>

int const TARGET_FPS = 60;
int const TARGET_WIDTH = 640;
int const TARGET_HEIGHT = 360;
int const WINDOW_SCALE = 2;
int const BASE_POSITION_COUNT = 512;
int const LIGHTNING_STEPS = 4;
int const FIRST_STEP_SIZE = 1 << LIGHTNING_STEPS;
int const TARGET_POSITION_COUNT =
    FIRST_STEP_SIZE * (BASE_POSITION_COUNT - 1) + 1;
int const LIGHTNING_STEP = 32;
int const LIGHTNING_LEN = 256;
int const ANIMATION_FRAMES = BASE_POSITION_COUNT;

Vector3 positions[TARGET_POSITION_COUNT];

int main() {
  system("rm -f /tmp/my/*");
  InitWindow(WINDOW_SCALE * TARGET_WIDTH, WINDOW_SCALE * TARGET_HEIGHT,
             "Window 1");

  Shader shader = LoadShader(nullptr, "../bloom.frag");
  RenderTexture2D target_texture =
      LoadRenderTexture(TARGET_WIDTH, TARGET_HEIGHT);
  RenderTexture2D render_texture =
      LoadRenderTexture(TARGET_WIDTH, TARGET_HEIGHT);

  Camera3D camera;
  camera.position = {5.0f, 2.0f, 0.0f};
  camera.target = {0.0f, 0.0f, 0.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.type = CAMERA_PERSPECTIVE;
  // SetCameraMode(camera, CAMERA_ORBITAL);

  for (int i = 0; i < BASE_POSITION_COUNT; ++i) {
    float phi = 0.1f * (i + 1);
    float r = 0.01f * (i + 1);
    positions[FIRST_STEP_SIZE * i].x = r * cosf(phi);
    positions[FIRST_STEP_SIZE * i].y = sinf(r) / r;
    positions[FIRST_STEP_SIZE * i].z = r * sinf(phi);
  }

  for (int i = 0; i < LIGHTNING_STEPS; ++i) {
    int step = FIRST_STEP_SIZE >> i;
    int half_step = step >> 1;
    for (int j = 0; j + 1 < TARGET_POSITION_COUNT; j += step) {
      Vector3 p1 = positions[j];
      Vector3 p2 = positions[j + step];
      Vector3 center = {
          0.5f * (p1.x + p2.x),
          0.5f * (p1.y + p2.y),
          0.5f * (p1.z + p2.z),
      };
      Vector3 diff = {
          p2.x - p1.x,
          p2.y - p1.y,
          p2.z - p1.z,
      };
      Vector3 axis1 = {diff.x * diff.z, diff.y * diff.z,
                       -diff.x * diff.x - diff.y * diff.z};
      Vector3 divided = {diff.x / axis1.x, diff.y / axis1.y, diff.z / axis1.z};
      float diff_len =
          sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
      float axis1_len =
          sqrtf(axis1.x * axis1.x + axis1.y * axis1.y + axis1.z * axis1.z);
      axis1.x *= diff_len / axis1_len;
      axis1.y *= diff_len / axis1_len;
      axis1.z *= diff_len / axis1_len;
      Vector3 axis2 = {
          diff.y * axis1.z - diff.z * axis1.y,
          diff.z * axis1.x - diff.x * axis1.z,
          diff.x * axis1.y - diff.y * axis1.x,
      };

      float t1 = 0.25f * GetRandomValue(-32767, 32767) / 32768.0f;
      float t2 = 0.25f * GetRandomValue(-32767, 32767) / 32768.0f;

      positions[j + half_step] = {
          center.x + t1 * axis1.x + t2 * axis2.x,
          center.y + t1 * axis1.y + t2 * axis2.y,
          center.z + t1 * axis1.z + t2 * axis2.z,
      };
    }
  }

  // SetTargetFPS(TARGET_FPS);
  for (int current_frame = 0;
       !WindowShouldClose() && current_frame < ANIMATION_FRAMES;
       ++current_frame) {
    float phi = 2.0f * M_PI * current_frame / ANIMATION_FRAMES;
    camera.position.x = 5.0f * cosf(phi);
    camera.position.z = 5.0f * sinf(phi);

    BeginDrawing();
    BeginTextureMode(render_texture);
    ClearBackground(BLACK);
    BeginMode3D(camera);

    for (int i = 0; i + 1 < TARGET_POSITION_COUNT; ++i) {
      if (((i - LIGHTNING_STEP * current_frame) / LIGHTNING_LEN) % 2 == 0)
        continue;
      DrawLine3D(positions[i], positions[i + 1],
                 ColorFromHSV({210.0f, 0.5f, 1.0f}));
    }

    // lightning_pos = (lightning_pos + LIGHTNING_STEP) % TARGET_POSITION_COUNT;
    // for (int i = 0; i < LIGHTNING_LEN; ++i) {
    //   int p1 = (lightning_pos + i) % TARGET_POSITION_COUNT;
    //   int p2 = (p1 + 1) % TARGET_POSITION_COUNT;
    //   DrawLine3D(positions[p1], positions[p2], WHITE);
    // }

    EndMode3D();
    EndTextureMode();

    BeginTextureMode(target_texture);
    BeginShaderMode(shader);
    DrawTexture(render_texture.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();

    Image to_save = GetTextureData(target_texture.texture);
    ExportImage(to_save, TextFormat("/tmp/my/f%03d.png", current_frame));
    UnloadImage(to_save);

    DrawTexturePro(
        target_texture.texture, {0, 0, TARGET_WIDTH, TARGET_HEIGHT},
        {0, 0, WINDOW_SCALE * TARGET_WIDTH, WINDOW_SCALE * TARGET_HEIGHT}, {0},
        0, WHITE);

    DrawFPS(10, 10);
    EndDrawing();
  }

  UnloadRenderTexture(render_texture);
  UnloadRenderTexture(target_texture);
  UnloadShader(shader);
  CloseWindow();

  system("ffmpeg /tmp/my/export.gif /tmp/my/export.mp4 /tmp/my/export.mkv -i /tmp/my/f%03d.png");
  return 0;
}
