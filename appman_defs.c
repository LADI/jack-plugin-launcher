CDBUS_SIGNAL_ARGS_BEGIN(applicationWasActivated, "")
  CDBUS_SIGNAL_ARG_DESCRIBE("id", "s", "")
  CDBUS_SIGNAL_ARG_DESCRIBE("aliasId", "s", "")
CDBUS_SIGNAL_ARGS_END

CDBUS_SIGNAL_ARGS_BEGIN(countChanged, "")
CDBUS_SIGNAL_ARGS_END

CDBUS_SIGNAL_ARGS_BEGIN(applicationAdded, "")
  CDBUS_SIGNAL_ARG_DESCRIBE("id", "s", "")
CDBUS_SIGNAL_ARGS_END

CDBUS_SIGNAL_ARGS_BEGIN(applicationAboutToBeRemoved, "")
  CDBUS_SIGNAL_ARG_DESCRIBE("id", "s", "")
CDBUS_SIGNAL_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(applicationIds, "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(get, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("id", "s", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(startApplicationOpenUrl, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("id", "s", "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("documentUrl", "s", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(startApplication, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("id", "s", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(stopApplication2, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("id", "s", "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("forceKill", "b", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(stopApplication, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("id", "s", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(stopAllApplications, "")
CDBUS_METHOD_ARGS_END

CDBUS_METHOD_ARGS_BEGIN(stopAllApplications2, "")
  CDBUS_METHOD_ARG_DESCRIBE_IN("forceKill", "b", "")
CDBUS_METHOD_ARGS_END

CDBUS_METHODS_BEGIN
CDBUS_METHOD_DESCRIBE(applicationIds, jpl_appman_cdbus_applicationIds_dbus)
CDBUS_METHOD_DESCRIBE(get, jpl_appman_cdbus_get_dbus)
CDBUS_METHOD_DESCRIBE(startApplicationOpenUrl, jpl_appman_cdbus_startApplicationOpenUrl_dbus)
CDBUS_METHOD_DESCRIBE(startApplication, jpl_appman_cdbus_startApplication_dbus)
CDBUS_METHOD_DESCRIBE(stopApplication2, jpl_appman_cdbus_stopApplication2_dbus)
CDBUS_METHOD_DESCRIBE(stopApplication, jpl_appman_cdbus_stopApplication_dbus)
CDBUS_METHOD_DESCRIBE(stopAllApplications, jpl_appman_cdbus_stopAllApplications_dbus)
CDBUS_METHOD_DESCRIBE(stopAllApplications2, jpl_appman_cdbus_stopAllApplications2_dbus)
CDBUS_METHODS_END

CDBUS_SIGNALS_BEGIN
CDBUS_SIGNAL_DESCRIBE(applicationWasActivated)
CDBUS_SIGNAL_DESCRIBE(countChanged)
CDBUS_SIGNAL_DESCRIBE(applicationAdded)
CDBUS_SIGNAL_DESCRIBE(applicationAboutToBeRemoved)
CDBUS_SIGNALS_END

#define JPL_APPMAN_CDBUS_IFACE_ORG_LADISH_APPLICATIONMANAGER0 "org.ladish.ApplicationManager0"

CDBUS_INTERFACE_DEFAULT_HANDLER_METHODS_AND_SIGNALS(jpl_appman_cdbus_interface_org_ladish_ApplicationManager0, JPL_APPMAN_CDBUS_IFACE_ORG_LADISH_APPLICATIONMANAGER0)
