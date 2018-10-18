#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>

#include <algorithm>
#include <cmath>
#include <execution>
#include <numeric>
#include <iomanip>
#include <locale>
#include <random>
#include <vector>

#include <blaze/Math.h>


void hw(std::size_t a_rows, std::size_t a_cols,
    std::size_t b_rows, std::size_t b_cols);
HPX_PLAIN_ACTION(hw, hw_action);


blaze::DynamicMatrix<double> mul_ijk(blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

    for (int i = 0; i < a.rows(); ++i)
    {
        for (int j = 0; j < b.columns(); ++j)
        {
            for (int k = 0; k < a.columns(); ++k)
            {
                c(i, j) += a(i, k) * b(k, j);
            }
        }
    }

    return c;
}

blaze::DynamicMatrix<double> mul_jik(blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

    for (int j = 0; j < b.columns(); ++j)
    {
        for (int i = 0; i < a.rows(); ++i)
        {
            for (int k = 0; k < a.columns(); ++k)
            {
                c(i, j) += a(i, k) * b(k, j);
            }
        }
    }

    return c;
}

blaze::DynamicMatrix<double> mul_kij(blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

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

    return c;
}

blaze::DynamicMatrix<double> mul_kji(blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

    for (int k = 0; k < a.columns(); ++k)
    {
        for (int j = 0; j < b.columns(); ++j)
        {
            for (int i = 0; i < a.rows(); ++i)
            {
                c(i, j) += a(i, k) * b(k, j);
            }
        }
    }

    return c;
}

blaze::DynamicMatrix<double> mul_summa_serial(
    blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

    {
        // for k := 0 to n - 1
        for (int k = 0; k < a.columns(); ++k)
        {
            // C[:, :] += A[:, k] * B[k, :]
            for (int i = 0; i < a.rows(); ++i)
                for (int j = 0; j < b.columns(); ++j)
                {
                    c(i, j) += a(i, k) * b(k, j);
                }
        }
    }

    return c;
}

blaze::DynamicMatrix<double> mul_summa_parallel(
    blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b)
{
    blaze::DynamicMatrix<double> c(a.rows(), b.columns(), 0);

    {
        //std::size_t const os_threads = hpx::get_os_thread_count();
        std::size_t const block_size = 40;  //std::sqrt(os_threads);

        // for k := 0 to n - 1 by b
        for (int k = 0; k < a.columns(); k += block_size)
        {
            // Owner of A[:, k:k + b - 1] broadcast to column_block_A
            // Owner of B[k:k + b - 1, :] broadcast to row_block_B

            // C += Serial Matrix Multiply(column_block_A, row_block_B)
            // C[:, :] += A[:, k] * B[k, :]
            for (int i = 0; i < a.rows(); i += block_size)
                for (int j = 0; j < b.columns(); j += block_size)
                {
                    blaze::submatrix(c, i, j, block_size, block_size) +=
                        blaze::submatrix(a, i, k, block_size, block_size) *
                        blaze::submatrix(b, k, j, block_size, block_size);
                }
        }
    }

    return c;
}

void test_case(std::string title,
    blaze::DynamicMatrix<double> const& expected,
    blaze::DynamicMatrix<double> const& a,
    blaze::DynamicMatrix<double> const& b,
    blaze::DynamicMatrix<double> (*fx)(
        blaze::DynamicMatrix<double> const& a,
        blaze::DynamicMatrix<double> const& b))
{
    hpx::util::high_resolution_timer t;
    auto actual = fx(a, b);
    std::int64_t elapsed = t.elapsed_nanoseconds();

    std::cout.imbue(std::locale(""));

    std::cout << std::right
              << std::setw(20) << title << ", "
              << std::setw(4)  << std::boolalpha << (expected == actual) << ", "
              << std::setw(10)  << elapsed << "ns\n";
}

void hw(std::size_t a_rows, std::size_t a_cols,
    std::size_t b_rows, std::size_t b_cols)
{
    blaze::Rand<blaze::DynamicMatrix<double>> r;
    auto const a = r.generate(a_rows, a_cols);
    auto const b = r.generate(b_rows, b_cols);

    //std::cout << "A =\n" << a << "\n";
    //std::cout << "B =\n" << b << "\n";

    auto const expected = a * b;
    //std::cout << "Expected C =\n" << expected << "\n";

    //std::cout << std::string(80, '=') << "\n";;

    test_case("ijk", expected, a, b, mul_ijk);
    test_case("jik", expected, a, b, mul_jik);
    test_case("kij", expected, a, b, mul_kij);
    test_case("kji", expected, a, b, mul_kji);
    test_case("SUMMA (Serial)", expected, a, b, mul_summa_serial);
    test_case("SUMMA (Parallel)", expected, a, b, mul_summa_parallel);
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