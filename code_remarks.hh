#ifndef CODE_REMARKS_HH
#define CODE_REMARKS_HH

#include <vector>
#include <ios>

#include "code_remark_raw.hh"
#include "code_remark.hh"

namespace klp
{
  namespace ccp
  {
    class code_remarks
    {
    public:
      void add(const code_remark &r);
      void add(code_remark &&r);
      void add(const code_remark_raw &r);
      void add(code_remark_raw &&r);
      void clear() noexcept;
      bool empty() const noexcept;
      bool any_fatal() const noexcept;

      code_remarks& operator+=(const code_remarks &remarks);

      friend std::ostream& operator<<(std::ostream &o, const code_remarks &rs);

    private:
      std::vector<code_remark_raw> _remarks_raw;
      std::vector<code_remark> _remarks;
    };

    std::ostream& operator<<(std::ostream &o, const code_remarks &rs);
  }
}

#endif
