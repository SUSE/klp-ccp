#include "lp_creation_policy.hh"

using namespace klp::ccp;

lp_creation_policy::~lp_creation_policy() noexcept = default;


lp_creation_policy::symbol_modification::symbol_modification()
  : new_linkage{linkage_change::lc_none}
{}

lp_creation_policy::symbol_modification::
symbol_modification(std::string &&_new_name, const linkage_change _new_linkage)
  : new_name(std::move(_new_name)), new_linkage(_new_linkage)
{}

lp_creation_policy::externalized_symbol_modification::
externalized_symbol_modification()
  : make_pointer(false)
{}

lp_creation_policy::externalized_symbol_modification::
externalized_symbol_modification
	(std::string &&_new_name,
	 const symbol_modification::linkage_change _new_linkage,
	 const bool _make_pointer)
  : sym_mod(std::move(_new_name), _new_linkage), make_pointer(_make_pointer)
{}
