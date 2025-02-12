(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

val is_enforceable : Typing_env_types.env -> Typing_defs.decl_ty -> bool

val get_enforcement :
  Typing_env_types.env -> Typing_defs.decl_ty -> Typing_defs_core.enforcement

val compute_enforced_ty :
  Typing_env_types.env ->
  Typing_defs.decl_ty ->
  Typing_defs.decl_ty Typing_defs.possibly_enforced_ty

val compute_enforced_and_pessimize_ty :
  Typing_env_types.env ->
  ?explicitly_untrusted:bool ->
  Typing_defs.decl_ty ->
  Typing_defs.decl_ty Typing_defs.possibly_enforced_ty

val compute_enforced_and_pessimize_fun_type :
  Typing_env_types.env ->
  Typing_defs.decl_fun_type ->
  Typing_defs.decl_ty Typing_defs.fun_type
