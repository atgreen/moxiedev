/* Tcl/Tk command definitions for Insight - Breakpoints.
   Copyright (C) 2001, 2002, 2008, 2009 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "symtab.h"
#include "symfile.h"
#include "source.h"
#include "linespec.h"
#include "breakpoint.h"
#include "tracepoint.h"
#include "gdb_string.h"
#include <tcl.h>
#include "gdbtk.h"
#include "gdbtk-cmds.h"
#include "observer.h"
#include "arch-utils.h"
#include "exceptions.h"

/* From breakpoint.c */
extern struct breakpoint *breakpoint_chain;

/* From gdbtk-hooks.c */
extern void report_error (void);

/* These two lookup tables are used to translate the type & disposition fields
   of the breakpoint structure (respectively) into something gdbtk understands.
   They are also used in gdbtk-hooks.c */

char *bptypes[] =
  {"none", "breakpoint", "hw breakpoint", "until",
   "finish", "watchpoint", "hw watchpoint",
   "read watchpoint", "acc watchpoint",
   "longjmp", "longjmp resume", "step resume",
   "sigtramp", "watchpoint scope",
   "call dummy", "shlib events", "catch load",
   "catch unload", "catch fork", "catch vfork",
   "catch exec", "catch catch", "catch throw"
  };
char *bpdisp[] =
  {"delete", "delstop", "disable", "donttouch"};

/* Is this breakpoint interesting to a user interface? */
#define BREAKPOINT_IS_INTERESTING(bp) \
((bp)->type == bp_breakpoint             \
 || (bp)->type == bp_hardware_breakpoint \
 || (bp)->type == bp_watchpoint          \
 || (bp)->type == bp_hardware_watchpoint \
 || (bp)->type == bp_read_watchpoint     \
 || (bp)->type == bp_access_watchpoint)

/* Is this breakpoint a watchpoint?  */
#define BREAKPOINT_IS_WATCHPOINT(bp)					      \
((bp)->type == bp_watchpoint						      \
 || (bp)->type == bp_hardware_watchpoint				      \
 || (bp)->type == bp_read_watchpoint					      \
 || (bp)->type == bp_access_watchpoint)

/* Breakpoint/Tracepoint lists. Unfortunately, gdb forces us to
   keep a list of breakpoints, too. Why couldn't it be done like
   treacepoints? */
#define DEFAULT_LIST_SIZE 32
static struct breakpoint **breakpoint_list;
static int breakpoint_list_size = DEFAULT_LIST_SIZE;

/*
 * Forward declarations
 */

/* Breakpoint-related functions */
static int gdb_find_bp_at_addr (ClientData, Tcl_Interp *, int,
				Tcl_Obj * CONST objv[]);
static int gdb_find_bp_at_line (ClientData, Tcl_Interp *, int,
				Tcl_Obj * CONST objv[]);
static int gdb_get_breakpoint_info (ClientData, Tcl_Interp *, int,
				    Tcl_Obj * CONST[]);
static int gdb_get_breakpoint_list (ClientData, Tcl_Interp *, int,
				    Tcl_Obj * CONST[]);
static int gdb_set_bp (ClientData, Tcl_Interp *, int, Tcl_Obj * CONST objv[]);

/* Tracepoint-related functions */
static int gdb_actions_command (ClientData, Tcl_Interp *, int,
				Tcl_Obj * CONST objv[]);
static int gdb_get_trace_frame_num (ClientData, Tcl_Interp *, int,
				    Tcl_Obj * CONST objv[]);
static int gdb_get_tracepoint_info (ClientData, Tcl_Interp *, int,
				    Tcl_Obj * CONST objv[]);
static int gdb_get_tracepoint_list (ClientData, Tcl_Interp *, int,
				    Tcl_Obj * CONST objv[]);
static int gdb_trace_status (ClientData, Tcl_Interp *, int,
			     Tcl_Obj * CONST[]);
static int gdb_tracepoint_exists_command (ClientData, Tcl_Interp *,
					  int, Tcl_Obj * CONST objv[]);
static Tcl_Obj *get_breakpoint_commands (struct command_line *cmd);

static int tracepoint_exists (char *args);

/* Breakpoint/tracepoint events and related functions */

void gdbtk_create_breakpoint (int);
void gdbtk_delete_breakpoint (int);
void gdbtk_modify_breakpoint (int);
void gdbtk_create_tracepoint (int);
void gdbtk_delete_tracepoint (int);
void gdbtk_modify_tracepoint (int);
static void breakpoint_notify (int, const char *);
static void tracepoint_notify (int, const char *);

