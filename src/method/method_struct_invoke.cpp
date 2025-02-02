/*
 *
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "method_struct_invoke.hpp"

#include <algorithm> /* std::for_each */

#include "object_representation.h"	/* OR_ */
#include "method_struct_value.hpp"

#if defined (SERVER_MODE)
#include "network.h" /* METHOD_CALL */
#include "network_interface_sr.h" /* xs_receive_data_from_client() */
#include "server_support.h"	/* css_send_reply_and_data_to_client(), css_get_comm_request_id() */
#include "thread_manager.hpp"
#include "log_impl.h"
#endif

namespace cubmethod
{

//////////////////////////////////////////////////////////////////////////
// header
//////////////////////////////////////////////////////////////////////////
  header::header (uint64_t i, int c, int r)
    : id (i)
    , command (c)
    , req_id (r)
  {
    //
  }

  header::header (cubpacking::unpacker &unpacker)
  {
    unpack (unpacker);
  }

  void
  header::pack (cubpacking::packer &serializator) const
  {
    serializator.pack_bigint (id);
    serializator.pack_int (command);
    serializator.pack_int (req_id);
  }

  size_t
  header::get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const
  {
    size_t size = serializator.get_packed_bigint_size (start_offset); // id
    size += serializator.get_packed_int_size (size); // command
    size += serializator.get_packed_int_size (size); // req_id
    return size;
  }

  void
  header::unpack (cubpacking::unpacker &deserializator)
  {
    deserializator.unpack_bigint (id);
    deserializator.unpack_int (command);
    deserializator.unpack_int (req_id);
  }

//////////////////////////////////////////////////////////////////////////
// Common structure implementation
//////////////////////////////////////////////////////////////////////////
  prepare_args::prepare_args (METHOD_GROUP_ID id, int tid, METHOD_TYPE type,
			      std::vector<std::reference_wrapper<DB_VALUE>> &vec)
    : group_id (id), tran_id (tid), type (type), args (vec)
  {
    //
  }

  void
  prepare_args::pack (cubpacking::packer &serializator) const
  {
    serializator.pack_bigint (group_id);
    switch (type)
      {
      case METHOD_TYPE_INSTANCE_METHOD:
      case METHOD_TYPE_CLASS_METHOD:
      {
	serializator.pack_all (args);
	break;
      }
      case METHOD_TYPE_JAVA_SP:
      {
	serializator.pack_int (tran_id);
	serializator.pack_int (args.size ());
	dbvalue_java dbvalue_wrapper;
	std::for_each (args.begin (), args.end (),[&serializator, &dbvalue_wrapper] (DB_VALUE & value)
	{
	  dbvalue_wrapper.value = &value;
	  dbvalue_wrapper.pack (serializator);
	});
	break;
      }
      default:
	assert (false);
	break;
      }
  }

  void
  prepare_args::unpack (cubpacking::unpacker &deserializator)
  {
    // TODO: unpacking is not necessary
    assert (false);
  }

  size_t
  prepare_args::get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const
  {
    size_t size = serializator.get_packed_bigint_size (start_offset);	// group id
    switch (type)
      {
      case METHOD_TYPE_INSTANCE_METHOD:
      case METHOD_TYPE_CLASS_METHOD:
      {
	size += serializator.get_packed_bigint_size (size);	// arg count
	std::for_each (args.begin (), args.end (),
		       [&size, &serializator] (DB_VALUE & value)
	{
	  size += serializator.get_packed_db_value_size (value, size);	// DB_VALUEs
	});
	break;
      }
      case METHOD_TYPE_JAVA_SP:
      {
	size += serializator.get_packed_int_size (size);	// tran_id
	size += serializator.get_packed_int_size (size);	// arg count
	dbvalue_java dbvalue_wrapper;
	std::for_each (args.begin (), args.end (),
		       [&size, &serializator, &dbvalue_wrapper] (DB_VALUE & value)
	{
	  dbvalue_wrapper.value = &value;
	  size += dbvalue_wrapper.get_packed_size (serializator, size); /* value */
	});
	break;
      }
      default:
	assert (false);
	break;
      }
    return size;
  }

//////////////////////////////////////////////////////////////////////////
// Method Builtin (C Language Method)
//////////////////////////////////////////////////////////////////////////
  invoke_builtin::invoke_builtin (METHOD_GROUP_ID g_id, method_sig_node *sig)
    : group_id (g_id)
    , sig (sig)
  {
    //
  }

  void
  invoke_builtin::pack (cubpacking::packer &serializator) const
  {
    serializator.pack_bigint (group_id);
    sig->pack (serializator);
  }

  void
  invoke_builtin::unpack (cubpacking::unpacker &deserializator)
  {
    deserializator.unpack_bigint (group_id);
    sig = new METHOD_SIG ();
    sig->unpack (deserializator);
  }

  size_t
  invoke_builtin::get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const
  {
    size_t size = serializator.get_packed_bigint_size (start_offset); //group id
    size += sig->get_packed_size (serializator, size); // sig
    return size;
  }

//////////////////////////////////////////////////////////////////////////
// Method Java
//////////////////////////////////////////////////////////////////////////
  invoke_java::invoke_java (METHOD_GROUP_ID id, int tid, method_sig_node *sig, bool tc)
    : group_id (id)
    , tran_id (tid)
  {
    signature.assign (sig->method_name);
    num_args = sig->num_method_args;

    arg_pos.resize (num_args);
    arg_mode.resize (num_args);
    arg_type.resize (num_args);

    for (int i = 0; i < num_args; i++)
      {
	arg_pos[i] = sig->method_arg_pos[i];
	arg_mode[i] = sig->arg_info.arg_mode[i];
	arg_type[i] = sig->arg_info.arg_type[i];
      }

    result_type = sig->arg_info.result_type;
    transaction_control = tc;
  }

  void
  invoke_java::pack (cubpacking::packer &serializator) const
  {
    serializator.pack_bigint (group_id);
    serializator.pack_int (tran_id);
    serializator.pack_string (signature);
    serializator.pack_int (num_args);

    for (int i = 0; i < num_args; i++)
      {
	serializator.pack_int (arg_pos[i]);
	serializator.pack_int (arg_mode[i]);
	serializator.pack_int (arg_type[i]);
      }

    serializator.pack_int (result_type);
    serializator.pack_bool (transaction_control);
  }

  void
  invoke_java::unpack (cubpacking::unpacker &deserializator)
  {
    // TODO: unpacking is not necessary
    assert (false);
  }

  size_t
  invoke_java::get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const
  {
    size_t size = serializator.get_packed_bigint_size (start_offset); // group_id
    size += serializator.get_packed_int_size (size); // tran_id
    size += serializator.get_packed_string_size (signature, size); // signature
    size += serializator.get_packed_int_size (size); // num_args

    for (int i = 0; i < num_args; i++)
      {
	size += serializator.get_packed_int_size (size); // arg_pos
	size += serializator.get_packed_int_size (size); // arg_mode
	size += serializator.get_packed_int_size (size); // arg_type
      }

    size += serializator.get_packed_int_size (size); // return_type
    size += serializator.get_packed_bool_size (size); // transaction_control
    return size;
  }
}
