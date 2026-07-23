/// @file time_picker.cpp
/// @brief TimePicker widget — interactive clock face + hue/minute indicators.

#include "vaura/widgets/time_picker.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/tree/render_object.hpp"
#include <layout_engine/Anu.h>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace vaura {

// ── Clock face render ─────────────────────────────────────────

class RenderClockFace : public RenderBox {
public:
    int   hour=12, minute=0;
    Color face_color    = 0xFF0F172A;
    Color hand_color    = 0xFF0EA5E9;
    Color center_color  = 0xFF0EA5E9;
    Color num_color     = 0xFFF8FAFC;
    Color sel_color     = 0xFF0EA5E9;
    float sz            = 200.0f;

    explicit RenderClockFace(float s):sz(s){
        ANUNodeStyleSetWidth(anuNode(),s);
        ANUNodeStyleSetHeight(anuNode(),s);
    }

    void paint(PaintContext& ctx) override {
        auto& c = ctx.canvas;
        float cx = ctx.offset.x + sz/2, cy = ctx.offset.y + sz/2;
        float r  = sz/2 - 4;

        // Face
        Paint fp; fp.setColor(face_color); fp.setStyle(PaintStyle::Fill);
        c.drawRRect({ctx.offset.x+2,ctx.offset.y+2,sz-4,sz-4}, BorderRadius::circular(r), fp);

        static const float PI = 3.14159265f;

        // Hour numbers
        for (int h=1; h<=12; ++h) {
            float angle = (h/12.0f)*2*PI - PI/2;
            float nx = cx + (r-20)*std::cos(angle);
            float ny = cy + (r-20)*std::sin(angle);
            bool sel = (h == (hour%12==0?12:hour%12));
            Paint tp; tp.setColor(sel ? sel_color : num_color); tp.setStyle(PaintStyle::Fill);
            float fs = 10.5f;
            // Draw number at approximate center position
            c.drawText(std::to_string(h), {nx-6, ny-fs*0.5f}, tp, fs);
        }

        // Hour hand
        float ha = ((hour%12)/12.0f + minute/720.0f)*2*PI - PI/2;
        Paint hh; hh.setColor(hand_color); hh.setStyle(PaintStyle::Stroke);
        hh.setStrokeWidth(3); hh.setStrokeCap(StrokeCap::Round);
        Path hp; hp.moveTo(cx,cy); hp.lineTo(cx+(r*0.50f)*std::cos(ha), cy+(r*0.50f)*std::sin(ha));
        c.drawPath(hp, hh);

        // Minute hand
        float ma = (minute/60.0f)*2*PI - PI/2;
        Paint mh; mh.setColor(hand_color); mh.setStyle(PaintStyle::Stroke);
        mh.setStrokeWidth(2); mh.setStrokeCap(StrokeCap::Round);
        Path mp; mp.moveTo(cx,cy); mp.lineTo(cx+(r*0.72f)*std::cos(ma), cy+(r*0.72f)*std::sin(ma));
        c.drawPath(mp, mh);

        // Center dot
        Paint dp; dp.setColor(center_color); dp.setStyle(PaintStyle::Fill);
        c.drawRRect({cx-5,cy-5,10,10}, BorderRadius::circular(5), dp);
    }
};

class ClockFaceWidget : public SingleChildRenderObjectWidget {
public:
    int hour, minute; float sz;
    Color face_color, hand_color, center_color, num_color, sel_color;

    ClockFaceWidget(int h, int m, float s,
                    Color fc, Color hc, Color cc, Color nc, Color sc)
        : hour(h), minute(m), sz(s),
          face_color(fc), hand_color(hc), center_color(cc), num_color(nc), sel_color(sc)
    { this->child = nullptr; }

    [[nodiscard]] std::string_view typeName() const override { return "ClockFace"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto r = std::make_unique<RenderClockFace>(sz);
        updateRenderObject(ctx, *r); return r;
    }
    void updateRenderObject(BuildContext& ctx, RenderObject& ro) override {
        auto& r = static_cast<RenderClockFace&>(ro);
        r.hour=hour; r.minute=minute; r.sz=sz;
        r.face_color=face_color; r.hand_color=hand_color;
        r.center_color=center_color; r.num_color=num_color; r.sel_color=sel_color;
        r.markNeedsPaint();
    }
};

// ── Widget ─────────────────────────────────────────────────────

