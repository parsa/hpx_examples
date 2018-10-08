#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/scoped_timer.hpp>
#include <hpx/util/transform_iterator.hpp>
#include <iterator>

#include <algorithm>
#include <numeric>
#include <vector>
#include <execution>

#include <boost/type_index.hpp>

// Reference: https://github.com/STEllAR-GROUP/hpx/tree/HEAD/tests/unit/util/iterator
struct adaptable_mult_functor
{
    using result_type = int;
    using argument_type = int;

    adaptable_mult_functor() {}

    template <typename Iterator>
    int operator()(Iterator b) const
    {
        std::cout << boost::typeindex::type_id_with_cvr<Iterator>().pretty_name() << "\n";
        return 2 * *b;
    }

    int a;
};

int hpx_main(int argc, char* argv[])
{
    int const N = 10;
    int x[N], y[N];
    std::iota(x, x + N, 1);

    std::copy(x, x + N, y);

    std::for_each(x, x + N, [](int& n) { n *= 2; });

    std::copy(x, x + N, std::ostream_iterator<int>{std::cout, " "});

    using iter_t = hpx::util::transform_iterator<int*, adaptable_mult_functor>;
    iter_t i(y, adaptable_mult_functor{});
    iter_t end = i + N;
    //auto f = [](int* x) { return 2 * *x; };
    //auto i = hpx::util::make_transform_iterator(&y[0], f);
    //auto end = hpx::util::make_transform_iterator(&y[0] + N, f);

    std::copy(i, end, std::ostream_iterator<int>{std::cout, " "});

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    hpx::init(argc, argv);

    return 0;
}