#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>

#include <algorithm>
#include <execution>
#include <numeric>
#include <random>
#include <vector>

std::size_t hw_worker(std::size_t desired);
HPX_PLAIN_ACTION(hw_worker, hw_worker_action)
std::size_t hw_worker(std::size_t desired)
{
    std::size_t current = hpx::get_worker_thread_num();
    if (current == desired)
    {
        char const* msg = "os thread {1} on locality {2}\n";

        hpx::util::format_to(
            hpx::cout, msg, desired, hpx::get_locality_id())
            << hpx::flush;

        return desired;
    }

    return static_cast<std::size_t>(-1);
}

void hw();
HPX_PLAIN_ACTION(hw, hw_action);
void hw()
{
    std::size_t const os_threads = hpx::get_os_thread_count();
    hpx::naming::id_type const here = hpx::find_here();

    std::set<std::size_t> attendance;
    for (std::size_t os_thread = 0; os_thread < os_threads; ++os_thread)
        attendance.insert(os_thread);

    while (!attendance.empty())
    {
        std::vector<hpx::future<std::size_t>> futures;
        futures.reserve(attendance.size());

        for (std::size_t worker : attendance)
        {
            futures.push_back(hpx::async<hw_worker_action>(here, worker));
        }

        hpx::lcos::local::spinlock mtx;
        hpx::wait_each(
            hpx::util::unwrapping([&](std::size_t t){
            if (std::size_t(-1) != t)
            {
                std::lock_guard<hpx::lcos::local::spinlock> lk(mtx);
                attendance.erase(t);
            }
        }),
        futures);
    }
}

int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t n = vm["n-value"].as<std::uint64_t>();

    hw_action h;
    h(hpx::find_here());

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc_cmdline(
    "Usage: " HPX_APPLICATION_STRING " [options]");
    desc_cmdline.add_options()
        ("n-value", boost::program_options::value<std::uint64_t>()->default_value(10), "n value for the fibonacci function");

    return hpx::init(desc_cmdline, argc, argv);
}