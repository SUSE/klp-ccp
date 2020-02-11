/*
 * Copyright (C) 2019  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TARGET_GCC_HH
#define TARGET_GCC_HH

#include "target.hh"
#include "gcc_cmdline_parser.hh"
#include "preprocessor.hh"

namespace klp
{
  namespace ccp
  {
    class preprocessor;

    class target_gcc : public target
    {
    public:
      target_gcc(const char * const version);

      virtual ~target_gcc() noexcept = 0;

      virtual void parse_command_line
		(int argc, const char *argv[],
		 header_resolver &hr,
		 preprocessor &pp,
		 const std::function<void(const std::string&)> &report_warning)
	override;

      const gcc_cmdline_parser::gcc_version&
      get_gcc_version() const noexcept
      { return _gcc_version; }

    protected:
      struct opts_common
      {
	opts_common() noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated,
			const gcc_cmdline_parser::gcc_version &ver);

	std::string base_file;

	bool optimize;
	bool optimize_size;
      };

      struct opts_c_family
      {
	opts_c_family() noexcept;

	void handle_opt(const gcc_cmdline_parser::option * const o,
			const char *val, const bool negative,
			const bool generated,
			const gcc_cmdline_parser::gcc_version &ver);

	std::vector<std::string> pre_includes;
	std::vector<std::string> include_dirs;
	std::vector<std::string> include_dirs_quoted;
	std::vector<std::string> include_dirs_system;
	std::vector<std::string> include_dirs_after;

	struct macro_def_or_undef
	{
	  bool undef;
	  std::string arg;
	};
	std::vector<macro_def_or_undef> macro_defs_and_undefs;

	bool flag_undef;
      };

    private:
      virtual const gcc_cmdline_parser::option *
      _arch_get_opt_table() const noexcept = 0;

      virtual void
      _arch_handle_opt(const gcc_cmdline_parser::option * const o,
		       const char *val, const bool negative,
		       const bool generated) = 0;

      virtual void _arch_register_builtin_macros(preprocessor &pp) const = 0;

      static gcc_cmdline_parser::gcc_version
      _parse_version(const char * const version);

      void _decode_options
	(int argc, const char *argv[],
	 const std::function<void(const std::string&)> &report_warning);

      void
      _handle_opt(const gcc_cmdline_parser::option * const opt_table,
		  const gcc_cmdline_parser::option * const o,
		  const char *val, const bool negative,
		  const bool generated,
		  const gcc_cmdline_parser::option &opt_table_arch);

      void _register_builtin_macros(preprocessor &pp) const;

      const gcc_cmdline_parser::gcc_version _gcc_version;

      opts_common _opts_common;
      opts_c_family _opts_c_family;
    };
  }
}

#endif
