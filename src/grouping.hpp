#ifndef GROUPING_HPP
#define GROUPING_HPP

#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

class IGrouping {
    public:
        virtual void prepare(sg4::ExecTaskPtr task) = 0;
        virtual std::string chooseInstance() = 0;
};

class ShuffleGrouping : public IGrouping {
    public:
        void prepare(sg4::ExecTaskPtr task) override;
        std::string chooseInstance() override;
    private:
        sg4::ExecTaskPtr task;
        int current_host = 0;
};

#endif 