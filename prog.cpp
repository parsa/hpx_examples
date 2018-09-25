#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>

#include <algorithm>
#include <numeric>
#include <vector>
#include <execution>


int hpx_main(int argc, char* argv[])
{
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    hpx::init(argc, argv);

    return 0;
}