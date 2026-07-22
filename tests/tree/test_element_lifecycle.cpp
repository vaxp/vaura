#include "vaura/tree/widget.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/tree/build_context.hpp"
#include <cassert>

using namespace vaura;

class ReconcilerTestWidget : public StatelessWidget {
public:
    WidgetPtr build(BuildContext& ctx) override {
        return nullptr;
    }
    std::string_view typeName() const override { return "ReconcilerTestWidget"; }
};

void test_element_lifecycle() {
    auto w = std::make_shared<ReconcilerTestWidget>();
    auto e = w->createElement();
    
    assert(e->lifecycle() == ElementLifecycle::Initial);
    
    // Simulate mount
    e->mount(nullptr, 0);
    assert(e->lifecycle() == ElementLifecycle::Active);
    
    // Simulate deactivate
    e->deactivate();
    assert(e->lifecycle() == ElementLifecycle::Inactive);
    
    // Simulate unmount
    e->unmount();
    assert(e->lifecycle() == ElementLifecycle::Defunct);
}

int main() {
    test_element_lifecycle();
    return 0;
}
