/*----------------------------------------------------------------------------*-
                             ======================
                              YSI - Master Include
                             ======================
Description:
	Handles distribution of control of code segments across all running scripts
	with YSI to avoid conflicts of checkpoint streamers etc and allow features
	like "/commands" displaying all commands on the server.
	
	Now fully autonomous - to use the master system simply define which one you
	want and include the script:
	
	  #define MASTER 14
	  #include <YSI\Master>
	
	That will make the current script attempt to make itself the main master -
	you don't need ANY other code for initialisation anywhere (which took a
	while to figure out)...
	
	This is now over 2000 lines of repetitive and complex macros!
Legal:
	Copyright (C) 2007 Alex "Y_Less" Cole

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA 02110-1301, USA.
Version:
	0.2
Changelog:
	06/08/10:
		Managed the ultimate - got completely transparent inclusion!
	05/08/10:
		Completed new master system, now does all syncing and clients.
	06/10/09:
		Rewrote using states.
		Added remote function macros.
		Reduced the number of masters to a single one.  It must have them all.
		Vastly simplified the master model - ending the master ends everything.
	06/01/08:
		Added code to not pass data if no other script exists.
	17/11/07:
		Added code for a script to remove itself from the global list.
	11/10/07:
		Not first version but added documentation months late.
Functions:
	Public:
		-
	Core:
		-
	Stock:
		-
	Static:
		-
	Inline:
		-
	API:
		-
	Hooks:
		-
Callbacks:
	-
Definitions:
	-
Enums:
	-
Macros:
	RF - Declare a function which may be remote.
	RV - Declare a void function which may be remote.
	RS - Declare an optional callback.
	RFN - Declare an optional callback with no parameters.
	RFC - Declare a slower but shorter function.
	RFP - Declare a function with tags and give recall hints.
	RFPC - Combination of P and C.
	RFCP - Combination of P and C.
Tags:
	-
Variables:
	Global:
		_@ - ID of this script.
	Static:
		-
Commands:
	-
Compile options:
	YSIM_USE - Use the master system.
	YSIM_CLIENT - Act purely as a client.
	YSIM_T_ENABLE - Temporarily enable the master system.
	YSIM_T_DISABLE - Temporarily disable the master system.
	YSIM_C_ENABLE - Temporarily enable the client system.
	YSIM_C_DISABLE - Temporarily disable the client system.
	YSIM_S_ENABLE - Temporarily enable the server system.
	YSIM_S_DISABLE - Temporarily disable the server system.
Operators:
	-
Natives:
	-
-*----------------------------------------------------------------------------*/

// ALWAYS!  The compiler defines this automatically, don't use it as this is
// included multiple times to handle multiple master systems.
#undef _inc_y_master

#if !defined MASTER
	#tryinclude <YSI\internal\y_automasters>
	#if !defined MASTER
		#error "You must define a unique MASTER number from 0 to 25."
	#endif
#endif

#if MASTER > 25 || MASTER < 0
	// There are some secret ones reserved purely for YSI!
	#if !defined _YSIM_USED_AUTO || MASTER > 42
		#error "Invalid MASTER value, must be between 0 and 25 (inclusive)."
	#endif
#endif

#if defined YSIM_HAS_MASTER
	#undef YSIM_HAS_MASTER
#endif

#if defined YSIM_IS_CLIENT
	#undef YSIM_IS_CLIENT
#endif

#if defined YSIM_IS_SERVER
	#undef YSIM_IS_SERVER
#endif

#if defined YSIM_CLOUD
	#undef YSIM_CLOUD
#endif

//#if defined YSIM_NOT_CLIENT
//	#undef YSIM_NOT_CLIENT
//#endif

#if defined YSIM_OVERRIDE
	// Get stored settings for YSI libraries.  This allows the group system to
	// use the same master settings as another library without being there at
	// the time.
	#if YSIM_STORED_SETTINGS & 1
		#define YSIM_HAS_MASTER 1
	#else
		#define YSIM_HAS_MASTER 0
	#endif
	
	#if YSIM_STORED_SETTINGS & 2
		#define YSIM_IS_CLIENT 1
	#else
		#define YSIM_IS_CLIENT 0
	#endif
	
	#if YSIM_STORED_SETTINGS & 4
		#define YSIM_IS_SERVER 1
	#else
		#define YSIM_IS_SERVER 0
	#endif
	
	#if YSIM_STORED_SETTINGS & 8
		#define YSIM_CLOUD 1
	#else
		#define YSIM_CLOUD 0
	#endif
#else
	// Set up simple quick macros to query different current capabilities.
	#if (!defined YSI_NO_MASTER || defined YSIM_T_ENABLE) && !defined YSIM_T_DISABLE
		#define YSIM_HAS_MASTER 1
	#else
		#define YSIM_HAS_MASTER 0
	#endif
	
	#if (defined YSIM_CLIENT || defined YSIM_C_ENABLE) && !defined YSIM_C_DISABLE
		#define YSIM_IS_CLIENT 1
	#else
		#define YSIM_IS_CLIENT 0
	#endif
	
	#if (defined YSIM_SERVER || defined YSIM_S_ENABLE) && !defined YSIM_S_DISABLE
		#define YSIM_IS_SERVER 1
	#else
		#define YSIM_IS_SERVER 0
	#endif
	
	#if YSIM_HAS_MASTER && !YSIM_IS_CLIENT && !YSIM_IS_SERVER
		#define YSIM_CLOUD 1
	#else
		#define YSIM_CLOUD 0
	#endif
#endif

#if defined _inc_y_masteronce

// Been included at least once, clean up from before.
#undef RF
#undef RF@p
#undef RF@c
#undef RF@v
#undef RF@n
#undef RF@pc
#undef RF@pv
#undef RF@pcv
#undef RF@vc
#undef RF@vn

#undef RC
#undef RC@v
#undef RC@n
#undef RC@vn

#undef Master_Caller
//#undef Master_Init

