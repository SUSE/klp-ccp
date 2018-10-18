#include "architecture.hh"
#include <stdexcept>

using namespace suse::cp;

mpa::limbs::size_type suse::cp::int_mode_to_width(const int_mode_kind m) noexcept
{
  switch (m) {
  case int_mode_kind::imk_none:
    assert(0);
    __builtin_unreachable();

  case int_mode_kind::imk_QI:
    return 8;

  case int_mode_kind::imk_HI:
    return 16;

  case int_mode_kind::imk_SI:
    return 32;

  case int_mode_kind::imk_DI:
    return 64;

  case int_mode_kind::imk_TI:
    return 128;
  }
}

int_mode_kind suse::cp::width_to_int_mode(const mpa::limbs::size_type w)
{
  if (w > 128) {
    throw std::overflow_error("no mode for integer width");
  } else if (w > 64) {
    return int_mode_kind::imk_TI;
  } else if (w > 32) {
    return int_mode_kind::imk_DI;
  } else if (w > 16) {
    return int_mode_kind::imk_SI;
  } else if (w > 8) {
    return int_mode_kind::imk_HI;
  } else {
    return int_mode_kind::imk_QI;
  }
}


architecture::~architecture() noexcept = default;

types::std_int_type::kind architecture::get_ptrdiff_kind() const noexcept
{
  return this->int_mode_to_std_int_kind(this->get_pointer_mode());
}
