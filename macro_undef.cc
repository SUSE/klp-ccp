#include "macro_undef.hh"

using namespace klp::ccp;

macro_undef::macro_undef(std::shared_ptr<const macro> &&macro,
			 const file_range &file_range)
  : _macro(macro), _file_range(file_range)
{}