#else

// These parts should only be defined once ever.  This code is not dependent on
// the current MASTER value, it's all generic.

#define _inc_y_masteronce

#include <YSI\y_utils>
#include <YSI\y_debug>

#define YSIM_MASTER #M

#define YSIM_RETURN #R

#define YSIM_CALLER #C

#define YSIM_STRING (42)

// Public variable.
public
	_@ = -1;

#define YSIM_NOT_CLIENT (!YSIM_HAS_MASTER || !YSIM_IS_CLIENT)

//#if !defined MAX_MASTERS
#define MAX_MASTERS 26
//#else
//	#if MAX_MASTERS > 32
//		#undef MAX_MASTERS
//		#define MAX_MASTERS 32
//	#endif
//#endif

forward Master_Reassert();

static
	YSI_g_sMasterCount,
	YSI_g_sMasterData[MAX_MASTERS];

native U@(id=0, const name[]="", value=cellmin, const string[]="") = setproperty;
native V@(id=0, const name[]="", value=cellmin, string[]="") = getproperty;
native W@(const function[], const format[], {Float,_}:...) = CallRemoteFunction;

stock X@(n)
{
	U@(8, YSIM_RETURN, n);
}

stock Y@()
{
	return U@(8, YSIM_CALLER, _@);
}

stock Z@()
{
	U@(8, YSIM_CALLER, -1);
	return V@(8, YSIM_RETURN);
}

stock T@()
{
	U@(8, YSIM_CALLER, -1);
}

/*----------------------------------------------------------------------------*-
Hook:
	OnGameModeInit
Notes:
	Constructor.  Gets the script a master ID.  Now ALWAYS gets an ID, even if
	the master system is disabled - doing otherwise is just too complicated.
-*----------------------------------------------------------------------------*/

#if defined FILTERSCRIPT
	public OnFilterScriptInit()
#else
	public OnGameModeInit()
#endif
{
	P:1("Master_OGM");
	if (!existproperty(8, YSIM_MASTER))
	{
		setproperty(8, YSIM_MASTER, 0);
	}
	// Properties get lost between script changes so we need to force a rebuild.
	CallRemoteFunction("Master_Reassert", "");
	new
		masters = getproperty(8, YSIM_MASTER),
		i = 0;
	while (i != 32)
	{
		if (!(masters & (1 << i)))
		{
			_@ = i;
			masters |= 1 << i;
			break;
		}
		++i;
	}
	if (i != 32)
	{
		setproperty(8, YSIM_MASTER, masters);
	}
	C:1(if (_@ == -1) printf("*** Internal Error! MasterID not assigned"););
	// Just use one name...
	CallLocalFunction("YSIM_OnScriptInit", "");
	return 1;
}

#if defined FILTERSCRIPT
	#if defined _ALS_OnFilterScriptInit
		#undef OnFilterScriptInit
	#else
		#define _ALS_OnFilterScriptInit
	#endif
	#define OnFilterScriptInit YSIM_OnScriptInit
#else
	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif
	#define OnGameModeInit YSIM_OnScriptInit
#endif
forward YSIM_OnScriptInit();

/*----------------------------------------------------------------------------*-
Hook:
	OnGameModeExit
Notes:
	Destructor.
-*----------------------------------------------------------------------------*/

#if defined FILTERSCRIPT
	public OnFilterScriptExit()
#else
	public OnGameModeExit()
#endif
{
	// Loop through everything this script is master for and call the remote
	// function for it.  EXCEPT for this script itself!
	new
		func[4];
	for (new i = 0; i != YSI_g_sMasterCount; ++i)
	{
		// This is slightly slower for ending and starting scripts, but uses far
		// less heap space, and these values are rarely used, so may as well
		// pack them (which is what has happened here).
		func[0] = YSI_g_sMasterData[i] & 0xFF;
		func[1] = (YSI_g_sMasterData[i] >> 8) & 0xFF;
		func[2] = YSI_g_sMasterData[i] >> 16;
		CallLocalFunction(func, "");
		// The properties currently clear instantly, but that may not always be
		// the case.
		setproperty(8, YSIM_MASTER, getproperty(8, YSIM_MASTER) & ~(1 << _@));
	}
	CallLocalFunction("YSIM_OnScriptExit", "");
	return 1;
}

#if defined FILTERSCRIPT
	#if defined _ALS_OnFilterScriptExit
		#undef OnFilterScriptExit
	#else
		#define _ALS_OnFilterScriptExit
	#endif
	#define OnFilterScriptExit YSIM_OnScriptExit
#else
	#if defined _ALS_OnGameModeExit
		#undef OnGameModeExit
	#else
		#define _ALS_OnGameModeExit
	#endif
	#define OnGameModeExit YSIM_OnScriptExit
#endif
forward YSIM_OnScriptExit();

/*----------------------------------------------------------------------------*-
Function:
	Master_Reassert
Params:
	-
Return:
	-
Notes:
	Rebuilds the collection of master data whenever a script is restarted.
-*----------------------------------------------------------------------------*/

public Master_Reassert()
{
	// Make sure that the caller parameter is always -1 by default.
	U@(8, YSIM_CALLER, -1);
	if (_@ != -1)
	{
		// Readd this script's master value.
		setproperty(8, YSIM_MASTER, getproperty(8, YSIM_MASTER) | (1 << _@));
		// Readd this script's owned scripts.
		new
			func[4];
		for (new i = 0; i != YSI_g_sMasterCount; ++i)
		{
			// This is slightly slower for ending and starting scripts, but uses far
			// less heap space, and these values are rarely used, so may as well
			// pack them (which is what has happened here).
			func[0] = YSI_g_sMasterData[i] & 0xFF;
			func[1] = (YSI_g_sMasterData[i] >> 8) & 0xFF;
			func[2] = YSI_g_sMasterData[i] >> 16;
			setproperty(8, func, _@);
		}
	}
}

