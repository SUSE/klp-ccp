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

#include <cassert>
#include <cerrno>
#include <system_error>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "user_policy_command.hh"

using namespace klp::ccp;

user_policy_command::user_policy_command(const std::string &cmd)
{
  char in_quotation = '\0';
  bool last_was_escape = false;

  std::string cur_arg;
  for (auto it = cmd.begin(); it != cmd.end(); ++it) {
    switch (*it) {
    case '\'':
      if (in_quotation == *it) {
	in_quotation = '\0';
      } else if (!last_was_escape && !in_quotation) {
	in_quotation = *it;
      } else {
	if (last_was_escape && in_quotation) {
	  assert(in_quotation == '\"');
	  cur_arg.append(1, '\\');
	}
	cur_arg.append(1, *it);
	last_was_escape = false;
      }
      break;

    case '\"':
      if (!last_was_escape && in_quotation == *it)
	in_quotation = '\0';
      else if (!last_was_escape && !in_quotation)
	in_quotation = *it;
      else
	cur_arg.append(1, *it);
      last_was_escape = false;
      break;

    case '\\':
      if (!last_was_escape && in_quotation != '\'') {
	last_was_escape = true;
      } else {
	cur_arg.append(1, *it);
	last_was_escape = false;
      }
      break;

    case ' ':
      /* fall through */
    case '\t':
      if (in_quotation) {
	if (last_was_escape)
	  cur_arg.append(1, '\\');
	cur_arg.append(1, *it);
      } else if (last_was_escape) {
	cur_arg.append(1, *it);
      } else if (!cur_arg.empty()) {
	_args.emplace_back(std::move(cur_arg));
      }
      last_was_escape = false;
      break;

    case '\n':
      if (!last_was_escape) {
	if (in_quotation) {
	  cur_arg.append(1, *it);
	} else if (!cur_arg.empty()) {
	  _args.emplace_back(std::move(cur_arg));
	}
      }
      last_was_escape = false;
      break;

    default:
      if (in_quotation && last_was_escape)
	cur_arg.append(1, '\\');
      cur_arg.append(1, *it);
      last_was_escape = false;
      break;
    }
  }

  if (!cur_arg.empty())
    _args.emplace_back(std::move(cur_arg));

  if (in_quotation)
    throw cmd_parse_except("unterminated quotation in command");

  if (last_was_escape)
    throw cmd_parse_except("escape character at end of command");

  if (_args.empty())
    throw cmd_parse_except("no command given");
}

user_policy_command::instance
user_policy_command::execute(const std::vector<std::string> &extra_args,
			     char *envp[], std::regex &&re_result) const
{
  std::vector<std::string> argv;
  argv.reserve(_args.size() + extra_args.size());
  for (const auto &arg : _args)
    argv.push_back(arg);
  for (const auto &arg : extra_args)
    argv.push_back(arg);

  std::vector<char *> c_argv;
  c_argv.reserve(argv.size() + 1);
  for (const auto &arg : argv)
    c_argv.push_back(const_cast<char*>(arg.c_str()));
  c_argv.push_back(nullptr);

  int stdout_pipe[2];
  if (pipe(stdout_pipe)) {
    throw std::system_error{
	    errno, std::system_category(),
	    "failed to created pipe for \"" + _args[0] + "\""
	  };
  }

  int stderr_pipe[2];
  if (pipe(stderr_pipe)) {
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    throw std::system_error{
	    errno, std::system_category(),
	    "failed to create pipe for \"" + _args[0] + "\""
	  };
  }

  const pid_t pid = fork();
  if (pid == -1) {
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);

    throw std::system_error{
	    errno, std::system_category(),
	    "failed to fork for \"" + _args[0] + "\""
	  };

  } else if (!pid) {
    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    if (dup2(stderr_pipe[1], 2) < 0) {
      close(stdout_pipe[1]);
      close(stderr_pipe[1]);
      std::cerr
	<< (std::system_error{
	     errno, std::system_category(),
	     "error: failed to redirect stdout for \"" + _args[0] + "\""
	    }).what()
	<< std::endl;
      std::abort();
    }
    close(stderr_pipe[1]);

    if (dup2(stdout_pipe[1], 1) < 0) {
      close(stdout_pipe[1]);

      std::cerr
	<< (std::system_error{
	     errno, std::system_category(),
	     "failed to redirect stderr for \"" + _args[0] + "\""
	    }).what()
	<< std::endl;
      std::abort();
    }
    close(stdout_pipe[1]);

    execve(c_argv[0], &c_argv[0], envp);
    std::cerr
      << (std::system_error{
	    errno, std::system_category(),
	    "failed to execute \"" + _args[0] + "\""
	  }).what()
      << std::endl;
    std::abort();

  } else {
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    return instance{pid, stdout_pipe[0], stderr_pipe[0], std::move(argv),
		    std::move(re_result)};
  }
}


