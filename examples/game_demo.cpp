#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/animation/ticker.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Yoga.h>
#include <random>
#include <chrono>

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkPath.h>
#include <include/effects/SkRuntimeEffect.h>
#include "cubelovers_demo/cube_lovers_sksl.h"

using namespace vaura;

// ════════════════════════════════════════════════════════════════
// Shaders
// ════════════════════════════════════════════════════════════════

const char* SHIP_SKSL = R"(
    uniform float u_time;
    uniform float2 u_resolution;

    half4 main(float2 fragCoord) {
        float2 uv = (fragCoord - 0.5 * u_resolution) / u_resolution.y;
        uv.y = -uv.y; // point up
        
        float ax = abs(uv.x);
        
        // Nose/Body
        float d1 = length(float2(uv.x * 2.0, uv.y - 0.1)) - 0.25;
        // Swept wings
        float d2 = max(ax * 0.8 + uv.y * 0.5 - 0.1, ax - 0.35);
        d2 = max(d2, -uv.y - 0.3);
        
        float d = min(d1, d2);
        
        // Smooth edges
        float alpha = smoothstep(0.02, 0.0, d);
        
        // Colors
        half3 baseCol = half3(0.1, 0.8, 0.9); // Cyan-ish
        half3 coreCol = half3(1.0, 1.0, 1.0);
        
        // Plasma core pulsating
        float core = smoothstep(0.15, 0.0, length(float2(uv.x, uv.y - 0.1))) * (0.8 + 0.2 * sin(u_time * 10.0));
        
        // Thrusters (engines at the bottom)
        float fire = 0.0;
        if (uv.y < -0.2) {
            float flicker = sin(u_time * 50.0) * 0.05 + 0.05;
            float engine_x = ax - 0.15;
            float engine_y = uv.y + 0.3 + flicker;
            float dist = length(float2(engine_x, engine_y * 1.5));
            fire = smoothstep(0.12, 0.0, dist);
        }
        
        // Shield glow around the ship
        float shield = smoothstep(0.05, 0.0, d) * (0.3 + 0.1 * sin(u_time * 5.0 - uv.y * 10.0));
        
        half3 finalCol = baseCol * alpha * 0.6;
        finalCol += coreCol * core;
        finalCol += half3(1.0, 0.3, 0.0) * fire;
        finalCol += half3(0.2, 0.5, 1.0) * shield;
        
        float finalAlpha = max(alpha, fire);
        finalAlpha = max(finalAlpha, shield);
        
        return half4(finalCol, finalAlpha);
    }
)";

const char* ASTEROID_SKSL = R"(
    uniform float u_time;
    uniform float2 u_resolution;

    half4 main(float2 fragCoord) {
        float2 uv = (fragCoord - 0.5 * u_resolution) / u_resolution.y;
        
        // Convert to polar coordinates for spikes
        float a = atan(uv.y, uv.x);
        float r = length(uv);
        
        // Spikes: 8 sharp spikes slowly rotating
        float rot = u_time * -3.0; // Rotate backwards
        float spikes = abs(sin((a * 4.0) + rot)) * 0.15;
        
        float d = r - (0.3 + spikes);
        
        // Anti-aliased edge
        float alpha = smoothstep(0.03, 0.0, d);
        
        // Base color (dark metallic red/black)
        half3 col = half3(0.15, 0.05, 0.05);
        
        // Inner glowing core
        float core_dist = r - 0.15;
        float core = smoothstep(0.1, 0.0, core_dist) * (0.7 + 0.3 * sin(u_time * 20.0));
        
        // Lava cracks (mixing with core)
        float cracks = smoothstep(0.0, 0.03, abs(sin(a * 6.0 - r * 15.0 + u_time)));
        col = mix(half3(1.0, 0.3, 0.0), col, cracks);
        col += half3(1.0, 1.0, 0.5) * core;
        
        // Outer aura/glow
        float aura = smoothstep(0.1, 0.0, d) * 0.5;
        col += half3(1.0, 0.1, 0.0) * aura;
        
        return half4(col * alpha, alpha);
    }
)";