/*----------------------------------------------------------------------------*-
Function:
	_Master_Get
Params:
	library[] - The name of the library to try become master for.
Return:
	-
Notes:
	-
-*----------------------------------------------------------------------------*/

stock bool:_Master_Get(library[])
{
	if (existproperty(8, library))
	{
		if (getproperty(8, library) != -1)
		{
			return false;
		}
	}
	setproperty(8, library, _@);
	// Add this library to the list.  The list is designed to only deal with
	// two or three character master names now!
	if (YSI_g_sMasterCount < MAX_MASTERS)
	{
		YSI_g_sMasterData[YSI_g_sMasterCount++] = library[0] | (library[1] << 8) | (library[2] << 16);
	}
	C:1(else printf("*** Internal Error! Too many master scripts"););
	return true;
}

// Define all the alternate spellings once.
#define RF@cp  RF@pc
#define RF@pvc RF@pcv
#define RF@cpv RF@pcv
#define RF@cvp RF@pcv
#define RF@vpc RF@pcv
#define RF@vcp RF@pcv
#define RF@vp  RF@pv
#define RF@cv  RF@vc
#define RF@nv  RF@vn

#define RF@cpt  RF@pct
#define RF@tcp  RF@pct
#define RF@tpc  RF@pct
#define RF@ctp  RF@pct
#define RF@ptc  RF@pct
#define RF@tp  RF@pt
#define RF@tc  RF@ct
#define RF@tn  RF@nt

/*
#define RF:%1[%2](%3)
#define RF@p:%1[%2](%3)<%4>
#define RF@c:%1[%2](%3)
#define RF@v:%1[%2](%3)
#define RF@n:%1[]()
#define RF@pc:%1[%2](%3)<%4>
#define RF@pv:%1[%2](%3)<%4>
#define RF@pcv:%1[%2](%3)<%4>
#define RF@vc:%1[%2](%3)
#define RF@vn:%1[]()
*/

// Define all the alternate spellings once.
#define RC@nv  RC@vn

/*
#define RC:%1[%2](%3)
#define RC@v:%1[%2](%3)
#define RC@n:%1[]()
#define RC@vn:%1[]()
*/

#endif

// Clear up some artifacts
#if defined YSIM_DEFINED
	#undef YSIM_DEFINED
#endif

#if defined _YCM
	#undef _YCM
#endif

// Now set the current script data.
#if MASTER == 0
	#if defined _YCM_a@
		#define YSIM_DEFINED
	#else
		#define _YCM_a@
	#endif
	#define _YCM a@
#endif
#if MASTER == 1
	#if defined _YCM_b@
		#define YSIM_DEFINED
	#else
		#define _YCM_b@
	#endif
	#define _YCM b@
#endif
#if MASTER == 2
	#if defined _YCM_c@
		#define YSIM_DEFINED
	#else
		#define _YCM_c@
	#endif
	#define _YCM c@
#endif
#if MASTER == 3
	#if defined _YCM_d@
		#define YSIM_DEFINED
	#else
		#define _YCM_d@
	#endif
	#define _YCM d@
#endif
#if MASTER == 4
	#if defined _YCM_e@
		#define YSIM_DEFINED
	#else
		#define _YCM_e@
	#endif
	#define _YCM e@
#endif
#if MASTER == 5
	#if defined _YCM_f@
		#define YSIM_DEFINED
	#else
		#define _YCM_f@
	#endif
	#define _YCM f@
#endif
#if MASTER == 6
	#if defined _YCM_g@
		#define YSIM_DEFINED
	#else
		#define _YCM_g@
	#endif
	#define _YCM g@
#endif
#if MASTER == 7
	#if defined _YCM_h@
		#define YSIM_DEFINED
	#else
		#define _YCM_h@
	#endif
	#define _YCM h@
#endif
#if MASTER == 8
	#if defined _YCM_i@
		#define YSIM_DEFINED
	#else
		#define _YCM_i@
	#endif
	#define _YCM i@
#endif
#if MASTER == 9
	#if defined _YCM_j@
		#define YSIM_DEFINED
	#else
		#define _YCM_j@
	#endif
	#define _YCM j@
#endif
#if MASTER == 10
	#if defined _YCM_k@
		#define YSIM_DEFINED
	#else
		#define _YCM_k@
	#endif
	#define _YCM k@
#endif
#if MASTER == 11
	#if defined _YCM_l@
		#define YSIM_DEFINED
	#else
		#define _YCM_l@
	#endif
	#define _YCM l@
#endif
#if MASTER == 12
	#if defined _YCM_m@
		#define YSIM_DEFINED
	#else
		#define _YCM_m@
	#endif
	#define _YCM m@
#endif
#if MASTER == 13
	#if defined _YCM_n@
		#define YSIM_DEFINED
	#else
		#define _YCM_n@
	#endif
	#define _YCM n@
#endif
#if MASTER == 14
	#if defined _YCM_o@
		#define YSIM_DEFINED
	#else
		#define _YCM_o@
	#endif
	#define _YCM o@
#endif
#if MASTER == 15
	#if defined _YCM_p@
		#define YSIM_DEFINED
	#else
		#define _YCM_p@
	#endif
	#define _YCM p@
#endif
#if MASTER == 16
	#if defined _YCM_q@
		#define YSIM_DEFINED
	#else
		#define _YCM_q@
	#endif
	#define _YCM q@
#endif
#if MASTER == 17
	#if defined _YCM_r@
		#define YSIM_DEFINED
	#else
		#define _YCM_r@
	#endif
	#define _YCM r@
#endif
#if MASTER == 18
	#if defined _YCM_s@
		#define YSIM_DEFINED
	#else
		#define _YCM_s@
	#endif
	#define _YCM s@
#endif
#if MASTER == 19
	#if defined _YCM_t@
		#define YSIM_DEFINED
	#else
		#define _YCM_t@
	#endif
	#define _YCM t@
