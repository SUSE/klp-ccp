/*
 * Copyright (C) 2024  SUSE Software Solutions Germany GmbH
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

// Must come first.
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <osdefs.h>

#include "lp_creation_python_policy.hh"
#include "ast.hh"
#include "pp_result.hh"
#include "code_remark.hh"
#include "code_remarks.hh"
#include "raw_pp_token.hh"
#include "lp_except.hh"
#include <string>
#include <new>
#include <functional>

using namespace klp::ccp;

python_except::python_except(const char *what)
  : std::runtime_error{what}
{}

python_except::~python_except() noexcept = default;


void klp::ccp::handle_python_except()
{
  if (PyErr_Occurred())
    PyErr_Print();
}


namespace
{
  struct allocated_ids
  {
    using allocated_ids_type = lp_creation_policy::allocated_ids_type;

    static PyObject* create(const allocated_ids_type &ids);

    static void py_dealloc(PyObject *self);

    static PyObject* contains(PyObject *self, PyObject *args,
			      PyObject *kwargs);

    static PyMethodDef py_methods[];
    static PyTypeObject py_type;

    PyObject_HEAD;
    const allocated_ids_type *_wrapped_ids;
  };
}

extern "C" {
  static void allocated_ids_py_dealloc(PyObject *self);
  static PyObject* allocated_ids_py_contains(PyObject *self, PyObject *args,
					     PyObject *kwargs);
}

PyObject* allocated_ids::create(const allocated_ids_type &ids)
{
  PyObject *o = PyType_GenericAlloc(&allocated_ids::py_type, 0);
  if (!o)
    return nullptr;

  reinterpret_cast<allocated_ids*>(o)->_wrapped_ids = &ids;

  return o;
}

void allocated_ids_py_dealloc(PyObject *self)
{
  Py_TYPE(self)->tp_free(self);
}

PyObject*
allocated_ids_py_contains(PyObject *self, PyObject *args, PyObject *kwargs)
{
  const allocated_ids *me = reinterpret_cast<const allocated_ids *>(self);
  if (!me->_wrapped_ids) {
    PyErr_SetString(PyExc_ReferenceError,
		    "Accessed AllocatedIds instance out of scope.");
    return nullptr;
  }

  const char *query_id = nullptr;
  // Beginning with Python 3.13 this can be constified.
  char kw_id[] = "id";
  char * kwlist[] = { kw_id, nullptr };
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", &kwlist[0],
				   &query_id)) {
    return nullptr;
  }

  if (me->_wrapped_ids->find(query_id) != me->_wrapped_ids->end())
    Py_RETURN_TRUE;

  Py_RETURN_FALSE;
}

PyMethodDef allocated_ids::py_methods[] {
  {
    "contains",
    reinterpret_cast<PyCFunction>(allocated_ids_py_contains),
    METH_VARARGS | METH_KEYWORDS,
    "Check whether a given identifier has been used somewhere already."
  },
  { nullptr }
};

// Note: some members are initialized outside the initializer list
// from pyinit_ccp_interface_mod().
PyTypeObject allocated_ids::py_type = {
  .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "_ccp.AllocatedIds",
  .tp_basicsize = sizeof(allocated_ids),
};

static PyModuleDef py_ccp_interface_mod_def = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "_ccp",
  .m_doc = "Provide interfacing functionality between ccp and Python.",
  .m_size = -1,
};

static PyObject *pyinit_ccp_interface_mod(void)
{
  // Install the ccp <-> Python interface extension.
  // g++ 7.5 does not support initialization of these members with a
  // designated initializer list yet and would bail out with.  "sorry,
  // unimplemented: non-trivial designated initializers not
  // supported".  For the moment being, do it from here then :/
  allocated_ids::py_type.tp_dealloc =
    static_cast<destructor>(allocated_ids_py_dealloc);
  allocated_ids::py_type.tp_flags =
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_DISALLOW_INSTANTIATION;
  allocated_ids::py_type.tp_doc =
    PyDoc_STR("Registry of id tokens used somewhere.");
  allocated_ids::py_type.tp_methods = allocated_ids::py_methods;

  if (PyType_Ready(&allocated_ids::py_type))
    return nullptr;

  PyObject *ccp_interface_mod = PyModule_Create(&py_ccp_interface_mod_def);
  if (!ccp_interface_mod)
    return nullptr;

  if ((PyModule_AddObjectRef
       (ccp_interface_mod,
	"AllocatedIds",
	reinterpret_cast<PyObject *>(&allocated_ids::py_type))) < 0) {
    Py_DECREF(ccp_interface_mod);
    return nullptr;
  }

  return ccp_interface_mod;
}


struct lp_creation_python_policy::_data
{
  _data() noexcept;
  ~_data() noexcept;

  using instantiate_remark_type =
    std::function<code_remark(const code_remark::severity,
			      std::string&&)>;

  bool is_py_lp_creation_policy_warning(PyObject *o) const noexcept;

  bool is_py_lp_creation_policy_error(PyObject *o) const noexcept;

  code_remark convert_py_lp_creation_policy_warning
    (PyObject *o,
     const instantiate_remark_type &instantiate) const;

  void convert_py_lp_creation_policy_warnings
    (PyObject *o,
     const instantiate_remark_type &instantiate,
     code_remarks &remarks) const;

  code_remark convert_py_lp_creation_policy_error
    (PyObject *o,
     const instantiate_remark_type &instantiate,
     code_remarks &remarks) const;

  PyObject* unwrap_py_lp_creation_policy_call_result
    (PyObject *r,
     const instantiate_remark_type &instantiate_remark,
     code_remarks &remarks) const;

  PyObject* build_py_loc(const pp_token_index id_tok,
			 const pp_result &pp_result) const;

  PyObject*
  get_py_linkage_enum_member(const ast::linkage::linkage_kind &linkage)
    const noexcept;

  PyObject *wrap_allocated_ids(const allocated_ids::allocated_ids_type &ids)
    const;

  lp_creation_policy::symbol_modification
  convert_symbol_modification(PyObject *o);

  lp_creation_policy::externalized_symbol_modification
  convert_externalized_symbol_modification(PyObject *o);

  PyObject *linkage_enum_member_none;
  PyObject *linkage_enum_member_internal;
  PyObject *linkage_enum_member_external;
  PyObject *linkage_enum_member_nested_fun_auto;
  PyObject *symbol_modification_cls;
  long linkage_change_enum_member_none_val = -1;
  long linkage_change_enum_member_make_static_val = -1;
  long linkage_change_enum_member_make_extern_val = -1;
  PyObject *externalized_symbol_modification_cls;
  PyObject *lp_creation_policy_warning_cls;
  PyObject *lp_creation_policy_error_cls;

  PyObject *pol_obj;
};

lp_creation_python_policy::_data::_data() noexcept
  : linkage_enum_member_none{nullptr},
    linkage_enum_member_internal{nullptr},
    linkage_enum_member_external{nullptr},
    linkage_enum_member_nested_fun_auto{nullptr},
    symbol_modification_cls{nullptr},
    linkage_change_enum_member_none_val{-1},
    linkage_change_enum_member_make_static_val{-1},
    linkage_change_enum_member_make_extern_val{-1},
    externalized_symbol_modification_cls{nullptr},
    lp_creation_policy_warning_cls{nullptr},
    lp_creation_policy_error_cls{nullptr},
    pol_obj{nullptr}
{}

lp_creation_python_policy::_data::~_data() noexcept
{
  Py_XDECREF(linkage_enum_member_none);
  Py_XDECREF(linkage_enum_member_internal);
  Py_XDECREF(linkage_enum_member_external);
  Py_XDECREF(linkage_enum_member_nested_fun_auto);
  Py_XDECREF(symbol_modification_cls);
  Py_XDECREF(externalized_symbol_modification_cls);
  Py_XDECREF(lp_creation_policy_warning_cls);
  Py_XDECREF(lp_creation_policy_error_cls);
  Py_XDECREF(pol_obj);
}

bool lp_creation_python_policy::_data::
is_py_lp_creation_policy_warning(PyObject *o) const noexcept
{
  return PyObject_IsInstance(o, lp_creation_policy_warning_cls);
}

bool lp_creation_python_policy::_data::
is_py_lp_creation_policy_error(PyObject *o) const noexcept
{
  return o && PyObject_IsInstance(o, lp_creation_policy_error_cls);
}

code_remark lp_creation_python_policy::_data::
convert_py_lp_creation_policy_warning
  (PyObject *o,
   const instantiate_remark_type &instantiate) const
{
  if (!is_py_lp_creation_policy_warning(o)) {
    throw python_except {
      "expected instance of python class \"ccp.LpCreationPolicyWarning\""
    };
  }

  PyObject *msg_member = PyObject_GetAttrString(o, "_msg");
  if (!msg_member) {
    throw python_except {
      "expected \"_msg\" member in python class \"ccp.LpCreationPolicyWarning\""
    };
  }
  if (!PyUnicode_Check(msg_member)) {
    Py_DECREF(msg_member);
    throw python_except {
      ("expected string value in "
       "python member \"ccp.LpCreationPolicyWarning._msg\"")
    };
  }
  const char *pmsg = PyUnicode_AsUTF8(msg_member);
  if (!pmsg) {
    Py_DECREF(msg_member);
    throw python_except {
      ("couldn't obtain string value from "
       "python member \"ccp.LpCreationPolicyWarning._msg\"")
    };
  }
  std::string msg;
  try {
    msg = pmsg;
    Py_DECREF(msg_member);
  } catch (...) {
    Py_DECREF(msg_member);
    throw;
  }

  return instantiate(code_remark::severity::warning,
		     std::move(msg));
}

void lp_creation_python_policy::_data::convert_py_lp_creation_policy_warnings
  (PyObject *o,
   const instantiate_remark_type &instantiate,
   code_remarks &remarks) const
{
  if (!o || Py_IsNone(o))
    return;

  // A single warning.
  if (is_py_lp_creation_policy_warning(o)) {
    remarks.add
      (convert_py_lp_creation_policy_warning(o, instantiate));
    return;
  }

  // Otherwiseit is expected that the passed object is an iterable
  // over warning instances.
  if (Py_TYPE(o)->tp_iter == NULL && !PySequence_Check(o)) {
    throw python_except {
      "expected iterable over python \"ccp.LpCreationPolicyWarning\" instances"
    };
  }
  PyObject *iter = PyObject_GetIter(o);
  if (!iter) {
    throw python_except {
      ("failed to obtain iterator over python"
       " \"ccp.LpCreationPolicyWarning\" instances")
    };
  }

  PyObject *e = PyIter_Next(iter);
  while (e) {
    if (!is_py_lp_creation_policy_warning(e)) {
      throw python_except {
	("unexpected non-\"ccp.LpCreationPolicyWarning\" instance"
	 " in python iterable")
      };
      Py_DECREF(e);
      Py_DECREF(iter);
    }
    try {
      remarks.add
	(convert_py_lp_creation_policy_warning(e, instantiate));
    } catch (...) {
      Py_DECREF(e);
      Py_DECREF(iter);
      throw;
    }
    Py_DECREF(e);
    e = PyIter_Next(iter);
  }
  Py_DECREF(iter);
  if (PyErr_Occurred()) {
    throw python_except {
      "failed to iterate over \"ccp.LpCreationPolicyWarning\" instances"
    };
  }
}

code_remark lp_creation_python_policy::_data::
convert_py_lp_creation_policy_error
  (PyObject *o,
   const instantiate_remark_type &instantiate,
   code_remarks &remarks) const
{
  if (!is_py_lp_creation_policy_error(o)) {
    throw python_except {
      "expected instance of python class \"ccp.LpCreationPolicyError\""
    };
  } else if (!PyExceptionInstance_Check(o)) {
    throw python_except {"expected python exception instance"};
  }

  // PyException_GetArgs() is available only from Python 3.12 onwards.
  // In the meanwhile, grab the msg from the _msg member duplicated in
  // from the inheriting class for this purpose..
  PyObject *msg_member = PyObject_GetAttrString(o, "_msg");
  if (!msg_member) {
    throw python_except {
      "failed to retrieve \"ccp.LpCreationPolicyError\" python exception args"
    };
  }
  if (!PyUnicode_Check(msg_member)) {
    Py_DECREF(msg_member);
    throw python_except {
      ("expected string value in "
       "\"ccp.LpCreationPolicyError\" python exception")
    };
  }
  const char *pmsg = PyUnicode_AsUTF8(msg_member);
  if (!pmsg) {
    Py_DECREF(msg_member);
    throw python_except {
      ("couldn't obtain string value from "
       "\"ccp.LpCreationPolicyError\" python exception")
    };
  }
  std::string msg;
  try {
    msg = pmsg;
    Py_DECREF(msg_member);
  } catch (...) {
    Py_DECREF(msg_member);
    throw;
  }

  // Extract and store any warnings preceeding the fatal error.
  PyObject *warnings_member = PyObject_GetAttrString(o, "_warnings");
  if (!warnings_member) {
    throw python_except {
      ("expected \"_warnings\" member in python class"
       " \"ccp.LpCreationPolicyError\"")
    };
  }

  try {
    convert_py_lp_creation_policy_warnings(warnings_member,
					   instantiate,
					   remarks);
    Py_DECREF(warnings_member);
  } catch (...) {
    Py_DECREF(warnings_member);
    throw;
  }

  code_remark err_remark = instantiate(code_remark::severity::fatal,
				       std::move(msg));
  remarks.add(err_remark);
  return err_remark;
}

PyObject* lp_creation_python_policy::_data::
unwrap_py_lp_creation_policy_call_result
  (PyObject *r,
   const instantiate_remark_type &instantiate_remark,
   code_remarks &remarks) const
{
  // The accepted return values from python policy calls are
  // result, (result, ) or (result, warnings). Extract the warnings
  // and unwrap the result.
  if (!r) {
    if (is_py_lp_creation_policy_error(PyErr_Occurred())) {
      PyObject *exc = PyErr_Occurred();
      Py_INCREF(exc);
      PyErr_Clear();
      code_remark remark =
	[&]() {
	  try {
	    return (convert_py_lp_creation_policy_error
		    (exc, instantiate_remark, remarks));
	  } catch (...) {
	    Py_DECREF(exc);
	    throw;
	  }
	}();
      Py_DECREF(exc);
      throw lp_except(std::move(remark));
    } else {
      throw python_except{
	("failed to invoke python \"LpCreationPolicyAbc\""
	 " method implementation")
      };
    }
  } else if (!PyTuple_Check(r)) {
    return r;
  } else if (PyTuple_Size(r) == 1) {
    PyObject *_r = PyTuple_GetItem(r, 0);
    if (_r) {
      Py_INCREF(_r);
      Py_DECREF(r);
    } else  {
      Py_DECREF(r);
      throw python_except {
	"failed to retrieve result tuple entry returned from python policy call"
      };
    }
    return _r;
  } else if (PyTuple_Size(r) != 2) {
    Py_DECREF(r);
    throw python_except {
      "unexpected number of entries in tuple returned from python policy call"
    };
  }

  PyObject *warnings = PyTuple_GetItem(r, 1);
  if (!warnings) {
    Py_DECREF(r);
    throw python_except {
      "failed to retrieve warnings tuple entry returned from python policy call"
    };
  }
  try {
    convert_py_lp_creation_policy_warnings(warnings,
					   instantiate_remark,
					   remarks);
  } catch (...) {
    Py_DECREF(r);
    throw;
  }

  PyObject *_r = PyTuple_GetItem(r, 0);
  if (_r) {
    Py_INCREF(_r);
    Py_DECREF(r);
  } else  {
    Py_DECREF(r);
    throw python_except {
      "failed to retrieve result tuple entry returned from python policy call"
    };
  }
  return _r;
}

PyObject* lp_creation_python_policy::_data::
build_py_loc(const pp_token_index id_tok, const pp_result &pp_result) const
{
  const raw_pp_token_index &raw_id_tok
    = (pp_result.pp_tokens_range_to_raw(pp_tokens_range{id_tok, id_tok + 1})
       .begin);
  const auto id_tok_source =
    pp_result.intersecting_sources_begin(raw_pp_tokens_range{raw_id_tok});
  assert(id_tok_source !=
	 (pp_result.intersecting_sources_end
	  (raw_pp_tokens_range{raw_id_tok})));
  const auto id_tok_line_col
    = (id_tok_source->offset_to_line_col
       (pp_result.get_raw_tokens()[raw_id_tok].get_range_in_file().begin));
  PyObject *loc =
    Py_BuildValue("sKK",
		  id_tok_source->get_filename().c_str(),
		  static_cast<unsigned long long>(id_tok_line_col.first),
		  static_cast<unsigned long long>(id_tok_line_col.second));
  if (!loc) {
    throw python_except{
      "failed to build source location information python tuple"
    };
  }

  return loc;
}

PyObject* lp_creation_python_policy::_data::
get_py_linkage_enum_member(const ast::linkage::linkage_kind &linkage)
  const noexcept
{
  PyObject *linkage_enum_member = nullptr;
  switch (linkage) {
  case ast::linkage::linkage_kind::none:
    linkage_enum_member = linkage_enum_member_none;
    break;

  case ast::linkage::linkage_kind::internal:
    linkage_enum_member = linkage_enum_member_internal;
    break;

  case ast::linkage::linkage_kind::external:
    linkage_enum_member = linkage_enum_member_external;
    break;

  case ast::linkage::linkage_kind::nested_fun_auto:
    linkage_enum_member = linkage_enum_member_nested_fun_auto;
    break;
  }
  return linkage_enum_member;
}

PyObject* lp_creation_python_policy::_data::
wrap_allocated_ids(const allocated_ids::allocated_ids_type &ids)
  const
{
  PyObject *o = allocated_ids::create(ids);
  if (!o) {
    throw python_except{
      "failed to instantiate \"_ccp.AllocatedIds\" interface wrapper"
    };
  }
  return o;
}


lp_creation_policy::symbol_modification lp_creation_python_policy::_data::
convert_symbol_modification(PyObject *o)
{
  if (!PyObject_IsInstance(o, symbol_modification_cls)) {
    Py_DECREF(o);
    throw python_except{
      "expected instance of python class \"ccp.SymbolModification\""
    };
  }

  PyObject *new_name_member =
    PyObject_GetAttrString(o, "_new_name");
  if (!new_name_member) {
    Py_DECREF(o);
    throw python_except {
      "expected \"_new_name\" member in python class \"ccp.SymbolModification\""
    };
  }
  std::string new_name;
  if (!Py_IsNone(new_name_member)) {
    if (!PyUnicode_Check(new_name_member)) {
      Py_DECREF(new_name_member);
      Py_DECREF(o);
      throw python_except {
	("expected string value in "
	 "python member \"ccp.SymbolModification._new_name\"")
      };
    }
    const char *pnew_name = PyUnicode_AsUTF8(new_name_member);
    if (!pnew_name) {
      Py_DECREF(new_name_member);
      Py_DECREF(o);
      throw python_except {
	("couldn't obtain string value from "
	 "python member \"ccp.SymbolModification._new_name\"")
      };
    }
    try {
      new_name = pnew_name;
    } catch (...) {
      Py_DECREF(new_name_member);
      Py_DECREF(o);
      throw;
    }
  }
  Py_DECREF(new_name_member);

  PyObject *linkage_change_member =
    PyObject_GetAttrString(o, "_linkage_change");
  if (!linkage_change_member) {
    Py_DECREF(o);
    throw python_except {
      ("expected \"_linkage_change\" member"
       " in python class \"ccp.SymbolModification\"")
    };
  }
  Py_DECREF(o);
  long linkage_change_val = PyLong_AsLong(linkage_change_member);
  Py_DECREF(linkage_change_member);
  if (linkage_change_val == -1 && PyErr_Occurred()) {
    throw python_except{
      ("could not convert \"ccp.SymbolModification._linkage_change\""
       " python member to int")
    };
  }

  lp_creation_policy::symbol_modification::linkage_change linkage_change;
  if (linkage_change_val == linkage_change_enum_member_none_val) {
    linkage_change =
      lp_creation_policy::symbol_modification::linkage_change::lc_none;
  } else if (linkage_change_val == linkage_change_enum_member_make_static_val) {
    linkage_change =
      lp_creation_policy::symbol_modification::linkage_change::lc_make_static;
  } else if (linkage_change_val == linkage_change_enum_member_make_extern_val) {
    linkage_change =
      lp_creation_policy::symbol_modification::linkage_change::lc_make_extern;
  } else {
    throw python_except{
      ("unrecognized value in \"ccp.SymbolModification._linkage_change\""
       " python member")
    };
  }

  return lp_creation_policy::symbol_modification{
    std::move(new_name), linkage_change
  };
}

lp_creation_policy::externalized_symbol_modification
lp_creation_python_policy::_data::
convert_externalized_symbol_modification(PyObject *o)
{
  if (!PyObject_IsInstance(o, externalized_symbol_modification_cls)) {
    Py_DECREF(o);
    throw python_except{
      "expected instance of python class \"ccp.ExternalizedSymbolModification\""
    };
  }

  PyObject *make_pointer_member =
    PyObject_GetAttrString(o, "_make_pointer");
  if (!make_pointer_member) {
    Py_DECREF(o);
    throw python_except {
      ("expected \"_make_pointer\" member"
       " in python class \"ccp.ExternalizedSymbolModification\"")
    };
  }
  if (!PyBool_Check(make_pointer_member)) {
    Py_DECREF(o);
    throw python_except {
      ("expected Bool type for \"_make_pointer\" member"
       " in python class \"ccp.ExternalizedSymbolModification\"")
    };
  }
  bool make_pointer = Py_IsTrue(make_pointer_member);
  Py_DECREF(make_pointer_member);

  PyObject *symbol_mod_member =
    PyObject_GetAttrString(o, "_symbol_mod");
  if (!symbol_mod_member) {
    Py_DECREF(o);
    throw python_except {
      ("expected \"_symbol_mod\" member"
       " in python class \"ccp.ExternalizedSymbolModification\"")
    };
  }
  try {
    // convert_symbol_modification() consumes the reference.
    lp_creation_policy::symbol_modification sym_mod =
      convert_symbol_modification(symbol_mod_member);
    Py_DECREF(o);
    return lp_creation_policy::externalized_symbol_modification{
      std::move(sym_mod.new_name),
      sym_mod.new_linkage,
      make_pointer
    };
  } catch (...) {
    Py_DECREF(o);
    throw;
  }
}

lp_creation_python_policy::lp_creation_python_policy
(const pp_result &pp_result,
 std::vector<std::string> &&patched_functions,
 const char * const python_policy_mod,
 const char * const python_policy_cls)
  : _pp_result{pp_result}, _patched_functions{std::move(patched_functions)},
    _d{new _data{}}
{
  // Construct a PYTHONPATH for the embedded interpreter.
  std::string python_path;
  const char *env_pypol_path = getenv("KLP_CCP_PYTHONPOLICY_PATH");
  if (env_pypol_path) {
    python_path.append(env_pypol_path);
    python_path.push_back(':');
  }
  python_path.append(KLP_CCP_PYPOL_DIR);
  const char *env_pythonpath = getenv("PYTHONPATH");
  if (env_pythonpath) {
    python_path.push_back(':');
    python_path.append(env_pythonpath);
  }

  PyStatus status;
  PyConfig config;
  PyConfig_InitPythonConfig(&config);
  config.configure_c_stdio = 0;
  config.install_signal_handlers = 0;
  status = PyConfig_SetBytesString(&config,
				   &config.pythonpath_env,
				   python_path.c_str());
  if (PyStatus_Exception(status)) {
    PyConfig_Clear(&config);
    // It would be better to obtain the error message as a string and
    // print that later, without exiting, but AFAICT that's not
    // possible.
    Py_ExitStatusException(status);
  }

  if (PyImport_AppendInittab("_ccp", pyinit_ccp_interface_mod)) {
    throw python_except{"could not extend python's inittab"};
  }

  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    PyConfig_Clear(&config);
    // It would be better to obtain the error message as a string and
    // print that later, without exiting, but AFAICT that's not
    // possible.
    Py_ExitStatusException(status);
  }
  PyConfig_Clear(&config);

  // Load the distributed ccp.py mod and inspect it further below.
  PyObject *ccp_mod_name = PyUnicode_DecodeFSDefault("ccp");
  PyObject *ccp_mod = PyImport_Import(ccp_mod_name);
  Py_XDECREF(ccp_mod_name);
  if (!ccp_mod)
    throw python_except{"could not import \"ccp\" python module"};

  // Lookup the Linkage enum member values and store them away for
  // comparison later on.
  PyObject *linkage_enum_cls =
    PyObject_GetAttrString(ccp_mod, "Linkage");
  if (!linkage_enum_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      "could not locate \"ccp.Linkage\" python enum class"
    };
  }
  for (auto &m : {
      std::make_pair(&_d->linkage_enum_member_none,
		     "NONE"),
      std::make_pair(&_d->linkage_enum_member_internal,
		     "INTERNAL"),
      std::make_pair(&_d->linkage_enum_member_external,
		     "EXTERNAL"),
      std::make_pair(&_d->linkage_enum_member_nested_fun_auto,
		     "NESTED_FUN_AUTO"),
    }) {
    *std::get<0>(m) = PyObject_GetAttrString(linkage_enum_cls, std::get<1>(m));
    if (!*std::get<0>(m)) {
      Py_DECREF(linkage_enum_cls);
      Py_DECREF(ccp_mod);
      throw python_except{
	"could not locate expected \"ccp.Linkage\" python enum member"
      };
    }
  }
  Py_DECREF(linkage_enum_cls);

  // Lookup the SymbolModification.LinkageChange enum member values
  // and store them away for comparison later on.
  _d->symbol_modification_cls =
    PyObject_GetAttrString(ccp_mod, "SymbolModification");
  if (!_d->symbol_modification_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      "could not locate \"ccp.SymbolModification\" python class"
    };
  }
  PyObject *linkage_change_enum_cls =
    PyObject_GetAttrString(_d->symbol_modification_cls, "LinkageChange");
  if (!linkage_change_enum_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      ("could not locate \"ccp.SymbolModification.LinkageChange\""
       " python enum class")
    };
  }
  for (auto &m : {
      std::make_pair(&_d->linkage_change_enum_member_none_val,
		     "NONE"),
      std::make_pair(&_d->linkage_change_enum_member_make_static_val,
		     "MAKE_STATIC"),
      std::make_pair(&_d->linkage_change_enum_member_make_extern_val,
		     "MAKE_EXTERN"),
    }) {
    PyObject *enum_member=
      PyObject_GetAttrString(linkage_change_enum_cls, std::get<1>(m));
    if (!enum_member) {
      Py_DECREF(linkage_change_enum_cls);
      Py_DECREF(ccp_mod);
      throw python_except{
	("could not locate expected \"ccp.SymbolModification.LinkageChange\""
	 " python enum member")
      };
    }
    *std::get<0>(m) = PyLong_AsLong(enum_member);
    Py_DECREF(enum_member);
    if (*std::get<0>(m) == -1 && PyErr_Occurred()) {
      Py_DECREF(linkage_change_enum_cls);
      Py_DECREF(ccp_mod);
      throw python_except{
	("could not convert \"ccp.SymbolModification.LinkageChange\""
	 " python enum member to int")
      };
    }
  }
  Py_DECREF(linkage_change_enum_cls);

  // Lookup a couple more classes needed for type inspection over the
  // course of the run.
  _d->externalized_symbol_modification_cls =
    PyObject_GetAttrString(ccp_mod, "ExternalizedSymbolModification");
  if (!_d->externalized_symbol_modification_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      "could not locate \"ccp.ExternalizedSymbolModification\" python class"
    };
  }

  _d->lp_creation_policy_warning_cls =
    PyObject_GetAttrString(ccp_mod, "LpCreationPolicyWarning");
  if (!_d->lp_creation_policy_warning_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      "could not locate \"ccp.LpCreationPolicyWarning\" python class"
    };
  }

  _d->lp_creation_policy_error_cls =
    PyObject_GetAttrString(ccp_mod, "LpCreationPolicyError");
  if (!_d->lp_creation_policy_error_cls) {
    Py_DECREF(ccp_mod);
    throw python_except{
      "could not locate \"ccp.LpCreationPolicyError\" python class"
    };
  }

  // Finally, lookup the "CcpLpCreationPolicy" Abstract Base Class
  // (ABC) defined in the cls.py mod -- it will be used below
  // for verifying the user specified policy implements it.
  PyObject *ccp_lp_creation_policy_abc_cls =
    PyObject_GetAttrString(ccp_mod, "LpCreationPolicyAbc");
  Py_DECREF(ccp_mod);
  if (!ccp_lp_creation_policy_abc_cls) {
    throw python_except{
      "could not locate \"ccp.LpCreationPolicyAbc\" python class"
    };
  }

  // Load the module providing the user specified policy class.
  PyObject *pol_mod_name = PyUnicode_DecodeFSDefault(python_policy_mod);
  PyObject *pol_mod = PyImport_Import(pol_mod_name);
  Py_XDECREF(pol_mod_name);
  if (!pol_mod) {
    Py_DECREF(ccp_lp_creation_policy_abc_cls);
    throw python_except{"could not import the specified python policy module"};
  }
  // And locate the class.
  PyObject *pol_cls = PyObject_GetAttrString(pol_mod, python_policy_cls);
  Py_DECREF(pol_mod);
  if (!pol_cls) {
    Py_DECREF(ccp_lp_creation_policy_abc_cls);
    throw python_except{"could not locate the specified python policy class"};
  }

  // Verify that the specified class implementing policy is a subclass
  // of the CcpLpCreationPolicy ABC.
  if (!PyObject_IsSubclass(pol_cls, ccp_lp_creation_policy_abc_cls)) {
    Py_DECREF(ccp_lp_creation_policy_abc_cls);
    Py_DECREF(pol_cls);
    throw python_except{
     "specified python policy class isn't a subclass of \"CcpLpCreationPolicy\""
    };
  }
  Py_DECREF(ccp_lp_creation_policy_abc_cls);

  // Instantiate. That's being done by calling the cls object
  // c.f. type_call() in the Python sources.
  if (!PyCallable_Check(pol_cls)) {
    throw python_except{
      "specified python policy class can't get instantiated"
    };
  }
  PyObject *args = PyTuple_New(0);
  if (!args) {
    Py_DECREF(pol_cls);
    throw python_except{
      "failed to prepare python policy class constructor invocation"
    };
  }
  _d->pol_obj = PyObject_CallObject(pol_cls, args);
  Py_DECREF(pol_cls);
  Py_DECREF(args);
  if (!_d->pol_obj)
    throw python_except{"python policy class construction failed"};
}

lp_creation_python_policy::~lp_creation_python_policy() noexcept
{
  _d.reset();
}

void lp_creation_python_policy::teardown_python() noexcept
{
  Py_FinalizeEx();
}

bool lp_creation_python_policy::
is_patched(const ast::function_definition &fd, code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);
  if (std::find(_patched_functions.cbegin(),
		_patched_functions.cend(),
		_pp_result.get_pp_tokens()[id_tok].get_value())
      != _patched_functions.cend()) {
    return true;
  }

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "is_patched_fun", "s",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str());

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyBool_Check(r)) {
    Py_DECREF(r);
    throw python_except{
      ("expected Bool result from python"
       " \"LpCreationPolicyAbc.is_patched_fun()\" policy implementation")
    };
  }
  bool result = Py_IsTrue(r);
  Py_DECREF(r);
  return result;
}

bool lp_creation_python_policy::
is_header_eligible(const pp_result::header_inclusion_root &pre_include,
		   code_remarks &remarks) const
{
  return _is_header_eligible(pre_include, true, remarks);
}

bool lp_creation_python_policy::
is_header_eligible(const pp_result::header_inclusion_child &h,
		   code_remarks &remarks) const
{
  return _is_header_eligible(h, false, remarks);
}

bool lp_creation_python_policy::
is_function_externalizable(const ast::function_definition &fd,
			   code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);
  return _is_function_externalizable(id_tok,
				     fd.get_linkage().get_linkage_kind(),
				     true,
				     remarks);
}

bool lp_creation_python_policy::
is_function_externalizable(const ast::init_declarator &id,
			   code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(id);
  return _is_function_externalizable(id_tok,
				     id.get_linkage().get_linkage_kind(),
				     false,
				     remarks);
}

bool lp_creation_python_policy::
is_function_externalization_preferred(const ast::function_definition &fd,
				      const bool in_eligible_headers,
				      code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);
  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *linkage_enum_member =
    _d->get_py_linkage_enum_member(fd.get_linkage().get_linkage_kind());
  PyObject *definition_loc = _d->build_py_loc(id_tok, _pp_result);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "is_function_externalization_preferred",
			"sOOO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			linkage_enum_member,
			in_eligible_headers ? Py_True : Py_False,
			definition_loc);
  Py_DECREF(definition_loc);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyBool_Check(r)) {
    Py_DECREF(r);
    throw python_except{
      ("expected Bool result from python"
       " \"LpCreationPolicyAbc.is_function_externalization_preferred()\""
       " policy implementation")
    };
  }
  bool result = Py_IsTrue(r);
  Py_DECREF(r);
  return result;
}

bool lp_creation_python_policy::
shall_externalize_object(const ast::init_declarator &id,
			 code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(id);

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *linkage_enum_member =
    _d->get_py_linkage_enum_member(id.get_linkage().get_linkage_kind());
  PyObject *declaration_loc = _d->build_py_loc(id_tok, _pp_result);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "shall_externalize_object",
			"sOO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			linkage_enum_member,
			declaration_loc);
  Py_DECREF(declaration_loc);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyBool_Check(r)) {
    Py_DECREF(r);
    throw python_except{
      ("expected Bool result from python"
       " \"LpCreationPolicyAbc.shall_externalize_object()\""
       " policy implementation")
    };
  }
  bool result = Py_IsTrue(r);
  Py_DECREF(r);
  return result;
}

lp_creation_python_policy::externalized_symbol_modification
lp_creation_python_policy::
get_sym_mod_for_externalized_fun(const ast::direct_declarator_id &ddid,
				 const allocated_ids_type &allocated_ids,
				 code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(ddid);

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *wrapped_allocated_ids = _d->wrap_allocated_ids(allocated_ids);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "get_sym_mod_for_externalized_fun",
			"sO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			wrapped_allocated_ids);
  (reinterpret_cast<struct allocated_ids *>
   (wrapped_allocated_ids))
    ->_wrapped_ids = nullptr;
  Py_DECREF(wrapped_allocated_ids);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  lp_creation_python_policy::externalized_symbol_modification ext_sym_mod
    = _d->convert_externalized_symbol_modification(r);
  if (ext_sym_mod.sym_mod.new_name ==
      _pp_result.get_pp_tokens()[id_tok].get_value()) {
    ext_sym_mod.sym_mod.new_name = std::string{};
  }
  return ext_sym_mod;
}

lp_creation_python_policy::symbol_modification
lp_creation_python_policy::
get_sym_mod_for_patched_fun(const ast::function_definition &fd,
			    const allocated_ids_type &allocated_ids,
			    code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *wrapped_allocated_ids = _d->wrap_allocated_ids(allocated_ids);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "get_sym_mod_for_patched_fun",
			"sO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			wrapped_allocated_ids);
  (reinterpret_cast<struct allocated_ids *>
   (wrapped_allocated_ids))
    ->_wrapped_ids = nullptr;
  Py_DECREF(wrapped_allocated_ids);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  lp_creation_python_policy::symbol_modification sym_mod
    = _d->convert_symbol_modification(r);
  if (sym_mod.new_name == _pp_result.get_pp_tokens()[id_tok].get_value()) {
    sym_mod.new_name = std::string{};
  }
  return sym_mod;
}

std::string  lp_creation_python_policy::
rename_rewritten_closure_fun(const ast::function_definition &fd,
			     const allocated_ids_type &allocated_ids,
			     code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(fd);

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *wrapped_allocated_ids = _d->wrap_allocated_ids(allocated_ids);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "rename_rewritten_closure_fun",
			"sO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			wrapped_allocated_ids);
  (reinterpret_cast<struct allocated_ids *>
   (wrapped_allocated_ids))
    ->_wrapped_ids = nullptr;
  Py_DECREF(wrapped_allocated_ids);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyUnicode_Check(r)) {
    Py_DECREF(r);
    throw python_except {
      ("expected string result from python"
       " \"LpCreationPolicyAbc.rename_rewritten_closure_fun()\""
       " policy implementation")
    };
  }
  const char *pnew_name = PyUnicode_AsUTF8(r);
  if (!pnew_name) {
    Py_DECREF(r);
    throw python_except {
      ("couldn't obtain string value from result returned by"
       " \"LpCreationPolicyAbc.rename_rewritten_closure_fun()\""
       " policy implementation")
    };
  }
  std::string new_name;
  try {
    new_name = pnew_name;
    Py_DECREF(r);
  } catch (...) {
    Py_DECREF(r);
    throw;
  }
  if (new_name == _pp_result.get_pp_tokens()[id_tok].get_value()) {
    new_name = std::string{};
  }
  return new_name;
}

lp_creation_python_policy::externalized_symbol_modification
lp_creation_python_policy::
get_sym_mod_for_externalized_obj(const ast::init_declarator &id,
				 const allocated_ids_type &allocated_ids,
				 code_remarks &remarks) const
{
  const pp_token_index id_tok = _get_id_tok(id);

  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *wrapped_allocated_ids = _d->wrap_allocated_ids(allocated_ids);
  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "get_sym_mod_for_externalized_obj",
			"sO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			wrapped_allocated_ids);
  (reinterpret_cast<struct allocated_ids *>
   (wrapped_allocated_ids))
    ->_wrapped_ids = nullptr;
  Py_DECREF(wrapped_allocated_ids);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  lp_creation_python_policy::externalized_symbol_modification ext_sym_mod
    = _d->convert_externalized_symbol_modification(r);
  if (ext_sym_mod.sym_mod.new_name ==
      _pp_result.get_pp_tokens()[id_tok].get_value()) {
    ext_sym_mod.sym_mod.new_name = std::string{};
  }
  return ext_sym_mod;
}

bool lp_creation_python_policy::
_is_header_eligible(const pp_result::header_inclusion_node &h,
		    const bool is_pre_include,
		    code_remarks &remarks) const
{
  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	h, range_in_file{0}
      };
    };

  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "is_header_eligible", "sO",
			h.get_filename().c_str(),
			is_pre_include ? Py_True : Py_False);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyBool_Check(r)) {
    Py_DECREF(r);
    throw python_except{
      ("expected Bool result from python"
       " \"LpCreationPolicyAbc.is_header_eligible()\" policy implementation")
    };
  }
  bool result = Py_IsTrue(r);
  Py_DECREF(r);
  return result;
}

bool lp_creation_python_policy::
_is_function_externalizable(const pp_token_index id_tok,
			    const ast::linkage::linkage_kind linkage,
			    const bool is_definition,
			    code_remarks &remarks) const
{
  const auto &instantiate_remark =
    [&](const code_remark::severity severity,
	std::string &&e) {
      return code_remark{
	severity,
	std::move(e),
	_pp_result,
	id_tok,
      };
    };

  PyObject *linkage_enum_member =
    _d->get_py_linkage_enum_member(linkage);

  PyObject *definition_loc;
  if (is_definition) {
    definition_loc = _d->build_py_loc(id_tok, _pp_result);
  } else {
    definition_loc = [](){Py_RETURN_NONE;}();
  }

  PyObject *r =
    PyObject_CallMethod(_d->pol_obj, "is_function_externalizable", "sOO",
			_pp_result.get_pp_tokens()[id_tok].get_value().c_str(),
			linkage_enum_member,
			definition_loc);
  Py_DECREF(definition_loc);

  r = _d->unwrap_py_lp_creation_policy_call_result(r, instantiate_remark,
						   remarks);
  assert(r != nullptr);
  if (!PyBool_Check(r)) {
    Py_DECREF(r);
    throw python_except{
      ("expected Bool result from python"
       " \"LpCreationPolicyAbc.is_function_externalizable()\""
       " policy implementation")
    };
  }
  bool result = Py_IsTrue(r);
  Py_DECREF(r);
  return result;
}

pp_token_index lp_creation_python_policy::
_get_id_tok(const ast::init_declarator &id) noexcept
{
  return _get_id_tok(id.get_declarator().get_direct_declarator_id());
}

pp_token_index lp_creation_python_policy::
_get_id_tok(const ast::function_definition &fd) noexcept
{
  return _get_id_tok(fd.get_declarator().get_direct_declarator_id());
}

pp_token_index lp_creation_python_policy::
_get_id_tok(const ast::direct_declarator_id &ddid) noexcept
{
  return ddid.get_id_tok();
}