// ════════════════════════════════════════════════════════════════
// Game Data Structures
// ════════════════════════════════════════════════════════════════

struct Asteroid {
    float x;
    float y;
    float radius;
    float speed;
};

struct Bullet {
    float x;
    float y;
    float width = 4.0f;
    float height = 15.0f;
    float speed = 800.0f;
};

struct GameData {
    float ship_x = 400.0f;
    float ship_y = 500.0f;
    float ship_size = 30.0f;
    
    std::vector<Asteroid> asteroids;
    std::vector<Bullet> bullets;
    int score = 0;
    bool game_over = false;
    bool is_shooting = false;
    float last_shot_time = 0.0f;
    
    // FPS tracking
    float current_fps = 0.0f;
    float game_time = 0.0f;
};

// ════════════════════════════════════════════════════════════════
// RenderGameWorld — Custom high-performance RenderObject
// ════════════════════════════════════════════════════════════════

class RenderGameWorld : public RenderBox {
public:
    const GameData* game_data = nullptr;
    sk_sp<SkRuntimeEffect> ship_effect_;
    sk_sp<SkRuntimeEffect> asteroid_effect_;
    sk_sp<SkRuntimeEffect> bg_effect_;

    RenderGameWorld() {
        YGNodeStyleSetFlexGrow(yogaNode(), 1.0f);
        YGNodeStyleSetWidthPercent(yogaNode(), 100.0f);
        YGNodeStyleSetHeightPercent(yogaNode(), 100.0f);
    }

    void initEffect() {
        if (!ship_effect_) {
            auto result = SkRuntimeEffect::MakeForShader(SkString(SHIP_SKSL));
            if (result.effect) ship_effect_ = result.effect;
            else fprintf(stderr, "SkSL Compile Error (Ship): %s\n", result.errorText.c_str());
        }
        if (!asteroid_effect_) {
            auto result = SkRuntimeEffect::MakeForShader(SkString(ASTEROID_SKSL));
            if (result.effect) asteroid_effect_ = result.effect;
            else fprintf(stderr, "SkSL Compile Error (Asteroid): %s\n", result.errorText.c_str());
        }
        if (!bg_effect_) {
            auto result = SkRuntimeEffect::MakeForShader(SkString(CUBE_LOVERS_SKSL));
            if (result.effect) bg_effect_ = result.effect;
            else fprintf(stderr, "SkSL Compile Error (Background): %s\n", result.errorText.c_str());
        }
    }

    bool hitTestSelf(Point position) const override {
        // Always return true so the entire game area is clickable
        return true;
    }

