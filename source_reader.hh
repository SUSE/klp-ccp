#ifndef SOURCE_READER_HH
#define SOURCE_READER_HH

#include <vector>
#include <string>

namespace klp
{
  namespace ccp
  {
    class range_in_file;

    class source_reader
    {
    public:
      typedef std::vector<char> buffer_type;

      virtual ~source_reader() noexcept;

      virtual buffer_type read() = 0;

      virtual void read(buffer_type &to_buffer,
			const range_in_file &from_range) = 0;
    };

    class file_source_reader final : public source_reader
    {
    public:
      file_source_reader(const std::string &filename);

      virtual ~file_source_reader() noexcept override;

      virtual buffer_type read() override;

      virtual void read(buffer_type &to_buffer,
			const range_in_file &from_range) override;

    private:
      void _fill_buffer();

      const std::string _filename;
      int _fd;
      buffer_type _buf;
      std::streamoff _buf_pos;
    };
  }
}

#endif
