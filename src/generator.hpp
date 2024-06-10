#include <simgrid/s4u.hpp>
namespace sg4 = simgrid::s4u;

/** @brief */
class IGenerator {
  public:
    virtual std::string type() const { return"IGenerator"; }
    virtual void operator()() const = 0;
};

class SimpleGenerator : IGenerator{
  public:
    explicit SimpleGenerator(sg4::TaskPtr task) : source(task) {}
    std::string type() const override { return "SimpleGenerator"; }
    void operator()() const;

  private:
    sg4::TaskPtr source;
};

// class CSVGenerator : public IGenerator {};