#endif
#if MASTER == 20
	#if defined _YCM_u@
		#define YSIM_DEFINED
	#else
		#define _YCM_u@
	#endif
	#define _YCM u@
#endif
#if MASTER == 21
	#if defined _YCM_v@
		#define YSIM_DEFINED
	#else
		#define _YCM_v@
	#endif
	#define _YCM v@
#endif
#if MASTER == 22
	#if defined _YCM_w@
		#define YSIM_DEFINED
	#else
		#define _YCM_w@
	#endif
	#define _YCM w@
#endif
#if MASTER == 23
	#if defined _YCM_x@
		#define YSIM_DEFINED
	#else
		#define _YCM_x@
	#endif
	#define _YCM x@
#endif
#if MASTER == 24
	#if defined _YCM_y@
		#define YSIM_DEFINED
	#else
		#define _YCM_y@
	#endif
	#define _YCM y@
#endif
#if MASTER == 25
	#if defined _YCM_z@
		#define YSIM_DEFINED
	#else
		#define _YCM_z@
	#endif
	#define _YCM z@
#endif
#if MASTER == 26
	#if defined _YCM_A@
		#define YSIM_DEFINED
	#else
		#define _YCM_A@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 15
					#else
						#define YSIM_RECALL_26 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 11
					#else
						#define YSIM_RECALL_26 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 13
					#else
						#define YSIM_RECALL_26 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 9
					#else
						#define YSIM_RECALL_26 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 14
					#else
						#define YSIM_RECALL_26 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 10
					#else
						#define YSIM_RECALL_26 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 12
					#else
						#define YSIM_RECALL_26 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_26 8
					#else
						#define YSIM_RECALL_26 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM A@
#endif
#if MASTER == 27
	#if defined _YCM_B@
		#define YSIM_DEFINED
	#else
		#define _YCM_B@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 15
					#else
						#define YSIM_RECALL_27 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 11
					#else
						#define YSIM_RECALL_27 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 13
					#else
						#define YSIM_RECALL_27 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 9
					#else
						#define YSIM_RECALL_27 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 14
					#else
						#define YSIM_RECALL_27 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 10
					#else
						#define YSIM_RECALL_27 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 12
					#else
						#define YSIM_RECALL_27 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_27 8
					#else
						#define YSIM_RECALL_27 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM B@
#endif
#if MASTER == 28
	#if defined _YCM_C@
		#define YSIM_DEFINED
	#else
		#define _YCM_C@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 15
					#else
						#define YSIM_RECALL_28 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 11
					#else
						#define YSIM_RECALL_28 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 13
					#else
						#define YSIM_RECALL_28 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 9
					#else
						#define YSIM_RECALL_28 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 14
					#else
						#define YSIM_RECALL_28 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 10
					#else
						#define YSIM_RECALL_28 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 12
					#else
						#define YSIM_RECALL_28 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_28 8
					#else
						#define YSIM_RECALL_28 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM C@
#endif
#if MASTER == 29
	#if defined _YCM_D@
		#define YSIM_DEFINED
	#else
		#define _YCM_D@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 15
					#else
						#define YSIM_RECALL_29 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 11
					#else
						#define YSIM_RECALL_29 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 13
					#else
						#define YSIM_RECALL_29 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 9
					#else
						#define YSIM_RECALL_29 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 14
					#else
						#define YSIM_RECALL_29 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 10
					#else
						#define YSIM_RECALL_29 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 12
					#else
						#define YSIM_RECALL_29 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_29 8
					#else
						#define YSIM_RECALL_29 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM D@
#endif
#if MASTER == 30
	#if defined _YCM_E@
		#define YSIM_DEFINED
	#else
		#define _YCM_E@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 15
					#else
						#define YSIM_RECALL_30 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 11
					#else
						#define YSIM_RECALL_30 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 13
					#else
						#define YSIM_RECALL_30 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 9
					#else
						#define YSIM_RECALL_30 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 14
					#else
						#define YSIM_RECALL_30 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 10
					#else
						#define YSIM_RECALL_30 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 12
					#else
						#define YSIM_RECALL_30 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_30 8
					#else
						#define YSIM_RECALL_30 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM E@
#endif
#if MASTER == 31
	#if defined _YCM_F@
		#define YSIM_DEFINED
	#else
		#define _YCM_F@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 15
					#else
						#define YSIM_RECALL_31 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 11
					#else
						#define YSIM_RECALL_31 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 13
					#else
						#define YSIM_RECALL_31 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 9
					#else
						#define YSIM_RECALL_31 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 14
					#else
						#define YSIM_RECALL_31 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 10
					#else
						#define YSIM_RECALL_31 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 12
					#else
						#define YSIM_RECALL_31 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_31 8
					#else
						#define YSIM_RECALL_31 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM F@
#endif
#if MASTER == 32
	#if defined _YCM_G@
		#define YSIM_DEFINED
	#else
		#define _YCM_G@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 15
					#else
						#define YSIM_RECALL_32 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 11
					#else
						#define YSIM_RECALL_32 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 13
					#else
						#define YSIM_RECALL_32 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 9
					#else
						#define YSIM_RECALL_32 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 14
					#else
						#define YSIM_RECALL_32 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 10
					#else
						#define YSIM_RECALL_32 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 12
					#else
						#define YSIM_RECALL_32 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_32 8
					#else
						#define YSIM_RECALL_32 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM G@
#endif
#if MASTER == 33
	#if defined _YCM_H@
		#define YSIM_DEFINED
	#else
		#define _YCM_H@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 15
					#else
						#define YSIM_RECALL_33 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 11
					#else
						#define YSIM_RECALL_33 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 13
					#else
						#define YSIM_RECALL_33 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 9
					#else
						#define YSIM_RECALL_33 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 14
					#else
						#define YSIM_RECALL_33 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 10
					#else
						#define YSIM_RECALL_33 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 12
					#else
						#define YSIM_RECALL_33 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_33 8
					#else
						#define YSIM_RECALL_33 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM H@
