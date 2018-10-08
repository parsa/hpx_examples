#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>
#include <hpx/util/transform_iterator.hpp>
#include <iterator>

#include <algorithm>
#include <array>
#include <execution>
#include <numeric>
#include <vector>

#include <boost/type_index.hpp>

int const N = 10;

// Reference: https://github.com/STEllAR-GROUP/hpx/tree/HEAD/tests/unit/util/iterator
struct adaptable_mult_functor
{
    adaptable_mult_functor() {}

    template <typename Iterator>
    int operator()(Iterator b) const
    {
        std::cout << boost::typeindex::type_id_with_cvr<Iterator>().pretty_name() << "\n";
        return 2 * *b;
    }
};

std::vector<int> fill_array()
{
    std::vector<int> x(10);

    std::iota(x.begin(), x.end(), 1);

    std::copy(x.begin(), x.end(), std::ostream_iterator<int>{std::cout, " "});

    std::cout << "\n";

    return x;
}

void transform_iterator_function_object()
{
    std::vector<int> x = fill_array();

    using iter_t = hpx::util::transform_iterator<std::vector<int>::iterator,
        adaptable_mult_functor>;
    iter_t i(x.begin(), adaptable_mult_functor{});
    iter_t end = i + N;

    std::copy(i, end, std::ostream_iterator<int>{std::cout, " "});

    std::cout << "\n";
}

void transform_iterator_lambda()
{
    std::vector<int> x = fill_array();

    auto i = hpx::util::make_transform_iterator(
        x.begin(), [](std::vector<int>::iterator x) { return 2 * *x; });
    auto end = i + N;

    std::copy(i, end, std::ostream_iterator<int>{std::cout, " "});

    std::cout << "\n";
}

int hpx_main(int argc, char* argv[])
{
    transform_iterator_function_object();
    transform_iterator_lambda();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    hpx::init(argc, argv);

    return 0;
}