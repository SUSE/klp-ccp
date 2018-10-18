#include "builtins.hh"

using namespace suse::cp;

constexpr builtin_func::builtin_func() noexcept = default;
builtin_func::~builtin_func() noexcept = default;


builtin_func::factory builtin_func::lookup(const std::string &id)
{
  static std::map<const std::string,
		  const builtin_func::factory > m = {
  };

  auto it = m.find(id);
  if (it == m.end())
    return nullptr;

  return it->second;
}
