#include "common/messagethreadpool.h"
#include <memory>

#define UNUSED(x) (void)(x)

class PerfPool : public common::MessageThreadPool {
    protected:
        virtual void OnMessage(Message message) {
            UNUSED(message);
        }
};

int main(int argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);
    int messages = 1000000;
    auto pool = std::make_unique<PerfPool>();
    for (int i = 0; i < messages; i++) {
        pool->post(1);
    }
    pool->stopWhenEmpty();
    pool->join();
    return 0;
}