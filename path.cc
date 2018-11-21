#include <algorithm>
#include <cerrno>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "path.hh"

std::string klp::ccp::normalize_path(std::string &&p)
{
  bool last_is_sep = false;

  for (auto it = p.begin(); it != p.end();) {
    if (*it == '/') {
      if (last_is_sep) {
	it = p.erase(it);
      } else {
	last_is_sep = true;
	++it;
      }
    } else if (last_is_sep && *it == '.') {
      if (p.end() - it >= 2 && *(it + 1) == '.' &&
	  (p.end() - it == 2 || (*(it + 2) == '/'))) {
	std::string::reverse_iterator rit{it};
	if (it != p.begin() + 1) {
	  ++rit;

	  // If the preceeding patch component is also a '..', then
	  // don't remove the current one.
	  if (p.rend() - rit == 2 && *rit == '.' && *(rit + 1) == '.') {
	    last_is_sep = false;
	    ++it;
	    continue;
	  }

	  rit = std::find(rit, p.rend(), '/');
	}

	if (p.end() - it == 2)
	  it = p.end();
	else
	  it += 3;

	it = p.erase(rit.base(), it);
      } else if (p.end() - it == 1 || *(it + 1) == '/') {
	it = p.erase(it, p.end() - it == 1 ? p.end() : it + 2);
      } else {
	++it;
	last_is_sep = false;
      }
    } else {
      last_is_sep = false;
      ++it;
    }
  }

  return std::move(p);
}

std::string klp::ccp::dirname(const std::string &p)
{
  std::string _p{p};
  _p = normalize_path(std::move(_p));

  auto end_pos = _p.rfind('/', _p.find_last_not_of('/'));
  if (!end_pos) {
    end_pos = 1;
  }

  return std::string(std::move(_p), 0, end_pos);
}

bool klp::ccp::file_readable(const std::string &p)
{
  if (access(p.c_str(), R_OK))
    return false;

  struct stat s;
  if (stat(p.c_str(), &s)) {
    throw std::system_error(errno, std::system_category(),
			    p);
  }

  return S_ISREG(s.st_mode);
}

std::string klp::ccp::getcwd()
{
  char *p = ::getcwd(nullptr, 0);

  if (!p)
    throw std::system_error(errno, std::system_category(), "getcwd");

  std::string sp(p);
  free(p);
  return sp;
}
