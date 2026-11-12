namespace klp
{
  namespace ccp
  {
    template<typename callable_type>
    auto
    pp_result::macro_invocation::expansion_forest::rescan_macro_invocation::
    output::fragment::
    operator()(callable_type &&c) const ->
      decltype(c(std::declval<repl_tag>()))
    {
      switch (_op) {
      case op::repl:
	return c(repl_tag{});
      case op::arg:
	return c(arg_tag{}, _arg);
      case op::stringification:
	return c(stringification_tag{}, _stringification);
      case op::concat:
	return c(concat_tag{}, _concat_sequence);
      case op::conditional_comma:
	return c(conditional_comma_tag{}, _out_range.length() == 0);
      case op::empty:
	return c(empty_tag{});
      case op::builtin_special:
	return c(builtin_special_tag{});
      }
    }

    template<typename callable_type>
    auto
    pp_result::macro_invocation::expansion_forest::rescan_macro_invocation::
    output::fragment::concat_sequence::component::
    operator()(callable_type &&c) const ->
      decltype(c(std::declval<repl_tag>()))
    {
      switch(_op) {
      case op::repl:
	return c(repl_tag{});
      case op::arg:
	return c(arg_tag{}, _arg);
      case op::stringification:
	return c(stringification_tag{}, _stringification);
      case op::conditional_comma:
	return c(conditional_comma_tag{}, _conditional_comma_skipped);
      }
    }
  }
}
