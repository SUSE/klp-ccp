#ifndef TYPES_HH
#define TYPES_HH

namespace suse
{
  namespace cp
  {
    namespace types
    {
      enum class qualifier
      {
	q_const,
	q_volatile,
	q_restrict,
	_q_end,
      };

      enum class struct_or_union_kind
      {
	souk_struct,
	souk_union,
      };
    }
  }
}

#endif