int
Gdbtk_Breakpoint_Init (Tcl_Interp *interp)
{
  /* Breakpoint commands */
  Tcl_CreateObjCommand (interp, "gdb_find_bp_at_addr", gdbtk_call_wrapper,
			gdb_find_bp_at_addr, NULL);
  Tcl_CreateObjCommand (interp, "gdb_find_bp_at_line", gdbtk_call_wrapper,
			gdb_find_bp_at_line, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_breakpoint_info", gdbtk_call_wrapper,
			gdb_get_breakpoint_info, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_breakpoint_list", gdbtk_call_wrapper,
			gdb_get_breakpoint_list, NULL);
  Tcl_CreateObjCommand (interp, "gdb_set_bp", gdbtk_call_wrapper, gdb_set_bp, NULL);

  /* Tracepoint commands */
  Tcl_CreateObjCommand (interp, "gdb_actions",
			gdbtk_call_wrapper, gdb_actions_command, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_trace_frame_num",
			gdbtk_call_wrapper, gdb_get_trace_frame_num, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_tracepoint_info",
			gdbtk_call_wrapper, gdb_get_tracepoint_info, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_tracepoint_list",
			gdbtk_call_wrapper, gdb_get_tracepoint_list, NULL);
  Tcl_CreateObjCommand (interp, "gdb_is_tracing",
			gdbtk_call_wrapper, gdb_trace_status,	NULL);
  Tcl_CreateObjCommand (interp, "gdb_tracepoint_exists",
			gdbtk_call_wrapper, gdb_tracepoint_exists_command, NULL);

  /* Initialize our tables of BPs. */
  breakpoint_list = (struct breakpoint **) xmalloc (breakpoint_list_size * sizeof (struct breakpoint *));
  memset (breakpoint_list, 0, breakpoint_list_size * sizeof (struct breakpoint *));

  return TCL_OK;
}

/*
 *  This section contains commands for manipulation of breakpoints.
 */

/* set a breakpoint by source file and line number
   flags are as follows:
   least significant 2 bits are disposition, rest is 
   type (normally 0).

   enum bptype {
   bp_breakpoint,                Normal breakpoint 
   bp_hardware_breakpoint,      Hardware assisted breakpoint
   }

   Disposition of breakpoint.  Ie: what to do after hitting it.
   enum bpdisp {
   del,                         Delete it
   del_at_next_stop,            Delete at next stop, whether hit or not
   disable,                     Disable it 
   donttouch                    Leave it alone 
   };
*/


/* This implements the tcl command "gdb_find_bp_at_addr"

* Tcl Arguments:
*    addr:     CORE_ADDR
* Tcl Result:
*    It returns a list of breakpoint numbers
*/
static int
gdb_find_bp_at_addr (ClientData clientData, Tcl_Interp *interp,
		     int objc, Tcl_Obj *CONST objv[])
{
  int i;
  CORE_ADDR addr;
  Tcl_WideInt waddr;

  if (objc != 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "address");
      return TCL_ERROR;
    }
  
  if (Tcl_GetWideIntFromObj (interp, objv[1], &waddr) != TCL_OK)
    return TCL_ERROR;
  addr = waddr;

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  for (i = 0; i < breakpoint_list_size; i++)
    {
      if (breakpoint_list[i] != NULL && breakpoint_list[i]->loc != NULL
	  && breakpoint_list[i]->loc->address == addr)
	Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				  Tcl_NewIntObj (i));
    }

  return TCL_OK;
}

/* This implements the tcl command "gdb_find_bp_at_line"

* Tcl Arguments:
*    filename: the file in which to find the breakpoint
*    line:     the line number for the breakpoint
* Tcl Result:
*    It returns a list of breakpoint numbers
*/
static int
gdb_find_bp_at_line (ClientData clientData, Tcl_Interp *interp,
		     int objc, Tcl_Obj *CONST objv[])

{
  struct symtab *s;
  int i, line;

  if (objc != 3)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "filename line");
      return TCL_ERROR;
    }

  s = lookup_symtab (Tcl_GetStringFromObj (objv[1], NULL));
  if (s == NULL)
    return TCL_ERROR;

  if (Tcl_GetIntFromObj (interp, objv[2], &line) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  for (i = 0; i < breakpoint_list_size; i++)
    if (breakpoint_list[i] != NULL
	&& breakpoint_list[i]->line_number == line
	&& !strcmp (breakpoint_list[i]->source_file, s->filename))
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				Tcl_NewIntObj (i));

  return TCL_OK;
}

