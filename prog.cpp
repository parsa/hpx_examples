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

#include <blaze/Math.h>


void hw(std::size_t a_rows, std::size_t a_cols,
    std::size_t b_rows, std::size_t b_cols);
HPX_PLAIN_ACTION(hw, hw_action);
void hw(std::size_t a_rows, std::size_t a_cols,
    std::size_t b_rows, std::size_t b_cols)
{
    blaze::Rand<blaze::DynamicMatrix<double>> r;
    auto const a = r.generate(a_rows, a_cols);
    auto const b = r.generate(b_rows, b_cols);

    std::cout << "A =\n" << a << "\n";
    std::cout << "B =\n" << b << "\n";

    auto const expected = a * b;
    std::cout << "Expected C =\n" << expected << "\n";

    {
        blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

        {
            for (int k = 0; k < a.columns(); ++k)
            {
                for (int i = 0; i < a.rows(); ++i)
                {
                    for (int j = 0; j < b.columns(); ++j)
                    {
                        c(i, j) += a(i, k) * b(k, j);
                    }
                }
            }
        }

        std::cout << "Actual C =\n" << c << "\n";
        std::cout << "Is expected == actual? " << std::boolalpha << (expected == c) << "\n";
    }
}

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t a_rows = vm["a.rows"].as<std::size_t>();
    std::size_t a_cols = vm["a.cols"].as<std::size_t>();
    std::size_t b_rows = vm["b.rows"].as<std::size_t>();
    std::size_t b_cols = vm["b.cols"].as<std::size_t>();

    hw_action h;
    h(hpx::find_here(), a_rows, a_cols, b_rows, b_cols);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc_cmdline(
    "Usage: " HPX_APPLICATION_STRING " [options]");
    desc_cmdline.add_options()("a.rows",
        boost::program_options::value<std::size_t>()->default_value(2),
        "number of rows for matrix A")("a.cols",
        boost::program_options::value<std::size_t>()->default_value(2),
        "number of columns for matrix A")("b.rows",
        boost::program_options::value<std::size_t>()->default_value(2),
        "number of rows for matrix B")("b.cols",
        boost::program_options::value<std::size_t>()->default_value(2),
        "number of columns for matrix B");

    return hpx::init(desc_cmdline, argc, argv);
}