user_policy_command::cmd_parse_except::cmd_parse_except(const char * const what)
  : _what(what)
{}

user_policy_command::cmd_parse_except::~cmd_parse_except() noexcept = default;

const char* user_policy_command::cmd_parse_except::what() const noexcept
{
  return _what;
}


user_policy_command::execution_except::execution_except(std::string &&what)
  : _what(std::move(what))
{}

user_policy_command::execution_except::~execution_except() noexcept = default;

const char* user_policy_command::execution_except::what() const noexcept
{
  return _what.c_str();
}


user_policy_command::instance::instance(const pid_t pid,
					const int stdout_fd,
					const int stderr_fd,
					std::vector<std::string> &&argv,
					std::regex &&re_result) noexcept
  : _pid(pid), _stdout_fd(stdout_fd), _stderr_fd(stderr_fd),
    _argv(std::move(argv)), _re_result(std::move(re_result))
{}

user_policy_command::instance::instance(instance &&i) noexcept
  : _pid(i._pid), _stdout_fd(i._stdout_fd), _stderr_fd(i._stderr_fd),
    _argv(std::move(i._argv)), _re_result(std::move(i._re_result)),
    _stdout_buf(std::move(i._stdout_buf)), _result(std::move(i._result)),
    _matched_result(std::move(i._matched_result)),
    _warnings(std::move(i._warnings)), _errors(std::move(i._errors)),
    _stderr_output(std::move(i._stderr_output))
{
  i._pid = -1;
  i._stdout_fd = -1;
  i._stderr_fd = -1;
}

user_policy_command::instance::~instance() noexcept
{
  if (_pid != -1) {
    kill(_pid, SIGKILL);
    waitpid(_pid, nullptr, 0);
  }

  if (_stdout_fd != -1)
    close(_stdout_fd);

  if (_stderr_fd != -1)
    close(_stderr_fd);
}