/* This implements the tcl command gdb_get_breakpoint_info
 *
 * Tcl Arguments:
 *   breakpoint_number
 * Tcl Result:
 *   A list with {file, function, line_number, address, type, enabled?,
 *                disposition, ignore_count, {list_of_commands},
 *                condition, thread, hit_count user_specification}
 */
static int
gdb_get_breakpoint_info (ClientData clientData, Tcl_Interp *interp, int objc,
			 Tcl_Obj *CONST objv[])
{
  struct symtab_and_line sal;
  int bpnum;
  struct breakpoint *b;
  char *funcname, *filename;
  int isPending = 0;

  Tcl_Obj *new_obj;

  if (objc != 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "breakpoint");
      return TCL_ERROR;
    }

  if (Tcl_GetIntFromObj (NULL, objv[1], &bpnum) != TCL_OK)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  b = (bpnum <= breakpoint_list_size ? breakpoint_list[bpnum] : NULL);
  if (!b || b->type != bp_breakpoint)
    {
      gdbtk_set_result (interp, "Breakpoint #%d does not exist.", bpnum);
      return TCL_ERROR;
    }

  isPending = (b->loc == NULL);
  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  /* Pending breakpoints will display "<PENDING>" as the file name and the 
     user expression into the Function field of the breakpoint view.
    "0" and "0" in the line number and address field.  */
  if (isPending)
    {
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewStringObj ("<PENDING>", -1));
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewStringObj (b->addr_string, -1));
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewIntObj (0));
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewIntObj (0));
    }
  else
    {
      sal = find_pc_line (b->loc->address, 0);

      filename = symtab_to_filename (sal.symtab);
      if (filename == NULL)
        filename = "";
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewStringObj (filename, -1));
      funcname = pc_function_name (b->loc->address);
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewStringObj (funcname, -1));
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewIntObj (b->line_number));
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                Tcl_NewStringObj (core_addr_to_string
                               (b->loc->address), -1));
  }

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (bptypes[b->type], -1));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewBooleanObj (b->enable_state == bp_enabled));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (bpdisp[b->disposition], -1));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewIntObj (b->ignore_count));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    get_breakpoint_commands (b->commands));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (b->cond_string, -1));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewIntObj (b->thread));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewIntObj (b->hit_count));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (BREAKPOINT_IS_WATCHPOINT (b)
					      ? b->exp_string
					      : b->addr_string, -1));

  return TCL_OK;
}

/* Helper function for gdb_get_breakpoint_info, this function is
   responsible for figuring out what to type at the "commands" command
   in gdb's cli in order to get at the same command list passed here. */

static Tcl_Obj *
get_breakpoint_commands (struct command_line *cmd)
{
  Tcl_Obj *obj, *tmp;

  obj = Tcl_NewObj ();
  while (cmd != NULL)
    {
      switch (cmd->control_type)
	{
	case simple_control:
	  /* A simple command. Just append it. */
	  Tcl_ListObjAppendElement (NULL, obj,
				    Tcl_NewStringObj (cmd->line, -1));
	  break;

	case break_control:
	  /* A loop_break */
	  Tcl_ListObjAppendElement (NULL, obj,
				    Tcl_NewStringObj ("loop_break", -1));
	  break;

	case continue_control:
	  /* A loop_continue */
	  Tcl_ListObjAppendElement (NULL, obj,
				    Tcl_NewStringObj ("loop_continue", -1));
	  break;

	case while_control:
	  /* A while loop. Must append "end" to the end of it. */
	  tmp = Tcl_NewStringObj ("while ", -1);
	  Tcl_AppendToObj (tmp, cmd->line, -1);
	  Tcl_ListObjAppendElement (NULL, obj, tmp);
	  Tcl_ListObjAppendList (NULL, obj,
				 get_breakpoint_commands (*cmd->body_list));
	  Tcl_ListObjAppendElement (NULL, obj,
				    Tcl_NewStringObj ("end", -1));
	  break;

	case if_control:
	  /* An if statement. cmd->body_list[0] is the true part,
	     cmd->body_list[1] contains the "else" (false) part. */
	  tmp = Tcl_NewStringObj ("if ", -1);
	  Tcl_AppendToObj (tmp, cmd->line, -1);
	  Tcl_ListObjAppendElement (NULL, obj, tmp);
	  Tcl_ListObjAppendList (NULL, obj,
				 get_breakpoint_commands (cmd->body_list[0]));
	  if (cmd->body_count == 2)
	    {
	      Tcl_ListObjAppendElement (NULL, obj,
					Tcl_NewStringObj ("else", -1));
	      Tcl_ListObjAppendList (NULL, obj,
				     get_breakpoint_commands(cmd->body_list[1]));
	    }
	  Tcl_ListObjAppendElement (NULL, obj,
				    Tcl_NewStringObj ("end", -1));
	  break;

	case invalid_control:
	  /* Something invalid. Just skip it. */
	  break;
	}

      cmd = cmd->next;
    }

  return obj;
}

