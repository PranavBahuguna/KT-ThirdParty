/* Unit testing for outcomes
(C) 2013-2019 Niall Douglas <http://www.nedproductions.biz/> (9 commits)


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <boost/outcome/outcome.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>

BOOST_OUTCOME_AUTO_TEST_CASE(issues_0010_test, "Expected's operator->(), operator*() and .error() throw exceptions when they should not")
{
  using namespace BOOST_OUTCOME_V2_NAMESPACE;
  const char *a = "hi", *b = "bye";
  struct udt1  // NOLINT
  {
    const char *_v{nullptr};
    udt1() = default;
    constexpr explicit udt1(const char *v) noexcept : _v(v) {}
    constexpr udt1(udt1 &&o) noexcept : _v(o._v) { o._v = nullptr; }
    udt1(const udt1 &) = default;
    constexpr udt1 &operator=(udt1 &&o) noexcept
    {
      _v = o._v;
      o._v = nullptr;
      return *this;
    }
    udt1 &operator=(const udt1 &) = delete;
    constexpr const char *operator*() const noexcept { return _v; }
  };
  struct udt2  // NOLINT
  {
    const char *_v{nullptr};
    udt2() = default;
    constexpr explicit udt2(const char *v) noexcept : _v(v) {}
    constexpr udt2(udt2 &&o) noexcept : _v(o._v) { o._v = nullptr; }
    udt2(const udt2 &) = default;
    constexpr udt2 &operator=(udt2 &&o) noexcept
    {
      _v = o._v;
      o._v = nullptr;
      return *this;
    }
    udt1 &operator=(const udt1 &) = delete;
    constexpr const char *operator*() const noexcept { return _v; }
  };
  result<udt1, udt2> p(udt1{a});
  result<udt1, udt2> n(udt2{b});
  // State check
  BOOST_CHECK(p.has_value());
  BOOST_CHECK(!n.has_value());
  // These should behave as expected (!)
  // BOOST_CHECK_NO_THROW(p.value());
  // BOOST_CHECK_NO_THROW(n.value());
  // And state is not destroyed
  BOOST_CHECK(p.has_value() && *p.assume_value() == a);
  BOOST_CHECK(!n.has_value() && *n.assume_error() == b);
  // LEWG Expected provides rvalue ref semantics for operator*(), error() and error_or()
  udt1 a1(std::move(p.assume_value()));
  BOOST_CHECK(*a1 == a);
  BOOST_CHECK(*p.assume_value() == nullptr);
  udt2 e2(std::move(n).assume_error());
  BOOST_CHECK(*e2 == b);
  BOOST_CHECK(*n.assume_error() == nullptr);  // NOLINT
}
