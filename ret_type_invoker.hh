#ifndef RET_TYPE_INVOKER
#define RET_TYPE_INVOKER

#include <type_traits>

namespace klp
{
  namespace ccp
  {
    // Detangle return value returning from function invocation.
    template <typename ret_type>
    class ret_type_invoker
    {
    public:
      ~ret_type_invoker() noexcept
      {
	reinterpret_cast<ret_type*>(&_result)->~ret_type();
      }

      template <typename callable_type, typename... args_types>
      void operator()(callable_type &&c, args_types&&... args)
      {
	new (&_result)ret_type(c(std::forward<args_types>(args)...));
      }

      ret_type&& grab_result()
      {
	return std::move(*reinterpret_cast<ret_type*>(&_result));
      }

    private:
      typename std::aligned_storage<sizeof(ret_type)>::type _result;
    };

    template <>
    class ret_type_invoker<void>
    {
    public:
      template <typename callable_type, typename... args_types>
      void operator()(callable_type &&c, args_types&&... args)
      {
	c(std::forward<args_types>(args)...);
      }

      void grab_result()
      { return; }
    };
  }
}

#endif // RET_TYPE_INVOKER