void user_policy_command::instance::wait()
{
  assert(_pid != -1);

  while (_stdout_fd != -1 || _stdout_fd != -1) {
    int nfds = 0;
    fd_set readfds;
    FD_ZERO(&readfds);

    if (_stdout_fd != -1) {
      if (_stdout_fd > nfds)
	nfds = _stdout_fd;
      FD_SET(_stdout_fd, &readfds);
    }
    if (_stderr_fd != -1) {
      if (_stderr_fd > nfds)
	nfds = _stderr_fd;
      FD_SET(_stderr_fd, &readfds);
    }
    ++nfds;

    if (select(nfds, &readfds, nullptr, nullptr, nullptr) < 0) {
      throw std::system_error{
	      errno, std::system_category(),
	      "failed to wait for output from \"" + _argv[0] + "\""
	    };
    }

    if (FD_ISSET(_stdout_fd, &readfds)) {
      _buffer_type buf;
      buf.resize(4096);
      ssize_t r = read(_stdout_fd, &buf[0], buf.size());
      if (r > 0) {
	buf.resize(r);
	buf.shrink_to_fit();

      } else if (!r) {
	close(_stdout_fd);
	_stdout_fd = -1;
	buf.clear();
	buf.shrink_to_fit();

      } else {
	throw std::system_error{
		errno, std::system_category(),
		"failed to read output from \"" + _argv[0] + "\""
	      };
      }

      _process_stdout(std::move(buf));
    }

    if (FD_ISSET(_stderr_fd, &readfds)) {
      _buffer_type buf;
      buf.resize(4096);
      ssize_t r = read(_stderr_fd, &buf[0], buf.size());
      if (r > 0) {
	buf.resize(r);
	buf.shrink_to_fit();

      } else if (!r) {
	close(_stderr_fd);
	_stderr_fd = -1;
	buf.clear();
	buf.shrink_to_fit();

      } else {
	throw std::system_error{
		errno, std::system_category(),
		"failed to read output from \"" + _argv[0] + "\""
	      };
      }

      _process_stderr(std::move(buf));
    }
  }

  int status;
  if (waitpid(_pid, &status, 0) == -1) {
    throw std::system_error{
	    errno, std::system_category(),
	    "failed to wait for \"" + _argv[0] + "\""
	  };
  }
  _pid = -1;

  if (WIFSIGNALED(status)) {
    const int signr = WTERMSIG(status);

    std::string msg =
      "command '" + _format_cmd() + "' caught signal " + std::to_string(signr);

    if (!_stderr_output.empty())
      msg += "; standard error was: " + _quote_str(_stderr_output);

    throw execution_except{std::move(msg)};

  } else if (WIFEXITED(status)) {
    const int exit_status = WEXITSTATUS(status);

    if (exit_status) {
      std::string msg =
	("command '" + _format_cmd() + "' failed with status " +
	 std::to_string(exit_status));

      if (!_stderr_output.empty())
	msg += "; standard error was: " + _quote_str(_stderr_output);

      throw execution_except{std::move(msg)};
    }

  } else {
    assert(0);
    __builtin_unreachable();
  }

  if (!_stderr_output.empty()) {
    std::string msg =
      ("command '" + _format_cmd() +
       "' returned with zero status, but standard error was non-empty: " +
       _quote_str(_stderr_output));

    throw execution_except{std::move(msg)};
  }

  if (!_errors.empty()) {
    _matched_result = std::smatch{};
    _result.clear();

  } else if (_matched_result.empty()) {
    std::string msg =
      ("command '" + _format_cmd() + "' didn't return any result");

    throw execution_except{std::move(msg)};
  }
}

const std::string& user_policy_command::instance::get_plain_result()
  const noexcept
{
  assert(_pid == -1);
  assert(!_matched_result.empty());
  return _result;
}

const std::smatch& user_policy_command::instance::get_matched_result()
  const noexcept
{
  assert(_pid == -1);
  assert(!_matched_result.empty());
  return _matched_result;
}

const std::vector<std::string>& user_policy_command::instance::get_warnings()
  const noexcept
{
  assert(_pid == -1);
  return _warnings;
}

const std::vector<std::string>& user_policy_command::instance::get_errors()
  const noexcept
{
  assert(_pid == -1);
  return _errors;
}

void user_policy_command::instance::_process_stdout(_buffer_type &&buf)
{
  const bool eof = buf.empty();
  _stdout_buf.insert(_stdout_buf.end(), buf.cbegin(), buf.cend());

  _buffer_type::iterator line_begin = _stdout_buf.begin();
  while (true) {
    _buffer_type::iterator line_end =
      std::find(line_begin, _stdout_buf.end(), '\n');
    if (line_end == _stdout_buf.end())
      break;

    _process_stdout_line(line_begin, line_end);
    line_begin = line_end + 1;
  }

  if (!eof) {
    _stdout_buf.erase(_stdout_buf.begin(), line_begin);

  } else {
    _process_stdout_line(line_begin, _stdout_buf.end());
    _stdout_buf.clear();
    _stdout_buf.shrink_to_fit();
  }
}

