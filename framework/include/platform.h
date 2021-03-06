//
// Created by islam on 07.11.17.
//

#pragma once

#include "declarations.h"

#include <field_view.hpp>
#include <function_view.hpp>
#include <object_view.hpp>

#include <string>

namespace interop {
struct platform_field_t: field_view_t {
    using field_view_t::field_view_t;
};

struct platform_function_t: function_view_t {
    using function_view_t::function_view_t;
};

struct platform_object_t: public object_view_t {
  protected:
    std::string object_name;

  public:
    platform_object_t(std::string name)
      : object_name{std::move(name)}
    {}

    const std::string & name() const override { return object_name; }
};

class platform_t {
  public:
    virtual ~platform_t() = default;

    virtual std::vector<module_ptr_t> initialize(const platform_configuration_t &) = 0;
    virtual void run()                                                             = 0;
    virtual void dispose()                                                         = 0;
};
} // namespace interop
