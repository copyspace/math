#define BOOST_TEST_MODULE barycentric_rational

#include <random>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/type_index.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/interpolators/barycentric_rational.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#ifdef BOOST_HAS_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif

using boost::multiprecision::cpp_bin_float_50;

template<class Real>
void test_interpolation_condition()
{
    std::cout << "Testing interpolation condition for barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name()  << "\n";
    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.1f, 1);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    x[0] = dis(gen);
    y[0] = dis(gen);
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = dis(gen);
    }

    boost::math::barycentric_rational<Real> interpolator(x.data(), y.data(), y.size());

    for (size_t i = 0; i < x.size(); ++i)
    {
        auto z = interpolator(x[i]);
        BOOST_CHECK_CLOSE(z, y[i], 100*std::numeric_limits<Real>::epsilon());
    }
}

template<class Real>
void test_interpolation_condition_high_order()
{
    std::cout << "Testing interpolation condition in high order for barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name()  << "\n";
    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.1f, 1);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    x[0] = dis(gen);
    y[0] = dis(gen);
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = dis(gen);
    }

    // Order 5 approximation:
    boost::math::barycentric_rational<Real> interpolator(x.data(), y.data(), y.size(), 5);

    for (size_t i = 0; i < x.size(); ++i)
    {
        auto z = interpolator(x[i]);
        BOOST_CHECK_CLOSE(z, y[i], 100*std::numeric_limits<Real>::epsilon());
    }
}


template<class Real>
void test_constant()
{
    std::cout << "Testing that constants are interpolated correctly using barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name() << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.1f, 1);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    Real constant = -8;
    x[0] = dis(gen);
    y[0] = constant;
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = y[0];
    }

    boost::math::barycentric_rational<Real> interpolator(x.data(), y.data(), y.size());

    for (size_t i = 0; i < x.size(); ++i)
    {
        // Don't evaluate the constant at x[i]; that's already tested in the interpolation condition test.
        auto z = interpolator(x[i] + dis(gen));
        BOOST_CHECK_CLOSE(z, constant, 100*sqrt(std::numeric_limits<Real>::epsilon()));
    }
}

template<class Real>
void test_constant_high_order()
{
    std::cout << "Testing that constants are interpolated correctly in high order using barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name() << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.1f, 1);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    Real constant = 5;
    x[0] = dis(gen);
    y[0] = constant;
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = y[0];
    }

    // Set interpolation order to 7:
    boost::math::barycentric_rational<Real> interpolator(x.data(), y.data(), y.size(), 7);

    for (size_t i = 0; i < x.size(); ++i)
    {
        auto z = interpolator(x[i] + dis(gen));
        BOOST_CHECK_CLOSE(z, constant, 1000*sqrt(std::numeric_limits<Real>::epsilon()));
    }
}


template<class Real>
void test_runge()
{
    std::cout << "Testing interpolation of Runge's 1/(1+25x^2) function using barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name() << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.005f, 0.01f);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    x[0] = -2;
    y[0] = 1/(1+25*x[0]*x[0]);
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = 1/(1+25*x[i]*x[i]);
    }

    boost::math::barycentric_rational<Real> interpolator(x.data(), y.data(), y.size(), 5);

    for (size_t i = 0; i < x.size(); ++i)
    {
        auto t = x[i] + dis(gen);
        auto z = interpolator(t);
        BOOST_CHECK_CLOSE(z, 1/(1+25*t*t), 0.02);
    }
}

template<class Real>
void test_weights()
{
    std::cout << "Testing weights are calculated correctly using barycentric interpolation on type " << boost::typeindex::type_id<Real>().pretty_name() << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    boost::random::uniform_real_distribution<Real> dis(0.005, 0.01);
    std::vector<Real> x(500);
    std::vector<Real> y(500);
    x[0] = -2;
    y[0] = 1/(1+25*x[0]*x[0]);
    for (size_t i = 1; i < x.size(); ++i)
    {
        x[i] = x[i-1] + dis(gen);
        y[i] = 1/(1+25*x[i]*x[i]);
    }

    boost::math::detail::barycentric_rational_imp<Real> interpolator(x.data(), x.data() + x.size(), y.data(), 0);

    for (size_t i = 0; i < x.size(); ++i)
    {
        auto w = interpolator.weight(i);
        if (i % 2 == 0)
        {
            BOOST_CHECK_CLOSE(w, 1, 0.00001);
        }
        else
        {
            BOOST_CHECK_CLOSE(w, -1, 0.00001);
        }
    }

    // d = 1:
    interpolator = boost::math::detail::barycentric_rational_imp<Real>(x.data(), x.data() + x.size(), y.data(), 1);

    for (size_t i = 1; i < x.size() -1; ++i)
    {
        auto w = interpolator.weight(i);
        auto w_expect = 1/(x[i] - x[i - 1]) + 1/(x[i+1] - x[i]);
        if (i % 2 == 0)
        {
            BOOST_CHECK_CLOSE(w, -w_expect, 0.00001);
        }
        else
        {
            BOOST_CHECK_CLOSE(w, w_expect, 0.00001);
        }
    }

}


BOOST_AUTO_TEST_CASE(barycentric_rational)
{
    test_weights<double>();
    test_constant<float>();
    test_constant<double>();
    test_constant<long double>();
    test_constant<cpp_bin_float_50>();

    test_constant_high_order<float>();
    test_constant_high_order<double>();
    test_constant_high_order<long double>();
    test_constant_high_order<cpp_bin_float_50>();

    test_interpolation_condition<float>();
    test_interpolation_condition<double>();
    test_interpolation_condition<long double>();
    test_interpolation_condition<cpp_bin_float_50>();

    test_interpolation_condition_high_order<float>();
    test_interpolation_condition_high_order<double>();
    test_interpolation_condition_high_order<long double>();
    test_interpolation_condition_high_order<cpp_bin_float_50>();

    test_runge<float>();
    test_runge<double>();
    test_runge<long double>();
    test_runge<cpp_bin_float_50>();

#ifdef BOOST_HAS_FLOAT128
    test_interpolation_condition<boost::multiprecision::float128>();
        test_constant<boost::multiprecision::float128>();
        test_constant_high_order<boost::multiprecision::float128>();
        test_interpolation_condition_high_order<boost::multiprecision::float128>();
        test_runge<boost::multiprecision::float128>();
#endif

}
