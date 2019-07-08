#ifndef OUTPUT_REMARKS_HH
#define OUTPUT_REMARKS_HH

#include <vector>
#include <ios>
#include "output_remark.hh"

namespace klp
{
  namespace ccp
  {
    class output_remarks
    {
    public:
      void add(const output_remark &r);
      void add(output_remark &&r);
      void clear() noexcept;
      bool empty() const noexcept;
      bool any_fatal() const noexcept;

      output_remarks& operator+=(const output_remarks &remarks);

      friend std::ostream& operator<<(std::ostream &o,
				      const output_remarks &rs);

    private:
      std::vector<output_remark> _remarks;
    };

    std::ostream& operator<<(std::ostream &o, const output_remarks &rs);
  }
}

#endif