#endif
#if MASTER == 34
	#if defined _YCM_I@
		#define YSIM_DEFINED
	#else
		#define _YCM_I@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 15
					#else
						#define YSIM_RECALL_34 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 11
					#else
						#define YSIM_RECALL_34 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 13
					#else
						#define YSIM_RECALL_34 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 9
					#else
						#define YSIM_RECALL_34 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 14
					#else
						#define YSIM_RECALL_34 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 10
					#else
						#define YSIM_RECALL_34 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 12
					#else
						#define YSIM_RECALL_34 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_34 8
					#else
						#define YSIM_RECALL_34 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM I@
#endif
#if MASTER == 35
	#if defined _YCM_J@
		#define YSIM_DEFINED
	#else
		#define _YCM_J@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 15
					#else
						#define YSIM_RECALL_35 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 11
					#else
						#define YSIM_RECALL_35 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 13
					#else
						#define YSIM_RECALL_35 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 9
					#else
						#define YSIM_RECALL_35 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 14
					#else
						#define YSIM_RECALL_35 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 10
					#else
						#define YSIM_RECALL_35 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 12
					#else
						#define YSIM_RECALL_35 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_35 8
					#else
						#define YSIM_RECALL_35 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM J@
#endif
#if MASTER == 36
	#if defined _YCM_K@
		#define YSIM_DEFINED
	#else
		#define _YCM_K@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 15
					#else
						#define YSIM_RECALL_36 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 11
					#else
						#define YSIM_RECALL_36 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 13
					#else
						#define YSIM_RECALL_36 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 9
					#else
						#define YSIM_RECALL_36 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 14
					#else
						#define YSIM_RECALL_36 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 10
					#else
						#define YSIM_RECALL_36 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 12
					#else
						#define YSIM_RECALL_36 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_36 8
					#else
						#define YSIM_RECALL_36 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM K@
#endif
#if MASTER == 37
	#if defined _YCM_L@
		#define YSIM_DEFINED
	#else
		#define _YCM_L@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 15
					#else
						#define YSIM_RECALL_37 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 11
					#else
						#define YSIM_RECALL_37 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 13
					#else
						#define YSIM_RECALL_37 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 9
					#else
						#define YSIM_RECALL_37 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 14
					#else
						#define YSIM_RECALL_37 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 10
					#else
						#define YSIM_RECALL_37 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 12
					#else
						#define YSIM_RECALL_37 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_37 8
					#else
						#define YSIM_RECALL_37 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM L@
#endif
#if MASTER == 38
	#if defined _YCM_M@
		#define YSIM_DEFINED
	#else
		#define _YCM_M@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 15
					#else
						#define YSIM_RECALL_38 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 11
					#else
						#define YSIM_RECALL_38 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 13
					#else
						#define YSIM_RECALL_38 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 9
					#else
						#define YSIM_RECALL_38 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 14
					#else
						#define YSIM_RECALL_38 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 10
					#else
						#define YSIM_RECALL_38 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 12
					#else
						#define YSIM_RECALL_38 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_38 8
					#else
						#define YSIM_RECALL_38 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM M@
#endif
#if MASTER == 39
	#if defined _YCM_N@
		#define YSIM_DEFINED
	#else
		#define _YCM_N@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 15
					#else
						#define YSIM_RECALL_39 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 11
					#else
						#define YSIM_RECALL_39 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 13
					#else
						#define YSIM_RECALL_39 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 9
					#else
						#define YSIM_RECALL_39 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 14
					#else
						#define YSIM_RECALL_39 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 10
					#else
						#define YSIM_RECALL_39 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 12
					#else
						#define YSIM_RECALL_39 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_39 8
					#else
						#define YSIM_RECALL_39 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM N@
#endif
#if MASTER == 40
	#if defined _YCM_O@
		#define YSIM_DEFINED
	#else
		#define _YCM_O@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 15
					#else
						#define YSIM_RECALL_40 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 11
					#else
						#define YSIM_RECALL_40 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 13
					#else
						#define YSIM_RECALL_40 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 9
					#else
						#define YSIM_RECALL_40 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 14
					#else
						#define YSIM_RECALL_40 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 10
					#else
						#define YSIM_RECALL_40 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 12
					#else
						#define YSIM_RECALL_40 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_40 8
					#else
						#define YSIM_RECALL_40 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM O@
#endif
#if MASTER == 41
	#if defined _YCM_P@
		#define YSIM_DEFINED
	#else
		#define _YCM_P@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 15
					#else
						#define YSIM_RECALL_41 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 11
					#else
						#define YSIM_RECALL_41 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 13
					#else
						#define YSIM_RECALL_41 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 9
					#else
						#define YSIM_RECALL_41 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 14
					#else
						#define YSIM_RECALL_41 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 10
					#else
						#define YSIM_RECALL_41 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 12
					#else
						#define YSIM_RECALL_41 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_41 8
					#else
						#define YSIM_RECALL_41 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM P@
#endif
#if MASTER == 42
	#if defined _YCM_Q@
		#define YSIM_DEFINED
	#else
		#define _YCM_Q@
		// Save the settings.
		#if YSIM_HAS_MASTER
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 15
					#else
						#define YSIM_RECALL_42 7
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 11
					#else
						#define YSIM_RECALL_42 3
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 13
					#else
						#define YSIM_RECALL_42 5
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 9
					#else
						#define YSIM_RECALL_42 1
					#endif
				#endif
			#endif
		#else
			#if YSIM_IS_CLIENT
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 14
					#else
						#define YSIM_RECALL_42 6
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 10
					#else
						#define YSIM_RECALL_42 2
					#endif
				#endif
			#else
				#if YSIM_IS_SERVER
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 12
					#else
						#define YSIM_RECALL_42 4
					#endif
				#else
					#if YSIM_CLOUD
						#define YSIM_RECALL_42 8
					#else
						#define YSIM_RECALL_42 0
					#endif
				#endif
			#endif
		#endif
	#endif
	#define _YCM Q@
