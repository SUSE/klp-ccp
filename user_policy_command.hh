#ifndef _USER_POLICY_COMMAND_HH
#define _USER_POLICY_COMMAND_HH

#include <vector>
#include <string>
#include <stdexcept>
#include <tuple>
#include <regex>
#include <sys/types.h>

namespace klp
{
  namespace ccp
  {
    class user_policy_command
    {
    public:
      class cmd_parse_except : public std::exception
      {
      public:
	virtual ~cmd_parse_except() noexcept;

	virtual const char* what() const noexcept override;

      private:
	friend class user_policy_command;

	explicit cmd_parse_except(const char *const what);

	const char *_what;
      };

      class execution_except : public std::exception
      {
      public:
	virtual ~execution_except() noexcept;

	virtual const char* what() const noexcept override;

      private:
	friend class user_policy_command;

	explicit execution_except(std::string &&what);

	const std::string _what;
      };

      class instance
      {
      public:
	instance(instance &&i) noexcept;

	~instance() noexcept;

	void wait();

	const std::string& get_plain_result() const noexcept;
	const std::smatch& get_matched_result() const noexcept;
	const std::vector<std::string>& get_warnings() const noexcept;
	const std::vector<std::string>& get_errors() const noexcept;

      private:
	friend class user_policy_command;

	typedef std::vector<char> _buffer_type;

	instance(const pid_t pid, const int stdout_fd, const int stderr_fd,
		 std::vector<std::string> &&argv,
		 std::regex &&re_result) noexcept;

	void _process_stdout(_buffer_type &&buf);
	void _process_stdout_line(_buffer_type::const_iterator line_begin,
				  _buffer_type::const_iterator line_end);
	void _process_stderr(_buffer_type &&buf);

	std::string _format_cmd() const;
	static std::string _quote_str(const std::string &s);

	pid_t _pid;
	int _stdout_fd;
	int _stderr_fd;
	std::vector<std::string> _argv;
	std::regex _re_result;

	_buffer_type _stdout_buf;

	std::string _result;
	std::smatch _matched_result;

	std::vector<std::string> _warnings;
	std::vector<std::string> _errors;
	std::string _stderr_output;
      };

      user_policy_command(const std::string &cmd);

      instance execute(const std::vector<std::string> &extra_args,
		       char *envp[], std::regex &&re_result) const;

    private:
      std::vector<std::string> _args;
    };
  }
}

#endif // _USER_POLICY_COMMAND_HH