    void paint(PaintContext& context) override {
        if (!game_data) return;

        SkCanvas* canvas = static_cast<SkCanvas*>(context.canvas.getNativeHandle());
        if (!canvas) return;

        Rect bounds = Rect::fromPointSize(context.offset, size_);

        canvas->save();
        canvas->translate(bounds.x, bounds.y);

        // 1. Draw Background
        initEffect();
        if (bg_effect_) {
            struct Uniforms {
                float time;
                float res_x;
                float res_y;
            } uniforms;
            uniforms.time = game_data->game_time;
            uniforms.res_x = bounds.width;
            uniforms.res_y = bounds.height;

            sk_sp<SkData> uniformData = SkData::MakeWithCopy(&uniforms, sizeof(uniforms));
            sk_sp<SkShader> shader = bg_effect_->makeShader(std::move(uniformData), nullptr, 0);

            SkPaint bgPaint;
            bgPaint.setShader(shader);
            bgPaint.setAntiAlias(true);
            canvas->drawRect(SkRect::MakeWH(bounds.width, bounds.height), bgPaint);
        } else {
            SkPaint bgPaint;
            bgPaint.setColor(SkColorSetARGB(255, 15, 23, 42));
            canvas->drawRect(SkRect::MakeWH(bounds.width, bounds.height), bgPaint);
        }

        // 2. Draw Stars (Optional - simple static representation)
        // (Skipped for simplicity, but could be easily added)

        // 3. Draw Asteroids (Spiked Bombs) with Shader
        initEffect();
        if (asteroid_effect_) {
            for (const auto& a : game_data->asteroids) {
                float hw = a.radius * 1.5f; // Padding for spikes and glow
                float hh = a.radius * 1.5f;
                SkRect bound = SkRect::MakeLTRB(a.x - hw, a.y - hh, a.x + hw, a.y + hh);
                
                struct Uniforms {
                    float time;
                    float res_x;
                    float res_y;
                } uniforms;
                uniforms.time = game_data->game_time;
                uniforms.res_x = hw * 2.0f;
                uniforms.res_y = hh * 2.0f;

                sk_sp<SkData> uniformData = SkData::MakeWithCopy(&uniforms, sizeof(uniforms));
                sk_sp<SkShader> shader = asteroid_effect_->makeShader(std::move(uniformData), nullptr, 0);

                SkPaint astPaint;
                astPaint.setShader(shader);
                astPaint.setAntiAlias(true);

                canvas->save();
                canvas->translate(bound.left(), bound.top());
                canvas->drawRect(SkRect::MakeWH(bound.width(), bound.height()), astPaint);
                canvas->restore();
            }
        }

        // 3.5. Draw Bullets (Lasers)
        SkPaint laserPaint;
        laserPaint.setColor(SkColorSetARGB(255, 34, 197, 94)); // Emerald green
        laserPaint.setAntiAlias(true);
        SkPaint glowPaint;
        glowPaint.setColor(SkColorSetARGB(100, 34, 197, 94)); // Translucent glow
        glowPaint.setAntiAlias(true);

        for (const auto& b : game_data->bullets) {
            SkRect r = SkRect::MakeLTRB(b.x - b.width/2, b.y - b.height/2, b.x + b.width/2, b.y + b.height/2);
            canvas->drawRoundRect(r, 2.0f, 2.0f, laserPaint);
            
            SkRect glowR = SkRect::MakeLTRB(b.x - b.width, b.y - b.height, b.x + b.width, b.y + b.height);
            canvas->drawRoundRect(glowR, 4.0f, 4.0f, glowPaint);
        }

        // 4. Draw Player Ship with Shader
        initEffect();
        if (ship_effect_) {
            float hw = game_data->ship_size * 1.5f;
            float hh = game_data->ship_size * 1.5f;
            SkRect shipBounds = SkRect::MakeLTRB(
                game_data->ship_x - hw, game_data->ship_y - hh,
                game_data->ship_x + hw, game_data->ship_y + hh
            );

            struct Uniforms {
                float time;
                float res_x;
                float res_y;
            } uniforms;
            uniforms.time = game_data->game_time;
            uniforms.res_x = hw * 2.0f;
            uniforms.res_y = hh * 2.0f;

            sk_sp<SkData> uniformData = SkData::MakeWithCopy(&uniforms, sizeof(uniforms));
            sk_sp<SkShader> shader = ship_effect_->makeShader(std::move(uniformData), nullptr, 0);

            SkPaint shipPaint;
            shipPaint.setShader(shader);
            shipPaint.setAntiAlias(true);

            canvas->save();
            canvas->translate(shipBounds.left(), shipBounds.top());
            canvas->drawRect(SkRect::MakeWH(shipBounds.width(), shipBounds.height()), shipPaint);
            canvas->restore();
        }

        // 5. Draw Game Over text directly on canvas if needed (or we can use UI widgets)
        if (game_data->game_over) {
            SkPaint textPaint;
            textPaint.setColor(SK_ColorWHITE);
            // In a real game we'd use SkFont, but we'll overlay a TextWidget via Stack instead for better font handling.
        }

        canvas->restore();
    }
};