#endif

// Check for non-unique masters here.
#if defined YSIM_DEFINED && !defined YSIM_OVERRIDE
	#error "Non-unique master used."
#endif

#if YSIM_CLOUD && !defined YSIM_DEFINED
	// FIRST declare the next callback in the chain.
	#if defined FILTERSCRIPT
		public OnFilterScriptInit()
	#else
		public OnGameModeInit()
	#endif
	{
		P:1(#_YCM "_OGM");
		// THEN rename the callbacks.
		#if defined _YCM_CALLBACK
			#undef _YCM_CALLBACK
		#endif
		#if MASTER == 0
			#define _YCM_CALLBACK a@OnScriptInit
		#endif
		#if MASTER == 1
			#define _YCM_CALLBACK b@OnScriptInit
		#endif
		#if MASTER == 2
			#define _YCM_CALLBACK c@OnScriptInit
		#endif
		#if MASTER == 3
			#define _YCM_CALLBACK d@OnScriptInit
		#endif
		#if MASTER == 4
			#define _YCM_CALLBACK e@OnScriptInit
		#endif
		#if MASTER == 5
			#define _YCM_CALLBACK f@OnScriptInit
		#endif
		#if MASTER == 6
			#define _YCM_CALLBACK g@OnScriptInit
		#endif
		#if MASTER == 7
			#define _YCM_CALLBACK h@OnScriptInit
		#endif
		#if MASTER == 8
			#define _YCM_CALLBACK i@OnScriptInit
		#endif
		#if MASTER == 9
			#define _YCM_CALLBACK j@OnScriptInit
		#endif
		#if MASTER == 10
			#define _YCM_CALLBACK k@OnScriptInit
		#endif
		#if MASTER == 11
			#define _YCM_CALLBACK l@OnScriptInit
		#endif
		#if MASTER == 12
			#define _YCM_CALLBACK m@OnScriptInit
		#endif
		#if MASTER == 13
			#define _YCM_CALLBACK n@OnScriptInit
		#endif
		#if MASTER == 14
			#define _YCM_CALLBACK o@OnScriptInit
		#endif
		#if MASTER == 15
			#define _YCM_CALLBACK p@OnScriptInit
		#endif
		#if MASTER == 16
			#define _YCM_CALLBACK q@OnScriptInit
		#endif
		#if MASTER == 17
			#define _YCM_CALLBACK r@OnScriptInit
		#endif
		#if MASTER == 18
			#define _YCM_CALLBACK s@OnScriptInit
		#endif
		#if MASTER == 19
			#define _YCM_CALLBACK t@OnScriptInit
		#endif
		#if MASTER == 20
			#define _YCM_CALLBACK u@OnScriptInit
		#endif
		#if MASTER == 21
			#define _YCM_CALLBACK v@OnScriptInit
		#endif
		#if MASTER == 22
			#define _YCM_CALLBACK w@OnScriptInit
		#endif
		#if MASTER == 23
			#define _YCM_CALLBACK x@OnScriptInit
		#endif
		#if MASTER == 24
			#define _YCM_CALLBACK y@OnScriptInit
		#endif
		#if MASTER == 25
			#define _YCM_CALLBACK z@OnScriptInit
		#endif
		#if MASTER == 26
			#define _YCM_CALLBACK A@OnScriptInit
		#endif
		#if MASTER == 27
			#define _YCM_CALLBACK B@OnScriptInit
		#endif
		#if MASTER == 28
			#define _YCM_CALLBACK C@OnScriptInit
		#endif
		#if MASTER == 29
			#define _YCM_CALLBACK D@OnScriptInit
		#endif
		#if MASTER == 30
			#define _YCM_CALLBACK E@OnScriptInit
		#endif
		#if MASTER == 31
			#define _YCM_CALLBACK F@OnScriptInit
		#endif
		#if MASTER == 32
			#define _YCM_CALLBACK G@OnScriptInit
		#endif
		#if MASTER == 33
			#define _YCM_CALLBACK H@OnScriptInit
		#endif
		#if MASTER == 34
			#define _YCM_CALLBACK I@OnScriptInit
		#endif
		#if MASTER == 35
			#define _YCM_CALLBACK J@OnScriptInit
		#endif
		#if MASTER == 36
			#define _YCM_CALLBACK K@OnScriptInit
		#endif
		#if MASTER == 37
			#define _YCM_CALLBACK L@OnScriptInit
		#endif
		#if MASTER == 38
			#define _YCM_CALLBACK M@OnScriptInit
		#endif
		#if MASTER == 39
			#define _YCM_CALLBACK N@OnScriptInit
		#endif
		#if MASTER == 40
			#define _YCM_CALLBACK O@OnScriptInit
		#endif
		#if MASTER == 41
			#define _YCM_CALLBACK P@OnScriptInit
		#endif
		#if MASTER == 42
			#define _YCM_CALLBACK Q@OnScriptInit
		#endif
		// Determine the next unique name,
		// See if this script should be master.
		if (_Master_Get(#_YCM))
		{
			P:2("Got master " #_YCM);
			state _YCM:y;
		}
		else
		{
			state _YCM:n;
		}
		// FINALLY call the new next one.
		CallLocalFunction(#_YCM_CALLBACK, "");
	}
	
	// Don't need extra checks here as we KNOW it is already defined.
	#if defined FILTERSCRIPT
		#undef OnFilterScriptInit
		#define OnFilterScriptInit _YCM_CALLBACK
	#else
		#undef OnGameModeInit
		#define OnGameModeInit _YCM_CALLBACK
	#endif
	forward _YCM_CALLBACK();
#endif

// Undefine this before the next call.
#undef MASTER

// There are the following remote function definitions:
// 
// RF - A basic function.
// RF@p - A function with special parameters which need passing hints.
// RF@c - A function with lots of paramteres that needs special code.
// RF@v - A function with no return value.
// RF@n - A function with no parameters.
// RF@t - A function with a tag result.
// RC - A callback function.
// RF@p - A callback with special parameters which need passing hints.
// RF@c - A callback with lots of paramteres that needs special code.
// RF@v - A callback with no return value.
// RF@n - A callback with no parameters.
// 
// Supported combinations:
// 
// pc, cp
// pcv, pvc, cpv, cvp, vpc, vcp
// pv, vp
// cv, vc
// vn, nv
// cpt ctp pct ptc tcp tpc
// ct tc
// nt tn
// pt tp
// 
// p and c are for special parameter sets, n is for no parameters, so those two
// groups can never be mixed together.  t is for tag result and v is for no
// return, so they can never be used together.
// 
// Note that the callback thing won't work with OnPlayerCommandText as it has a
// different default return to everything else.
// 

// These parts are redefined every time the master system is included.
#if YSIM_HAS_MASTER
	#if YSIM_IS_CLIENT
		// Defined the functions as remote calling stubs.  Then rename the
		// following code with a name unlikely to ever be used and define the
		// functions as stock so the compiler removes all the code quietly.
		#define RF:%1[%2](%3) stock %1(%3)return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);stock _@%1(%3)
		#define RF@p:%1[%2](%3)<%4> stock %1(%3)return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);stock _@%1(%3)
		#define RF@c RF
		#define RF@v:%1[%2](%3) stock %1(%3)U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1);stock _@%1(%3)
		#define RF@n:%1[]() stock %1()return U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);stock _@%1()
		#define RF@pc RF@p
		#define RF@pv:%1[%2](%3)<%4> stock %1(%3)U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1);stock _@%1(%3)
		#define RF@pcv RF@pv
		#define RF@vc RF@v
		#define RF@vn:%1[]() stock %1()U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1);stock _@%1()
		
		#define RF@t:%0:%1[%2](%3) stock %0:%1(%3)return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);stock %0:_@%1(%3)
		#define RF@pt:%0:%1[%2](%3)<%4> stock %0:%1(%3)return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);stock %0:_@%1(%3)
		#define RF@ct RF@t
		#define RF@nt:%0:%1[]() stock %0:%1()return U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);stock %0:_@%1()
		#define RF@pct RF@pt
		
		#define RC:%1[%2](%3) stock _@%1(%3)
		#define RC@v:%1[%2](%3) stock _@%1(%3)
		#define RC@n:%1[]() stock _@%1()
		#define RC@vn:%1[]() stock _@%1()
		
		#define Master_Caller() (_@)
	#else
		#if YSIM_IS_SERVER
			#define RF:%1[%2](%3) forward %1R(%3);public %1R(%3)U@(8,YSIM_RETURN,%1(%3));%1(%3)
			#define RF@p:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)U@(8,YSIM_RETURN,%1(%4));%1(%3)
			#define RF@c RF
			#define RF@v:%1[%2](%3) forward %1R(%3);public %1R(%3)
			#define RF@n:%1[]() forward %1R();public %1R()U@(8,YSIM_RETURN,%1());%1()
			#define RF@pc RF@p
			#define RF@pv:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)
			#define RF@pcv RF@pv
			#define RF@vc RF@v
			#define RF@vn:%1[]() forward %1R();public %1R()
			
			#define RF@t:%0:%1[%2](%3) forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)U@(8,YSIM_RETURN,_:%1(%3));%0:%1(%3)
			#define RF@pt:%0:%1[%2](%3)<%4> forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)U@(8,YSIM_RETURN,_:%1(%4));%0:%1(%3)
			#define RF@ct RF@t
			#define RF@nt:%0:%1[]() forward %0:%1();forward %0:%1R();public %0:%1R()U@(8,YSIM_RETURN,_:%1());%0:%1()
			#define RF@pct RF@pt
			
			#define RC:%1[%2](%3) public %1(%3)
			#define RC@v:%1[%2](%3) public %1(%3)
			#define RC@n:%1[]() public %1()
			#define RC@vn:%1[]() public %1()
			
			#define Master_Caller() \
				(V@(8, YSIM_CALLER))
			
			//#define Master_Get(%1) state (_Master_Get(#%1)) ? (%1:y) : (%1:n)
			//#define Master_Init(%1); _Master_Get(#%1);
		#else
			// This is a normal system with the possibility of being the master.
			// This is the very complicated one to write!
			#define RF:%1[%2](%3) forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>U@(8,YSIM_RETURN,%1(%3));%1(%3)<>return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);%1(%3)<_YCM:y>
			#define RF@p:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>U@(8,YSIM_RETURN,%1(%4));%1(%3)<>return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);%1(%3)<_YCM:y>
			#define RF@c:%1[%2](%3) forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>X@(%1(%3));%1(%3)<>return Y@(),W@(#%1R,#%2,%3),Z@();%1(%3)<_YCM:y>
			#define RF@v:%1[%2](%3) forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>%1(%3);%1(%3)<>U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1);%1(%3)<_YCM:y>
			#define RF@n:%1[]() forward %1R();public %1R()<>{}public %1R()<_YCM:y>U@(8,YSIM_RETURN,%1());%1()<>return U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1),V@(8,YSIM_RETURN);%1()<_YCM:y>
			#define RF@pc:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>X@(%1(%4));%1(%3)<>return Y@(),W@(#%1R,#%2,%4),Z@();%1(%3)<_YCM:y>
			#define RF@pv:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>%1(%4);%1(%3)<>U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1);%1(%3)<_YCM:y>
			#define RF@pcv:%1[%2](%3)<%4> forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>%1(%4);%1(%3)<>Y@(),W@(#%1R,#%2,%4),T@();%1(%3)<_YCM:y>
			#define RF@vc:%1[%2](%3) forward %1R(%3);public %1R(%3)<>{}public %1R(%3)<_YCM:y>%1(%3);%1(%3)<>Y@(),W@(#%1R,#%2,%3),T@();%1(%3)<_YCM:y>
			#define RF@vn:%1[]() forward %1R();public %1R()<>{}public %1R()<_YCM:y>%1();%1()<>U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1);%1()<_YCM:y>
			
			#define RF@t:%0:%1[%2](%3) forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)<>{}public %0:%1R(%3)<_YCM:y>U@(8,YSIM_RETURN,_:%1(%3));%0:%1(%3)<>return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%3),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);%0:%1(%3)<_YCM:y>
			//#define RF@pt:%0:%1[%2](%3)<%4> forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)<>{}public %0:%1R(%3)<_YCM:y>U@(8,YSIM_RETURN,_:%1(%4));%0:%1(%3)<>return U@(8,YSIM_CALLER,_@),W@(#%1R,#%2,%4),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);%0:%1(%3)<_YCM:y>
			#define RF@ct:%0:%1[%2](%3) forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)<>{}public %0:%1R(%3)<_YCM:y>X@(_:%1(%3));%0:%1(%3)<>return Y@(),W@(#%1R,#%2,%3),%0:Z@();%0:%1(%3)<_YCM:y>
			//#define RF@nt:%0:%1[]() forward %0:%1();forward %0:%1R();public %0:%1R()<>{}public %0:%1R()<_YCM:y>U@(8,YSIM_RETURN,_:%1());%0:%1()<>return U@(8,YSIM_CALLER,_@),W@(#%1R,""),U@(8,YSIM_CALLER,-1),%0:V@(8,YSIM_RETURN);%0:%1()<_YCM:y>
			#define RF@pct:%0:%1[%2](%3)<%4> forward %0:%1(%3);forward %0:%1R(%3);public %0:%1R(%3)<>{}public %0:%1R(%3)<_YCM:y>X@(_:%1(%4));%0:%1(%3)<>return Y@(),W@(#%1R,#%2,%4),%0:Z@();%0:%1(%3)<_YCM:y>
			
			#define RC:%1[%2](%3) public %1(%3)<>return 1;public %1(%3)<_YCM:y>
			#define RC@v:%1[%2](%3) public %1(%3)<>{}public %1(%3)<_YCM:y>
			#define RC@n:%1[]() public %1()<>return 1;public %1()<_YCM:y>
			#define RC@vn:%1[]() public %1()<>{}public %1()<_YCM:y>
			
			#define Master_Caller() \
				((V@(8, YSIM_CALLER) == -1) ? (_@) : (V@(8, YSIM_CALLER)))
			
			//#define Master_Get(%1) state (_Master_Get(#%1)) ? (%1:y) : (%1:n)
			//#define Master_Init(%1); if (_Master_Get(#%1)) state %1:y; else state %1:n;
		#endif
	#endif
#else
	// Master system isn't used.
	#define RF:%1[%2](%3) stock %1(%3)
	#define RF@p RF
	#define RF@c RF
	#define RF@v RF
	#define RF@n RF
	#define RF@pc RF
	#define RF@pv RF
	#define RF@pcv RF
	#define RF@vc RF
	#define RF@vn RF
	
	#define RF@t:%0:%1[%2](%3) stock %0:%1(%3)
	#define RF@pt RF@t
	#define RF@ct RF@t
	#define RF@nt RF@t
	#define RF@pct RF@t
	
	#define RC:%1[%2](%3) public %1(%3)
	#define RC@v RC
	#define RC@n RC
	#define RC@vn RC
	
	#define Master_Caller() (_@)
	
	//#define Master_Init(%1);
#endif

#if YSIM_CLOUD && !defined YSIM_DEFINED
	// Define a load of fake functions to cater for the compiler's need to
	// know all the possible states.  y means master, n means not master, m
	// means master system missing, p means previous (ending master), u
	// means sort of uninitialised (though technically initialised).  This
	// also doubles as a handy call in function for resetting masters when a
	// script ends.  Just to clarify - this function is NOT called _YCM,
	// that is a macro which is expanded every time this file is included.
	forward _YCM();
	public _YCM() <_YCM:y>
	{
		P:2(#_YCM ":y called");
		// Disable the current mastership.
		state _YCM:p;
		setproperty(8, #_YCM, -1);
		// Called locally to begin the hand off process.
		CallRemoteFunction(#_YCM, "i", _@);
	}
	
	public _YCM() <_YCM:n>
	{
		P:2(#_YCM ":n called");
		// This has been called remotely from a script which owns the
		// library masterhood (that's the only way this can ever be called).
		if (_Master_Get(#_YCM))
		{
			P:2("Got master " #_YCM);
			state _YCM:y;
		}
	}
	
	public _YCM() <_YCM:m, _YCM:p, _YCM:u>
	{
		P:2(#_YCM ":mpu called");
		// Do nothing at all (just define states).
	}
	
	public _YCM() <>
	{
		P:2(#_YCM ":<> called");
		if (_Master_Get(#_YCM))
		{
			P:2("Got master " #_YCM);
			state _YCM:y;
		}
		else
		{
			state _YCM:n;
		}
		// Do nothing at all (just define states).
	}
#endif

#if defined YSIM_T_ENABLE
	#undef YSIM_T_ENABLE
#endif

#if defined YSIM_T_DISABLE
	#undef YSIM_T_DISABLE
#endif

#if defined YSIM_C_ENABLE
	#undef YSIM_C_ENABLE
#endif

#if defined YSIM_C_DISABLE
	#undef YSIM_C_DISABLE
#endif

#if defined YSIM_S_ENABLE
	#undef YSIM_S_ENABLE
#endif

#if defined YSIM_S_DISABLE
	#undef YSIM_S_DISABLE
#endif

// Allow YSI scripts to use the extended master system
#if defined _YSIM_USED_AUTO
	#undef _YSIM_USED_AUTO
#endif

#if defined YSIM_OVERRIDE
	#undef YSIM_OVERRIDE
#endif

#if defined YSIM_STORED_SETTINGS
	#undef YSIM_STORED_SETTINGS
#endif
