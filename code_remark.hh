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

#ifndef CODE_REMARK_HH
#define CODE_REMARK_HH

#include <string>
#include <ios>
#include <memory>
#include <functional>
#include "pp_tokens_range.hh"
#include "pp_result.hh"
#include "range_in_file.hh"

namespace klp
{
  namespace ccp
  {
    class pp_result;

    class code_remark
    {
    public:
      enum class severity
	{
	  fatal,
	  warning,
	};

      code_remark(const severity sev, const std::string &msg,
		  const pp_result &pp_result, const pp_tokens_range &range);

      code_remark(const severity sev, const std::string &msg,
		  const pp_result &pp_result,
		  const pp_token_index first,
		  const pp_token_index last);

      code_remark(const severity sev, const std::string &msg,
		  const pp_result &pp_result,
		  const pp_token_index tok_index);

      code_remark(const severity sev, const std::string &msg,
		  const pp_result &pp_result,
		  const raw_pp_tokens_range &raw_range);

      code_remark(const severity sev, const std::string &msg,
		  const pp_result::header_inclusion_node &file,
		  const range_in_file &range_in_file);

      severity get_severity() const noexcept
      {
	return _sev;
      }

      friend std::ostream& operator<<(std::ostream &o,
				      const code_remark &remark);

    private:
      const pp_result::header_inclusion_node *_first_file;
      const pp_result::header_inclusion_node *_last_file;
      range_in_file::loc_type _begin_loc;
      range_in_file::loc_type _end_loc;
      std::string _msg;
      severity _sev;
    };

    std::ostream& operator<<(std::ostream &o, const code_remark &remark);
  }
}

#endif
