#ifndef SOURCE_READER_HH
#define SOURCE_READER_HH

#include <vector>
#include <string>

namespace klp
{
  namespace ccp
  {
    class source_reader
    {
    public:
      typedef std::vector<char> buffer_type;

      virtual ~source_reader() noexcept;

      virtual buffer_type read() = 0;
    };

    class file_source_reader final : public source_reader
    {
    public:
      file_source_reader(const std::string &filename);

      virtual ~file_source_reader() noexcept override;

      virtual buffer_type read() override;

    private:
      void _fill_buffer();

      const std::string _filename;
      int _fd;
      buffer_type _buf;
      std::streamoff _buf_pos;
    };

    class buf_source_reader final : public source_reader
    {
    public:
      buf_source_reader(const std::string &buf);

      virtual ~buf_source_reader() noexcept override;

      virtual buffer_type read() override;

    private:
      buffer_type _buf;
    };
  }
}

#endif