void user_policy_command::instance::
_process_stdout_line(_buffer_type::const_iterator line_begin,
		     _buffer_type::const_iterator line_end)
{
  const auto& f = std::use_facet<std::ctype<char>>(std::locale());
  auto skip_leading_ws =
    [&]() {
      while (line_begin != line_end &&
	     f.is(std::ctype_base::blank, *line_begin)) {
	++line_begin;
      };
    };
  skip_leading_ws();

  while (line_end != line_begin &&
	 f.is(std::ctype_base::blank, *(line_end - 1))) {
    --line_end;;
  }

  if (line_begin == line_end)
    return;

  static const char sRESULT[] = "RESULT:";
  static const char sWARNING[] = "WARNING:";
  static const char sERROR[] = "ERROR:";

  if (line_end - line_begin >= sizeof(sRESULT) - 1 &&
      std::equal(sRESULT, sRESULT + sizeof(sRESULT) - 1, line_begin)) {

    if (!_matched_result.empty()) {
      throw execution_except {
	      "command '" + _format_cmd() +
	      "' returned multiple result specifications"
	    };
    }

    line_begin += sizeof(sRESULT) - 1;
    skip_leading_ws();

    _result.assign(line_begin, line_end);
    if (!std::regex_match(_result, _matched_result, _re_result)) {
      throw execution_except {
	      "unrecognized result specification returned by command '"
	      + _format_cmd() + "': " + _quote_str(_result)
	    };
      _result.clear();
    }

  } else if (line_end - line_begin >= sizeof(sWARNING) - 1 &&
	     std::equal(sWARNING, sWARNING + sizeof(sWARNING) - 1,
			line_begin)) {
    line_begin += sizeof(sWARNING) - 1;
    skip_leading_ws();

    std::string s;
    s.assign(line_begin, line_end);
    _warnings.push_back(std::move(s));

  } else if (line_end - line_begin >= sizeof(sERROR) - 1 &&
	     std::equal(sERROR, sERROR + sizeof(sERROR) - 1,
			line_begin)) {
    line_begin += sizeof(sERROR) - 1;
    skip_leading_ws();

    std::string s;
    s.assign(line_begin, line_end);
    _errors.push_back(std::move(s));

  } else {
    std::string s;
    s.assign(line_begin, line_end);
    throw execution_except {
	    "unrecognized output from command '" + _format_cmd()
	      + "': " + _quote_str(s)
	  };
  }
}

void user_policy_command::instance::_process_stderr(_buffer_type &&buf)
{
  _stderr_output.append(buf.begin(), buf.end());

  if (buf.empty()) {
    const auto& f = std::use_facet<std::ctype<char>>(std::locale());
    while (!_stderr_output.empty() &&
	   f.is(std::ctype_base::blank, _stderr_output.back())) {
      _stderr_output.pop_back();
    }
  }
}

std::string user_policy_command::instance::_format_cmd() const
{
  std::string cmd;
  bool first = true;

  for (const auto &arg : _argv) {
    if (!first)
      cmd += ' ';
    else
      first = false;

    // Quote (and escape) the argument, if necessary.
    if (arg.find_first_of("\\\" \t\n") != std::string::npos) {
      cmd += _quote_str(arg);
    } else {
      cmd += arg;
    }
  }

  return cmd;
}

std::string user_policy_command::instance::_quote_str(const std::string &s)
{
  std::string quoted = '"' + s;
  for (std::string::size_type pos = quoted.find('\\', 1);
       pos != std::string::npos;
       pos = quoted.find('\\', pos + 2)) {
    quoted.insert(pos, 1, '\\');
  }
  for (std::string::size_type pos = quoted.find('"', 1);
       pos != std::string::npos;
       pos = quoted.find('"', pos + 2)) {
    quoted.insert(pos, 1, '\\');
  }
  quoted += '"';

  return quoted;
}
