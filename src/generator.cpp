#include "generator.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(generator, "generator logs");

void SimpleGenerator::operator()() const {
    for (int i = 0; i < 2; ++i) {
        sg4::this_actor::sleep_for(10);
        this->source->enqueue_firings(1);
    }
}