// ════════════════════════════════════════════════════════════════
// GameWorldWidget — The Widget wrapping the RenderObject
// ════════════════════════════════════════════════════════════════

class GameWorldWidget : public SingleChildRenderObjectWidget {
public:
    const GameData* data;

    explicit GameWorldWidget(const GameData* d) : data(d) {}

    [[nodiscard]] std::string_view typeName() const override { return "GameWorld"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderGameWorld>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rw = static_cast<RenderGameWorld&>(renderObject);
        rw.game_data = data;
        rw.markNeedsPaint(); // Force repaint every frame
    }
};

// ════════════════════════════════════════════════════════════════
// SpaceDodgerState — The Game Loop & Logic
// ════════════════════════════════════════════════════════════════

class SpaceDodgerWidget : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SpaceDodger"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SpaceDodgerState : public State {
public:
    GameData game;
    std::unique_ptr<Ticker> ticker_;
    std::chrono::steady_clock::time_point last_time_;
    
    // FPS Calculation
    int frame_count = 0;
    float time_accumulator = 0.0f;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> x_dist;
    std::uniform_real_distribution<float> radius_dist;
    std::uniform_real_distribution<float> speed_dist;

    SpaceDodgerState() 
        : rng(std::random_device{}())
        , x_dist(50.0f, 750.0f)
        , radius_dist(15.0f, 40.0f)
        , speed_dist(200.0f, 500.0f) {}

    void initState() override {
        State::initState();
        restartGame();
        
        last_time_ = std::chrono::steady_clock::now();
        ticker_ = createTicker([this]() { update(); });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        State::dispose();
    }

    void restartGame() {
        game.asteroids.clear();
        game.bullets.clear();
        game.score = 0;
        game.game_over = false;
        game.ship_x = 400.0f;
        game.is_shooting = false;
    }

    void update() {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_time_).count();
        last_time_ = now;
        
        // Calculate FPS
        frame_count++;
        time_accumulator += dt;
        game.game_time += dt;
        if (time_accumulator >= 1.0f) {
            game.current_fps = frame_count / time_accumulator;
            frame_count = 0;
            time_accumulator = 0.0f;
        }

        if (game.game_over) {
            setState([]{}); // Still need to render the game over screen
            return;
        }

        // 1. Spawn Asteroids
        // Cap the maximum number of asteroids to 10. Always try to maintain at least 2.
        if (game.asteroids.size() < 10) {
            float spawn_chance = dt * 2.0f; // Slower spawn rate
            std::uniform_real_distribution<float> chance(0.0f, 1.0f);
            if (chance(rng) < spawn_chance || game.asteroids.size() < 2) {
                // Slower acceleration curve, max speed cap
                float added_speed = std::min(game.score * 0.5f, 400.0f);
                
                game.asteroids.push_back({
                    .x = x_dist(rng),
                    .y = -50.0f,
                    .radius = radius_dist(rng),
                    .speed = speed_dist(rng) + added_speed 
                });
            }
        }

        // 2. Update Asteroids & Check Collisions with Ship
        for (auto it = game.asteroids.begin(); it != game.asteroids.end(); ) {
            it->y += it->speed * dt;
            
            // Collision detection (Simple circle to point distance for the ship nose)
            float dx = it->x - game.ship_x;
            float dy = it->y - (game.ship_y - game.ship_size);
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist < it->radius + 10.0f) {
                game.game_over = true;
            }

            // Remove if off screen
            if (it->y > 800.0f) {
                it = game.asteroids.erase(it);
                game.score += 10; // Point for surviving
            } else {
                ++it;
            }
        }

        // 3. Fire Bullets
        if (game.is_shooting && (game.game_time - game.last_shot_time > 0.15f)) { // 150ms fire rate
            game.bullets.push_back({
                .x = game.ship_x,
                .y = game.ship_y - game.ship_size // Fire from the nose
            });
            game.last_shot_time = game.game_time;
        }

        // 4. Update Bullets & Collisions with Asteroids
        for (auto bit = game.bullets.begin(); bit != game.bullets.end(); ) {
            bit->y -= bit->speed * dt;
            
            bool bullet_destroyed = false;
            for (auto ait = game.asteroids.begin(); ait != game.asteroids.end(); ) {
                float dx = bit->x - ait->x;
                float dy = bit->y - ait->y;
                float dist = std::sqrt(dx*dx + dy*dy);
                
                if (dist < ait->radius + bit->height/2) {
                    ait = game.asteroids.erase(ait); // Destroy asteroid
                    bullet_destroyed = true;
                    game.score += 5; // Points for shooting it down
                    break; 
                } else {
                    ++ait;
                }
            }
            
            if (bullet_destroyed || bit->y < -50.0f) {
                bit = game.bullets.erase(bit);
            } else {
                ++bit;
            }
        }

        // Trigger UI & Canvas update
        setState([]{});
    }

    WidgetPtr build(BuildContext& ctx) override {
        // We use a FlexBox as the root. It contains the GameWorld, HUD, and Overlay.
        // We wrap the entire root in a GestureDetector so it catches clicks even if the overlay blocks hit testing.

        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100);

        auto game_world = std::make_shared<GameWorldWidget>(&game);
        root->child(game_world);
        
        // ── UI Overlay ──
        
        // Score HUD
        auto hud = std::make_shared<FlexBox>();
        hud->positionType(YGPositionTypeAbsolute)
           .position(YGEdgeTop, 20)
           .position(YGEdgeLeft, 20)
           .flexDirection(YGFlexDirectionColumn);
           
        hud->child(text("SCORE: " + std::to_string(game.score), {
            .font_size = 32, .color = 0xFF10B981, .weight = TextStyle::Bold
        }));
        
        hud->child(text("FPS: " + std::to_string(static_cast<int>(game.current_fps)), {
            .font_size = 18, .color = 0xFF94A3B8
        }));

        root->child(hud);

        // Game Over Screen
        if (game.game_over) {
            auto overlay = std::make_shared<FlexBox>();
            overlay->positionType(YGPositionTypeAbsolute)
                   .position(YGEdgeAll, 0)
                   .widthPercent(100).heightPercent(100)
                   .justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter)
                   .backgroundColor(0x80000000); // Semi-transparent black

            overlay->child(text("GAME OVER", {
                .font_size = 64, .color = 0xFFEF4444, .weight = TextStyle::Bold
            }));
            
            overlay->child(text("Click anywhere to restart", {
                .font_size = 24, .color = 0xFFFFFFFF
            }));

            root->child(overlay);
        }

        // Wrap the entire screen in GestureDetector
        GestureDetectorConfig gd_config;
        gd_config.child = root;
        gd_config.on_tap_down = [this]() {
            if (game.game_over) {
                restartGame();
            } else {
                game.is_shooting = true;
            }
        };
        gd_config.on_tap_up = [this]() {
            game.is_shooting = false;
        };
        gd_config.on_pointer_move = [this](PointerEvent e) {
            if (!game.game_over) {
                game.ship_x = e.localPosition.x;
            }
        };
        
        return gesture_detector(gd_config);
    }
};

std::unique_ptr<State> SpaceDodgerWidget::createState() {
    return std::make_unique<SpaceDodgerState>();
}

// ════════════════════════════════════════════════════════════════
// Main Entry Point
// ════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    auto app_root = std::make_shared<SpaceDodgerWidget>();
    
    runApp(app_root, {
        .title = "VAURA Space Dodger (High Perf 2D)", 
        .width = 800, 
        .height = 600,
        .vsync = false, // Disable VSync
        .target_fps = 0 // Disable internal FPS capping
    });
    
    return 0;
}
