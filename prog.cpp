#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>

#include <algorithm>
#include <numeric>
#include <vector>
#include <execution>


namespace alg = hpx::parallel;

namespace functional {

    struct std_find_operation
    {
        template <typename TExecPolicy, typename TContainer, typename... TArgs>
        auto operator()(TExecPolicy ex, TContainer&& con, TArgs&&... args)
        {
            return std::find(ex, con.begin(), con.end(), std::forward<TArgs>(args)...);
        }
    };

    struct hpx_find_operation
    {
        template <typename TExecPolicy, typename TContainer, typename... TArgs>
        auto operator()(TExecPolicy ex, TContainer&& con, TArgs&&... args)
        {
            return alg::find(ex, con.begin(), con.end(), std::forward<TArgs>(args)...);
        }
    };
}

template <typename TFx1, typename TFx2, typename TContainer, typename... TArgs>
void run_and_measure(TFx1 fx1, TFx2 fx2, TContainer&& con, TArgs... args)
{
    float exec_time_seq_std{0.};
    {
        hpx::util::scoped_timer<float> t{exec_time_seq_std};
        fx1(std::execution::seq, con, args...);
    }
    float exec_time_seq_hpx{ 0. };
    {
        hpx::util::scoped_timer<float> t{exec_time_seq_hpx};
        fx2(alg::execution::seq, con, args...);
    }
    float exec_time_par_std{0.};
    {
        hpx::util::scoped_timer<float> t{exec_time_par_std};
        fx1(std::execution::par, con, args...);
    }
    float exec_time_par_hpx{0.};
    {
        hpx::util::scoped_timer<float> t{exec_time_par_hpx};
        fx2(alg::execution::par, con, args...);
    }
    float exec_time_par_unseq_std{0.};
    {
        hpx::util::scoped_timer<float> t{exec_time_par_unseq_std};
        fx1(std::execution::par_unseq, con, args...);
    }
    float exec_time_par_unseq_hpx{0.};
    {
        hpx::util::scoped_timer<float> t{exec_time_par_unseq_hpx};
        fx2(alg::execution::par_unseq, con, args...);
    }

    hpx::cout << exec_time_seq_std << ',' << exec_time_par_std << ','
              << exec_time_par_unseq_std << ',' << exec_time_seq_hpx << ','
              << exec_time_par_hpx << ',' << exec_time_par_unseq_hpx << '\n';
}

std::vector<int> generate_data()
{
    std::vector<int> v;
    v.reserve(100000);
    std::iota(v.begin(), v.end(), 1);

    return v;
}

int hpx_main(int argc, char* argv[])
{
    hpx::cout
        << "fx,std_seq,std_par,std_par_unseq,hpx_seq,hpx_par,hpx_par_unseq\n";

    auto v = generate_data();

    {
        run_and_measure(functional::std_find_operation{},
            functional::hpx_find_operation{}, v, 10);
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    hpx::init(argc, argv);

    return 0;
}