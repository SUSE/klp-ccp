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

#ifndef GCC_CMDLINE_PARSER_HH
#define GCC_CMDLINE_PARSER_HH

#include <utility>
#include <vector>
#include <functional>

namespace klp
{
  namespace ccp
  {
    class gcc_cmdline_parser
    {
    public:
      struct gcc_version
      {
	constexpr gcc_version() noexcept
	  : maj(0), min(0), patchlevel(0)
	{}

	constexpr gcc_version(unsigned int _maj, unsigned int _min,
			      unsigned int _patchlevel) noexcept
	  : maj(_maj), min(_min), patchlevel(_patchlevel)
	{}

	constexpr bool operator==(const gcc_version &v) const noexcept
	{
	  return (this->maj == v.maj &&
		  this->min == v.min &&
		  this->patchlevel == v.patchlevel);
	}

	constexpr bool operator<=(const gcc_version &v) const noexcept
	{
	  return (((!this->maj && !this->min && !this->patchlevel) ||
		   (!v.maj && !v.min && !v.patchlevel)) ? true :
		  (this->maj < v.maj ? true :
		   (this->maj > v.maj ? false :
		    (this->min < v.min ? true :
		     this->min > v.min ? false :
		     this->patchlevel <= v.patchlevel))));
	}

	constexpr bool operator<(const gcc_version &v) const noexcept
	{
	  return (this->maj < v.maj ? true :
		  (this->maj > v.maj ? false :
		   (this->min < v.min ? true :
		    this->min > v.min ? false :
		    this->patchlevel < v.patchlevel)));
	}

	unsigned int maj;
	unsigned int min;
	unsigned int patchlevel;
      };

      struct option
      {
	enum component
	{
	  comp_none	= 0,
	  comp_common	= 0x1,
	  comp_driver	= 0x2,
	  comp_target	= 0x4,
	  comp_c	= 0x8 ,
	  comp_cxx	= 0x10,
	  comp_objc	= 0x20,
	  comp_objcxx	= 0x40,
	};

	enum argument
	{
	  arg_none			= 0,
	  arg_joined			= 0x1,
	  arg_joined_or_missing	= 0x2,
	  arg_separate			= 0x4,
	};

	struct alias_type
	{
	  const char *name;
	  const char *pos_arg;
	  const char *neg_arg;
	};

	const char *name;
	int code;
	unsigned int comp;
	unsigned int arg;
	bool reject_negative;
	alias_type alias;
	const char *negative;

	gcc_version min_gcc_version;
	gcc_version max_gcc_version;
      };

      gcc_cmdline_parser(const gcc_version version) noexcept;

      void register_table(const option * const table);

      struct decoded_opt
      {
	decoded_opt(const option *_o, const option *_t, const char *_val,
		    const bool _negative) noexcept
	  : o(_o), table(_t), val(_val), negative(_negative)
	{}

	const option *o;
	const option *table;
	const char *val;
	bool negative;
      };

      typedef std::vector<decoded_opt> decoded_opts_type;

      decoded_opts_type operator()(const int argc, const char *argv[]) const;

      std::pair<const option*, const option*>
      find_option(const char *s) const noexcept;

    private:
      typedef std::pair<const option*, std::size_t> _table_type;

      const option* _find_option(const char *s, const _table_type &table)
	const noexcept;

      const gcc_version _version;

      std::vector<_table_type> _tables;
    };
  }
}

#endif