/* This implements the tcl command gdb_get_breakpoint_list
 * It builds up a list of the current breakpoints.
 *
 * Tcl Arguments:
 *    None.
 * Tcl Result:
 *    A list of breakpoint numbers.
 */
static int
gdb_get_breakpoint_list (ClientData clientData, Tcl_Interp *interp,
			 int objc, Tcl_Obj *CONST objv[])
{
  int i;
  Tcl_Obj *new_obj;

  if (objc != 1)
    {
      Tcl_WrongNumArgs (interp, 1, objv, NULL);
      return TCL_ERROR;
    }

  for (i = 0; i < breakpoint_list_size; i++)
    {
      if (breakpoint_list[i] != NULL
	  && breakpoint_list[i]->type == bp_breakpoint)
	{
	  new_obj = Tcl_NewIntObj (i);
	  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, new_obj);
	}
    }

  return TCL_OK;
}

/* This implements the tcl command "gdb_set_bp"
 * It sets breakpoints, and notifies the GUI.
 *
 * Tcl Arguments:
 *    addr:     the "address" for the breakpoint (either *ADDR or file:line)
 *    type:     the type of the breakpoint
 *    thread:   optional thread number
 * Tcl Result:
 *    The return value of the call to gdbtk_tcl_breakpoint.
 */
static int
gdb_set_bp (ClientData clientData, Tcl_Interp *interp,
	    int objc, Tcl_Obj *CONST objv[])
{
  int temp, ignore_count, thread, pending, enabled;
  char *address, *typestr, *condition;
  struct gdb_exception e;

  /* Insight does not use all of these (yet?).  */
  ignore_count = 0;
  condition = NULL;
  pending = 0;
  enabled = 1;

  if (objc != 3 && objc != 4)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "addr type ?thread?");
      return TCL_ERROR;
    }

  address = Tcl_GetStringFromObj (objv[1], NULL);
  if (address == NULL)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  typestr = Tcl_GetStringFromObj (objv[2], NULL);
  if (strncmp (typestr, "temp", 4) == 0)
    temp = 1;
  else if (strncmp (typestr, "normal", 6) == 0)
    temp = 0;
  else
    {
      gdbtk_set_result (interp, "type must be \"temp\" or \"normal\"");
      return TCL_ERROR;
    }

  if (objc == 4)
    {
      if (Tcl_GetIntFromObj (interp, objv[3], &thread) == TCL_ERROR)
	{
	  result_ptr->flags = GDBTK_IN_TCL_RESULT;
	  return TCL_ERROR;
	}
    }

  TRY_CATCH (e, RETURN_MASK_ALL)
    {
      set_breakpoint (get_current_arch (), address, condition,
		      0 /* hardwareflag */, temp, thread, ignore_count,
		      pending, enabled);
    }

  if (e.reason < 0)
    return TCL_ERROR;

  return TCL_OK;
}

/*
 * This section contains functions that deal with breakpoint
 * events from gdb.
 */

/* The next three functions use breakpoint_notify to allow the GUI 
 * to handle creating, deleting and modifying breakpoints.  These three
 * functions are put into the appropriate gdb hooks in gdbtk_init.
 */

void
gdbtk_create_breakpoint (int num)
{
  struct breakpoint *b;
  for (b = breakpoint_chain; b != NULL; b = b->next)
    {
      if (b->number == num)
	break;
    }

  if (b == NULL || !BREAKPOINT_IS_INTERESTING (b))
    return;

  /* Check if there is room to store it */
  if (num >= breakpoint_list_size)
    {
      int oldsize = breakpoint_list_size;
      while (num >= breakpoint_list_size)
	breakpoint_list_size += DEFAULT_LIST_SIZE;
      breakpoint_list = (struct breakpoint **) xrealloc (breakpoint_list, breakpoint_list_size * sizeof (struct breakpoint *));
      memset (&(breakpoint_list[oldsize]), 0, (breakpoint_list_size - oldsize) * sizeof (struct breakpoint *));
    }

  breakpoint_list[num] = b;
  breakpoint_notify (num, "create");
}

