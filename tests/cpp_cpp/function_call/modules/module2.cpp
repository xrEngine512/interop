//
// Created by islam on 13.05.17.
//

#include "module1.h"

#include <gtest/gtest.h>

using namespace std;

namespace interop {
void interop_register() {}

module_context_t * ctx;

void interop_link(module_context_t & context)
{
    printf("module 2 initialization\n");

    ctx = &context;
}
} // namespace interop

class interop_test: public ::testing::Test {};

typedef void (*hello_world_t)(int n, float k, double a);

const auto other_module = "module1";

TEST_F(interop_test, call)
{
    EXPECT_THROW(interop::ctx->get("not exists"), interop::module_lookup_error_t);
    auto && module = interop::ctx->get("module1");

    module.function("hello_world")->call(15, 4.5f, 7.0);
    module.function("hello_world")->call(8, 2.5f, 100.8);

    EXPECT_EQ(10, module.create("test", 10)->function("get")->call<int>());
    EXPECT_EQ(18, module.create("test", 18.78)->function("get")->call<int>());
    EXPECT_EQ(1.5, module.create("test", 0)->function("member2")->call<double>(1));
    EXPECT_EQ(24, module.function("add")->call<int>(10, 14));
    EXPECT_STREQ("say hello yourself!", module.function("say_hello")->call<const char *>());
    EXPECT_EQ(8086, module.function("non_capturing_lambda")->call<int>(6));
    EXPECT_EQ(560, module.function("capturing_lambda")->call<int>(8));
}

TEST_F(interop_test, call_dynamic)
{
    auto && module = interop::ctx->get("module1");

    auto obj = module.create("test", 11);

    EXPECT_EQ(11, obj->function("get")->call_dynamic());
    EXPECT_EQ(2.5, obj->function("member3")->call_dynamic({2.0f, 0.5}));
    EXPECT_EQ(2, module.function("add")->call_dynamic({1, 1}));
}

TEST_F(interop_test, dynamic_call_implicit_conversions)
{
    auto && module = interop::ctx->get("module1");

    auto obj = module.create("test", 11);

    EXPECT_EQ(11, obj->function("get")->call_dynamic());
    EXPECT_EQ(2.5, obj->function("member3")->call_dynamic({2.0, 0.5}));
    EXPECT_EQ(2.5, obj->function("member3")->call_dynamic({2, 0.5}));
    EXPECT_EQ(3.0, obj->function("member3")->call_dynamic({2, 1}));
}

TEST_F(interop_test, object_create_fail)
{
    auto & module = interop::ctx->get(other_module);

    EXPECT_THROW(module.create("not exists"), interop::type_lookup_error_t);
    EXPECT_THROW(module.create("test"), interop::function_lookup_error_t);
    EXPECT_THROW(module.create("test", 10, 11), interop::function_lookup_error_t);
}

class test_view {
    interop::object_ptr_t object;

  public:
    explicit test_view(interop::object_ptr_t obj)
      : object(std::move(obj))
      , member1{object->function("member1")}
      , member2{object->function("member2")}
      , member3{object->function("member3")}
      , get{object->function("get")}
    {}

#define _M(mem_name) interop::function_view_t::imitator_t<decltype(&test::mem_name)> mem_name

    _M(member1);
    _M(member2);
    _M(member3);
    _M(get);

#undef _M
};

TEST_F(interop_test, imitator)
{
    auto & module = interop::ctx->get(other_module);

    test_view test_obj(module.create("test", 10));

    EXPECT_EQ(10, test_obj.get());
    EXPECT_EQ(1.5, test_obj.member2(1));
    // one of operands is float, so expect lower than double precision overall
    EXPECT_FLOAT_EQ(1.0, test_obj.member3(0.7, 0.3));
}