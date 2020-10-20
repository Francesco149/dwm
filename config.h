/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 20;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const char *fonts[]          = {
	"scientifica:pixelsize=11",
	"emoji:pixelsize=11:antialias=true:autohint=true",
};
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { "#bbbbbb", "#222222", "#444444" },
	[SchemeSel]  = { "#eeeeee", "#222222", "#770000" },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"uxterm", "-class", "spterm", "-geometry", "120x34", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance    title       tags mask     flt term noswal mon */
	{ "brave-browser",  NULL,       NULL,       1 << 1,       0,  0,   0,     -1 },
	{ NULL,             "UXTerm",   NULL,       0,            0,  1,   0,     -1 },
	{ "spterm",   NULL,       NULL,       	    SPTAG(0),     1,  1,   0,     -1 },
};

/* layout(s) */
static const float mfact     = 0.65; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "uxterm", NULL };

/* password manager stuff */
#define SELPASS "gopass ls --flat | dmenu | xargs --no-run-if-empty gopass show -f"
#define TYPE(txt) "(t=\"" txt "\" && [ \"$t\" ] && xdotool type --clearmodifiers \"$t\")"
#define SELTYPE(filter) "pass=\"$(" SELPASS ")\" && " TYPE("$(echo \"$pass\" | " filter ")")
#define SELTYPEFLD(fld) SELTYPE("grep -i '^" fld ":' | awk -F: '{print $2}' | sed 's/^[ ]*//g'")
/* stupid gopass did a useless breaking change on the password format so now I have to supp both */
const Arg passcmd = SHCMD(SELTYPEFLD("password") " || " TYPE("$(echo \"$pass\" | head -n 1)"));
const Arg usercmd = SHCMD(SELTYPEFLD("username"));

#define UPLOAD \
  "notify-send \"Uploading $(basename \"$fil\") ...\" &&" \
  "json=$(mktemp) &&" \
  "(curl -F \"image=@$fil\" https://reeee.ee/upload > \"$json\") &&" \
  "jq .url < \"$json\" | sed 's/\"//g;s/$/.png/' | xclip -sel cli &&" \
  "(cat \"$json\" >> \"${UPLOADS_FILE:-$HOME/.cache/uploads}\") &&" \
  "rm -f \"$json\" &&" \
  "notify-send \"Uploaded $(basename \"$fil\")\""
const Arg uploadpiccmd = SHCMD(
  "fil=\"${SCREENSHOTS_DIR:-$HOME/screenshots}/section-$(date '+%y%m%d-%H%M-%S').png\" &&"
  "maim -s \"$fil\" &&"
  UPLOAD
);
const Arg uploadclipcmd = SHCMD(
  "fil=\"${SCREENSHOTS_DIR:-$HOME/screenshots}/clip-$(date '+%y%m%d-%H%M-%S').png\" &&"
  "xclip -selection clipboard -t image/png -o > \"$fil\" &&"
  UPLOAD
);
const Arg sscmd = SHCMD(
  "maim ${SCREENSHOTS_DIR:-$HOME/screenshots}/pic-full-$(date '+%y%m%d-%H%M-%S').png"
);

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_u,      spawn,          usercmd },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          passcmd },
	{ ControlMask|ShiftMask,        XK_4,      spawn,          uploadpiccmd},
	{ ControlMask|ShiftMask,        XK_5,      spawn,          uploadclipcmd},
	{ MODKEY|ShiftMask,             XK_Return, togglescratch,  {.ui = 0} },
	{ 0,                            XK_Print,  spawn,          sscmd },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