class TimePickerWidget : public StatefulWidget {
public:
    TimePickerConfig config;
    explicit TimePickerWidget(TimePickerConfig cfg):config(std::move(cfg)){}
    [[nodiscard]] std::string_view typeName() const override{return "TimePicker";}
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class TimePickerState : public State {
public:
    int cur_hour=0, cur_minute=0;

    void initState() override {
        State::initState();
        auto* tw=static_cast<const TimePickerWidget*>(widget());
        cur_hour=tw->config.hour; cur_minute=tw->config.minute;
    }

    void notify(){
        auto* tw=static_cast<const TimePickerWidget*>(widget());
        if(tw->config.on_changed) tw->config.on_changed(cur_hour,cur_minute);
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* tw=static_cast<const TimePickerWidget*>(widget());
        const auto& cfg=tw->config;

        int disp_h = cfg.use_24h ? cur_hour : (cur_hour%12==0?12:cur_hour%12);
        std::string am_pm = cur_hour<12 ? "AM" : "PM";
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << disp_h << ":" << std::setw(2) << cur_minute;
        if (!cfg.use_24h) ss << " " << am_pm;

        auto col = std::make_shared<FlexBox>();
        col->direction(FlexDirection::Column).align(Align::Center);

        // Time display
        col->child(text(ss.str(), {.font_size=32,.color=cfg.text_color,
                                    .weight=TextStyle::Bold,.align=TextStyle::Center}));

        // Clock face
        auto face = std::make_shared<ClockFaceWidget>(
            cur_hour, cur_minute, cfg.clock_size,
            cfg.face_color, cfg.hand_color, cfg.center_dot_color, cfg.text_color, cfg.selected_text);
        auto fw = std::make_shared<FlexBox>();
        fw->margin(Edge::Top,16).margin(Edge::Bottom,12).child(face);
        col->child(fw);

        // Hour buttons (0–23 or 0–11 in two rows)
        col->child(text("Hour", {.font_size=11,.color=0xFF64748B}));
        int max_h = cfg.use_24h ? 23 : 11;
        auto hr_row = std::make_shared<FlexBox>();
        hr_row->direction(FlexDirection::Row).wrap(FlexWrap::Wrap)
               .width(cfg.clock_size).margin(Edge::Top,4);
        for (int h=0; h<=max_h; ++h) {
            bool active = (h == cur_hour || (!cfg.use_24h && h==cur_hour%12));
            int hc=h;
            auto hbtn = gesture_detector({
                .child = container({
                    .color         = active ? cfg.hand_color : 0xFF334155,
                    .width         = cfg.clock_size/(max_h<12?12:12) - 3,
                    .height        = 28,
                    .border_radius = BorderRadius::circular(4),
                    .child         = text(std::to_string(h),
                                         {.font_size=10,.color=0xFFFFFFFF,.align=TextStyle::Center}),
                }),
                .on_tap = [this, hc](){ setState([this,hc]{cur_hour=hc;}); notify(); },
            });
            auto bw=std::make_shared<FlexBox>(); bw->margin(Edge::Right,2).margin(Edge::Bottom,2).child(hbtn);
            hr_row->child(bw);
        }
        col->child(hr_row);

        // Minute buttons (0,5,10,...,55)
        col->child(text("Minute", {.font_size=11,.color=0xFF64748B}));
        auto min_row = std::make_shared<FlexBox>();
        min_row->direction(FlexDirection::Row).wrap(FlexWrap::Wrap)
                .width(cfg.clock_size).margin(Edge::Top,4);
        for (int m=0; m<=55; m+=5) {
            bool active = (cur_minute/5*5 == m);
            int mc=m;
            auto mbtn = gesture_detector({
                .child = container({
                    .color         = active ? cfg.hand_color : 0xFF334155,
                    .width         = cfg.clock_size/12 - 3,
                    .height        = 28,
                    .border_radius = BorderRadius::circular(4),
                    .child         = text(std::to_string(m),
                                         {.font_size=10,.color=0xFFFFFFFF,.align=TextStyle::Center}),
                }),
                .on_tap=[this,mc](){ setState([this,mc]{cur_minute=mc;}); notify(); },
            });
            auto bw=std::make_shared<FlexBox>(); bw->margin(Edge::Right,2).margin(Edge::Bottom,2).child(mbtn);
            min_row->child(bw);
        }
        col->child(min_row);

        // AM/PM toggle
        if (!cfg.use_24h) {
            auto ampm=std::make_shared<FlexBox>();
            ampm->direction(FlexDirection::Row).margin(Edge::Top,10);
            for (const char* lbl : {"AM","PM"}) {
                bool active=(std::string(lbl)==am_pm);
                std::string l=lbl;
                auto btn=gesture_detector({
                    .child=container({
                        .color=active?cfg.hand_color:0xFF334155,
                        .padding=EdgeInsets{8,20,8,20},
                        .border_radius=BorderRadius::circular(6),
                        .child=text(l,{.color=0xFFFFFFFF,.weight=TextStyle::SemiBold}),
                    }),
                    .on_tap=[this,l,cfg](){
                        if(l=="AM"&&cur_hour>=12){cur_hour-=12;notify();}
                        if(l=="PM"&&cur_hour<12) {cur_hour+=12;notify();}
                        setState([]{});
                    },
                });
                auto bw=std::make_shared<FlexBox>();bw->margin(Edge::Right,8).child(btn);
                ampm->child(bw);
            }
            col->child(ampm);
        }

        return container({
            .color         = cfg.background_color,
            .padding       = EdgeInsets::all(20),
            .border_radius = BorderRadius::circular(16),
            .child         = col,
        });
    }
};

std::unique_ptr<State> TimePickerWidget::createState(){
    return std::make_unique<TimePickerState>();
}

std::shared_ptr<Widget> time_picker(TimePickerConfig config){
    return std::make_shared<TimePickerWidget>(std::move(config));
}

} // namespace vaura
