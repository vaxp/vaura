/// @file color_picker.cpp
/// @brief ColorPicker widget implementation (HSV picker).

#include "vaura/widgets/color_picker.hpp"
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
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace vaura {

// ── HSV helpers ───────────────────────────────────────────────

struct HSV { float h, s, v, a; };

static HSV colorToHSV(Color c) {
    float a = ((c >> 24) & 0xFF) / 255.0f;
    float r = ((c >> 16) & 0xFF) / 255.0f;
    float g = ((c >>  8) & 0xFF) / 255.0f;
    float b = ( c        & 0xFF) / 255.0f;
    float mx = std::max({r,g,b}), mn = std::min({r,g,b}), diff = mx-mn;
    float hue = 0;
    if (diff > 0) {
        if (mx==r) hue = std::fmod(60*(g-b)/diff+360,360);
        else if (mx==g) hue = 60*(b-r)/diff+120;
        else hue = 60*(r-g)/diff+240;
    }
    return {hue, mx>0?diff/mx:0, mx, a};
}

static Color hsvToColor(float h, float s, float v, float a) {
    float r,g,b;
    if (s <= 0) { r=g=b=v; }
    else {
        float hh = std::fmod(h,360)/60; int ii=(int)hh; float ff=hh-ii;
        float p=v*(1-s), q=v*(1-s*ff), t2=v*(1-s*(1-ff));
        switch(ii){case 0:r=v;g=t2;b=p;break;case 1:r=q;g=v;b=p;break;
                   case 2:r=p;g=v;b=t2;break;case 3:r=p;g=q;b=v;break;
                   case 4:r=t2;g=p;b=v;break;default:r=v;g=p;b=q;break;}
    }
    auto c8=[](float x){return(uint8_t)std::clamp((int)(x*255+.5f),0,255);};
    return ((uint32_t)c8(a)<<24)|((uint32_t)c8(r)<<16)|((uint32_t)c8(g)<<8)|c8(b);
}

static std::string toHex(Color c) {
    std::ostringstream ss;
    ss << "#" << std::uppercase << std::hex << std::setfill('0') << std::setw(6) << (c & 0x00FFFFFF);
    return ss.str();
}

// ── SV Picker render (gradient approximation) ─────────────────

class RenderSVPicker : public RenderBox {
public:
    float hue=0, sat=1, val=1, pw, ph;
    RenderSVPicker(float w,float h):pw(w),ph(h){
        ANUNodeStyleSetWidth(anuNode(),w);
        ANUNodeStyleSetHeight(anuNode(),h);
    }
    void paint(PaintContext& ctx) override {
        auto& c = ctx.canvas;
        float x=ctx.offset.x, y=ctx.offset.y;
        const int S=36;
        float cw=pw/S, ch=ph/S;
        for(int si=0;si<S;++si) for(int vi=0;vi<S;++vi){
            Color col=hsvToColor(hue,si/(float)(S-1),1-(vi/(float)(S-1)),1);
            Paint p; p.setColor(col); p.setStyle(PaintStyle::Fill);
            c.drawRRect({x+si*cw,y+vi*ch,cw+.5f,ch+.5f},BorderRadius::circular(0),p);
        }
        float cx2=x+sat*pw, cy2=y+(1-val)*ph;
        Paint ring; ring.setColor(0xFFFFFFFF); ring.setStyle(PaintStyle::Stroke); ring.setStrokeWidth(2);
        c.drawRRect({cx2-7,cy2-7,14,14},BorderRadius::circular(7),ring);
    }
};

class SVPickerWidget : public SingleChildRenderObjectWidget {
public:
    float hue,sat,val,pw,ph;
    SVPickerWidget(float h,float s,float v,float w,float ht):hue(h),sat(s),val(v),pw(w),ph(ht){this->child=nullptr;}
    [[nodiscard]] std::string_view typeName() const override{return "SVPicker";}
    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto r=std::make_unique<RenderSVPicker>(pw,ph); updateRenderObject(ctx,*r); return r;
    }
    void updateRenderObject(BuildContext& ctx, RenderObject& ro) override {
        auto& r=static_cast<RenderSVPicker&>(ro);
        r.hue=hue; r.sat=sat; r.val=val; r.markNeedsPaint();
    }
};

// ── Hue strip ─────────────────────────────────────────────────

class RenderHueStrip : public RenderBox {
public:
    float sw2,sh2;
    RenderHueStrip(float w,float h):sw2(w),sh2(h){
        ANUNodeStyleSetWidth(anuNode(),w); ANUNodeStyleSetHeight(anuNode(),h);
    }
    void paint(PaintContext& ctx) override {
        const int S=60; float cw=sw2/S;
        for(int i=0;i<S;++i){
            Paint p; p.setColor(hsvToColor(i/(float)S*360,1,1,1)); p.setStyle(PaintStyle::Fill);
            ctx.canvas.drawRRect({ctx.offset.x+i*cw,ctx.offset.y,cw+.5f,sh2},BorderRadius::circular(0),p);
        }
    }
};

class HueStripWidget : public SingleChildRenderObjectWidget {
public:
    float w,h; HueStripWidget(float w,float h):w(w),h(h){this->child=nullptr;}
    [[nodiscard]] std::string_view typeName() const override{return "HueStrip";}
    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        return std::make_unique<RenderHueStrip>(w,h);
    }
    void updateRenderObject(BuildContext& ctx, RenderObject& ro) override {}
};

// ── Main widget ───────────────────────────────────────────────