void
gdbtk_delete_breakpoint (int num)
{
  if (num >= 0
      && num <= breakpoint_list_size
      && breakpoint_list[num] != NULL)
    {
      breakpoint_notify (num, "delete");
      breakpoint_list[num] = NULL;
    }
}

void
gdbtk_modify_breakpoint (int num)
{
  if (num >= 0)
    breakpoint_notify (num, "modify");
}

/* This is the generic function for handling changes in
 * a breakpoint.  It routes the information to the Tcl
 * command "gdbtk_tcl_breakpoint" in the form:
 *   gdbtk_tcl_breakpoint action b_number b_address b_line b_file
 * On error, the error string is written to gdb_stdout.
 */
static void
breakpoint_notify (int num, const char *action)
{
  char *buf;

  if (num > breakpoint_list_size
      || num < 0
      || breakpoint_list[num] == NULL
      /* FIXME: should not be so restrictive... */
      || breakpoint_list[num]->type != bp_breakpoint)
    return;

  /* We ensure that ACTION contains no special Tcl characters, so we
     can do this.  */
  buf = xstrprintf ("gdbtk_tcl_breakpoint %s %d", action, num);

  if (Tcl_Eval (gdbtk_interp, buf) != TCL_OK)
    report_error ();
  free(buf); 
}

/*
 * This section contains the commands that deal with tracepoints:
 */

/* This implements the tcl command gdb_actions
 * It sets actions for a given tracepoint.
 *
 * Tcl Arguments:
 *    number: the tracepoint in question
 *    actions: the actions to add to this tracepoint
 * Tcl Result:
 *    None.
 */

static int
gdb_actions_command (ClientData clientData, Tcl_Interp *interp,
		     int objc, Tcl_Obj *CONST objv[])
{
  struct breakpoint *tp;
  Tcl_Obj **actions;
  int nactions, i, len;
  char *number, *args, *action;
  long step_count;
  struct action_line *next = NULL, *temp;
  enum actionline_type linetype;

  if (objc != 3)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "number actions");
      return TCL_ERROR;
    }

  args = number = Tcl_GetStringFromObj (objv[1], NULL);
  tp = get_tracepoint_by_number (&args, 0, 0);
  if (tp == NULL)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "Tracepoint \"",
			      number, "\" does not exist", NULL);
      return TCL_ERROR;
    }

  /* Free any existing actions */
  if (tp->actions != NULL)
    free_actions (tp);

  step_count = 0;

  Tcl_ListObjGetElements (interp, objv[2], &nactions, &actions);

  /* Add the actions to the tracepoint */
  for (i = 0; i < nactions; i++)
    {
      temp = xmalloc (sizeof (struct action_line));
      temp->next = NULL;
      action = Tcl_GetStringFromObj (actions[i], &len);
      temp->action = savestring (action, len);

      linetype = validate_actionline (&(temp->action), tp);

      if (linetype == BADLINE)
	{
	  free (temp);
	  continue;
	}

      if (next == NULL)
	{
	  tp->actions = temp;
	  next = temp;
	}
      else
	{
	  next->next = temp;
	  next = temp;
	}
    }

  return TCL_OK;
}

static int
gdb_get_trace_frame_num (ClientData clientData, Tcl_Interp *interp,
			 int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "linespec");
      return TCL_ERROR;
    }

  Tcl_SetIntObj (result_ptr->obj_ptr, get_traceframe_number ());
  return TCL_OK;

}

