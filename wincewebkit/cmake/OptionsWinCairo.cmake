INCLUDE(OptionsWindows)

ADD_DEFINITIONS(-DWTF_USE_WININET=1)
ADD_DEFINITIONS(-DJS_NO_EXPORT)
ADD_DEFINITIONS(-DENABLE_JSC_MULTIPLE_THREADS=0)
ADD_DEFINITIONS(-DHAVE_ACCESSIBILITY=0)
ADD_DEFINITIONS(-DNO_ALPHABLEND=1)
ADD_DEFINITIONS(-DUSE_SYSTEM_MALLOC=1)
ADD_DEFINITIONS(-DJSCCOLLECTOR_VIRTUALMEM_RESERVATION=0x200000)
ADD_DEFINITIONS(-D_UNICODE)
ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
ADD_DEFINITIONS(-DENABLE_QUERY_PERFORMANCE_COUNTER=0)

WEBKIT_FEATURE(ENABLE_AS_IMAGE "Enable SVG as image" DEFAULT ON SVG)
WEBKIT_FEATURE(ENABLE_BLOB "Enable blob slice" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_CHANNEL_MESSAGING "Enable channel messaging" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_DATABASE "Enable database" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_DATAGRID "Enable datagrid" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_DATALIST "Enable datalist" DEFAULT OFF HTML)
WEBKIT_FEATURE(ENABLE_DOM_STORAGE "Enable DOM storage" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_EVENTSOURCE "Enable event source" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_FAST_MOBILE_SCROLLING "Enable fast mobile scrolling" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_FILTERS "Enable SVG filters" DEFAULT OFF SVG)
WEBKIT_FEATURE(ENABLE_FTPDIR "Enable FTP directory support" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_GEOLOCATION "Enable geolocation" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_GLIB_SUPPORT "Enable Glib support" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_ICONDATABASE "Enable icon database" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_INSPECTOR "Enable inspector" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_JAVASCRIPT_DEBUGGER "Enable JavaScript debugger" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_JIT "Enable JIT code" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_MATHML "Enable MathML" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_NETSCAPE_PLUGIN_API "Enable Netscape plugin API" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_NOTIFICATIONS "Enable notifications" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_OFFLINE_WEB_APPLICATIONS "Enable offline web applications" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_ORIENTATION_EVENTS "Enable orientation events" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_PROGRESS_TAG "Enable progress tag" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_RUBY "Enable Ruby" DEFAULT OFF HTML)
WEBKIT_FEATURE(ENABLE_SANDBOX "Enable Sandbox" DEFAULT OFF HTML)
WEBKIT_FEATURE(ENABLE_SHARED_WORKERS "Enable shared workers" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_SVG "Enable SVG" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_SVG_ANIMATION "Enable SVG animation" DEFAULT OFF SVG)
WEBKIT_FEATURE(ENABLE_SVG_FONTS "Enable SVG fonts" DEFAULT ON SVG)
WEBKIT_FEATURE(ENABLE_SVG_FOREIGN_OBJECT "Enable SVG foreign object" DEFAULT OFF SVG)
WEBKIT_FEATURE(ENABLE_SVG_USE "Enable SVG use" DEFAULT ON SVG)
WEBKIT_FEATURE(ENABLE_VIDEO "Enable video" DEFAULT OFF HTML)
WEBKIT_FEATURE(ENABLE_WEB_SOCKETS "Enable web sockets" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_WML "Enable WML" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_WORKERS "Enable workers" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_XHTMLMP "Enable XHTMLMP" DEFAULT OFF)
WEBKIT_FEATURE(ENABLE_XPATH "Enable XPath" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_XSLT "Enable XSLT" DEFAULT ON)
WEBKIT_FEATURE(ENABLE_QUERY_PERFORMANCE_COUNTER "Enable performance timer" DEFAULT OFF)