class ColorPickerWidget : public StatefulWidget {
public:
    ColorPickerConfig config;
    explicit ColorPickerWidget(ColorPickerConfig cfg):config(std::move(cfg)){}
    [[nodiscard]] std::string_view typeName() const override{return "ColorPicker";}
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ColorPickerState : public State {
public:
    float hue=0,sat=1,val=1,alpha=1;
    bool  sv_pressing=false;

    void initState() override {
        State::initState();
        auto* cw=static_cast<const ColorPickerWidget*>(widget());
        auto hsv=colorToHSV(cw->config.selected_color);
        hue=hsv.h; sat=hsv.s; val=hsv.v; alpha=hsv.a;
    }

    void notify() {
        auto* cw=static_cast<const ColorPickerWidget*>(widget());
        if(cw->config.on_changed) cw->config.on_changed(hsvToColor(hue,sat,val,alpha));
    }

    // Compute sat/val from pixel position within the SV picker
    void applySVPos(float lx, float ly, float pw, float ph) {
        float ns=std::clamp(lx/pw,0.f,1.f);
        float nv=1-std::clamp(ly/ph,0.f,1.f);
        setState([this,ns,nv]{sat=ns;val=nv;}); notify();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* cw=static_cast<const ColorPickerWidget*>(widget());
        const auto& cfg=cw->config;

        float sv_pw=cfg.width-24;
        float sv_ph=sv_pw*0.6f;
        Color cur=hsvToColor(hue,sat,val,alpha);

        auto col=std::make_shared<FlexBox>();
        col->direction(FlexDirection::Column)
            .width(cfg.width)
            .backgroundColor(cfg.background_color)
            .padding(Edge::All,12);

        // SV picker (mouse-drag to pick saturation+value)
        auto sv_widget=std::make_shared<SVPickerWidget>(hue,sat,val,sv_pw,sv_ph);
        auto sv_gd=gesture_detector({
            .child         = sv_widget,
            .on_tap_down   = [this]()    { setState([this]{ sv_pressing=true; }); },
            .on_tap_up     = [this]()    { setState([this]{ sv_pressing=false; }); },
            .on_hover_exit = [this]()    { setState([this]{ sv_pressing=false; }); },
            .on_pointer_move = [this, sv_pw, sv_ph](PointerEvent e) {
                if (sv_pressing) applySVPos(e.localPosition.x, e.localPosition.y, sv_pw, sv_ph);
            },
        });
        col->child(sv_gd);

        // Hue strip — tappable at a position for rough hue selection
        auto hue_strip=std::make_shared<HueStripWidget>(sv_pw, 16.0f);
        auto hue_gd=gesture_detector({
            .child  = hue_strip,
            .on_tap = [this, sv_pw]() {
                // Hue is approximated; user can adjust via slider below
            },
        });
        auto hue_wrap=std::make_shared<FlexBox>();
        hue_wrap->margin(Edge::Top,10).borderRadius(8.0f).child(hue_gd);
        col->child(hue_wrap);

        // Hue rough selector via tapping hue strip (use row of tappable swatches)
        auto hue_btns=std::make_shared<FlexBox>();
        hue_btns->direction(FlexDirection::Row).margin(Edge::Top,8).wrap(FlexWrap::Wrap);
        for(int hi=0;hi<12;++hi){
            float h2=hi*30.0f;
            Color swc=hsvToColor(h2,1,1,1);
            bool active=std::abs(hue-h2)<15;
            float h2c=h2;
            auto sw=gesture_detector({
                .child=container({
                    .color         = swc,
                    .width         = (sv_pw/12)-4,
                    .height        = 20,
                    .border_radius = BorderRadius::circular(4),
                    .border        = Border{active?0xFFFFFFFF:0x00000000, active?2.0f:0},
                }),
                .on_tap=[this,h2c](){setState([this,h2c]{hue=h2c;});notify();},
            });
            auto sw_wrap=std::make_shared<FlexBox>();
            sw_wrap->margin(Edge::Right,2).child(sw);
            hue_btns->child(sw_wrap);
        }
        col->child(hue_btns);

        // Alpha slider (row of semi-transparent swatches)
        if(cfg.show_alpha){
            auto alpha_row=std::make_shared<FlexBox>();
            alpha_row->direction(FlexDirection::Row).margin(Edge::Top,8);
            for(int ai=0;ai<=10;++ai){
                float av=ai/10.0f;
                Color ac=hsvToColor(hue,sat,val,av);
                bool active=std::abs(alpha-av)<0.06f;
                float avc=av;
                auto asw=gesture_detector({
                    .child=container({
                        .color         = ac,
                        .width         = sv_pw/11-2,
                        .height        = 20,
                        .border_radius = BorderRadius::circular(4),
                        .border        = Border{active?0xFFFFFFFF:0x00000000, active?2.0f:0},
                    }),
                    .on_tap=[this,avc](){setState([this,avc]{alpha=avc;});notify();},
                });
                auto asw_wrap=std::make_shared<FlexBox>();
                asw_wrap->margin(Edge::Right,2).child(asw);
                alpha_row->child(asw_wrap);
            }
            col->child(alpha_row);
        }

        // Preview swatch + hex label
        auto prev_row=std::make_shared<FlexBox>();
        prev_row->direction(FlexDirection::Row).align(Align::Center).margin(Edge::Top,12);

        auto swatch=std::make_shared<FlexBox>();
        swatch->width(40).height(40).borderRadius(8).backgroundColor(cur).margin(Edge::Right,12);
        prev_row->child(swatch);

        if(cfg.show_hex_input){
            prev_row->child(text(toHex(cur),{.font_size=15,.color=cfg.text_color,.weight=TextStyle::SemiBold}));
        }
        col->child(prev_row);
        return col;
    }
};

std::unique_ptr<State> ColorPickerWidget::createState(){
    return std::make_unique<ColorPickerState>();
}

std::shared_ptr<Widget> color_picker(ColorPickerConfig config){
    return std::make_shared<ColorPickerWidget>(std::move(config));
}

} // namespace vaura