static int
gdb_get_tracepoint_info (ClientData clientData, Tcl_Interp *interp,
			 int objc, Tcl_Obj *CONST objv[])
{
  struct symtab_and_line sal;
  int tpnum;
  struct breakpoint *tp;
  struct action_line *al;
  Tcl_Obj *action_list;
  char *filename, *funcname;

  if (objc != 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "tpnum");
      return TCL_ERROR;
    }

  if (Tcl_GetIntFromObj (NULL, objv[1], &tpnum) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  tp = get_tracepoint (tpnum);

  if (tp == NULL)
    {
      gdbtk_set_result (interp, "Tracepoint #%d does not exist", tpnum);
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  sal = find_pc_line (tp->loc->address, 0);
  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "N/A";
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewStringObj (filename, -1));

  funcname = pc_function_name (tp->loc->address);
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewStringObj
			    (funcname, -1));

  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (sal.line));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewStringObj (core_addr_to_string (tp->loc->address), -1));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (tp->enable_state == bp_enabled));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (tp->pass_count));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (tp->step_count));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (tp->thread));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			    Tcl_NewIntObj (tp->hit_count));

  /* Append a list of actions */
  action_list = Tcl_NewObj ();
  for (al = tp->actions; al != NULL; al = al->next)
    {
      Tcl_ListObjAppendElement (interp, action_list,
				Tcl_NewStringObj (al->action, -1));
    }
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, action_list);

  return TCL_OK;
}

/* return a list of all tracepoint numbers in interpreter */
static int
gdb_get_tracepoint_list (ClientData clientData,
			 Tcl_Interp *interp,
			 int objc,
			 Tcl_Obj *CONST objv[])
{
  VEC(breakpoint_p) *tp_vec = NULL;
  int ix;
  struct breakpoint *tp;

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);

  tp_vec = all_tracepoints ();
  for (ix = 0; VEC_iterate (breakpoint_p, tp_vec, ix, tp); ix++)
    Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
			      Tcl_NewIntObj (tp->number));
  VEC_free (breakpoint_p, tp_vec);

  return TCL_OK;
}

static int
gdb_trace_status (ClientData clientData,
		  Tcl_Interp *interp,
		  int objc,
		  Tcl_Obj *CONST objv[])
{
  int result = 0;

  if (trace_running_p)
    result = 1;

  Tcl_SetIntObj (result_ptr->obj_ptr, result);
  return TCL_OK;
}

/* returns -1 if not found, tracepoint # if found */
static int
tracepoint_exists (char *args)
{
  VEC(breakpoint_p) *tp_vec = NULL;
  int ix;
  struct breakpoint *tp;
  char **canonical;
  struct symtabs_and_lines sals;
  char *file = NULL;
  int result = -1;

  sals = decode_line_1 (&args, 1, NULL, 0, &canonical, NULL);
  if (sals.nelts == 1)
    {
      resolve_sal_pc (&sals.sals[0]);
      file = xmalloc (strlen (sals.sals[0].symtab->dirname)
		      + strlen (sals.sals[0].symtab->filename) + 1);
      if (file != NULL)
	{
	  strcpy (file, sals.sals[0].symtab->dirname);
	  strcat (file, sals.sals[0].symtab->filename);

	  tp_vec = all_tracepoints ();
	  for (ix = 0; VEC_iterate (breakpoint_p, tp_vec, ix, tp); ix++)
	    {
	      if (tp->loc && tp->loc->address == sals.sals[0].pc)
		result = tp->number;
#if 0
	      /* Why is this here? This messes up assembly traces */
	      else if (tp->source_file != NULL
		       && strcmp (tp->source_file, file) == 0
		       && sals.sals[0].line == tp->line_number)
		result = tp->number;
#endif
	    }
	  VEC_free (breakpoint_p, tp_vec);
	}
    }
  if (file != NULL)
    free (file);
  return result;
}

static int
gdb_tracepoint_exists_command (ClientData clientData,
			       Tcl_Interp *interp,
			       int objc,
			       Tcl_Obj *CONST objv[])
{
  char *args;

  if (objc != 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv,
			"function:line|function|line|*addr");
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);

  Tcl_SetIntObj (result_ptr->obj_ptr, tracepoint_exists (args));
  return TCL_OK;
}

/*
 * This section contains functions which deal with tracepoint
 * events from gdb.
 */

void
gdbtk_create_tracepoint (int num)
{
  tracepoint_notify (num, "create");
}

void
gdbtk_delete_tracepoint (int num)
{
  tracepoint_notify (num, "delete");
}

void
gdbtk_modify_tracepoint (int num)
{
  tracepoint_notify (num, "modify");
}

static void
tracepoint_notify (int num, const char *action)
{
  char *buf;

  /* We ensure that ACTION contains no special Tcl characters, so we
     can do this.  */
  buf = xstrprintf ("gdbtk_tcl_tracepoint %s %d", action, num);

  if (Tcl_Eval (gdbtk_interp, buf) != TCL_OK)
    report_error ();
  free(buf); 
}
