from simgrid import Engine
from estresim import Job
import sys


if __name__ == "__main__":
    e = Engine(sys.argv)
    e.load_platform("platforms/three_multicore_hosts.xml")

    pm0 = e.host_by_name("PM0")
    A = Job.init("A", 1000000000, pm0)

    A.enqueue_firings(1, "instance_0")
