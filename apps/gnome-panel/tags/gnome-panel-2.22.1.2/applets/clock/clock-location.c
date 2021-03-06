#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include <glib.h>
#include <gio/gio.h>

#ifdef HAVE_NETWORK_MANAGER
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <NetworkManager/NetworkManager.h>
#endif

#include "clock-location.h"
#include "clock-marshallers.h"
#include "set-timezone.h"
#include "system-timezone.h"

G_DEFINE_TYPE (ClockLocation, clock_location, G_TYPE_OBJECT)

typedef struct {
        gchar *name;

        SystemTimezone *systz;

        gchar *timezone;

        gchar *tzname;

        gfloat latitude;
        gfloat longitude;

        gchar *weather_code;
        WeatherInfo *weather_info;
        guint weather_timeout;

	TempUnit temperature_unit;
	SpeedUnit speed_unit;
} ClockLocationPrivate;

enum {
	WEATHER_UPDATED,
	SET_CURRENT,
	LAST_SIGNAL
};

static guint location_signals[LAST_SIGNAL] = { 0 };

static void clock_location_finalize (GObject *);
static void clock_location_set_tz (ClockLocation *this);
static void clock_location_unset_tz (ClockLocation *this);
static void setup_weather_updates (ClockLocation *loc);
static void add_to_network_monitor (ClockLocation *loc);
static void remove_from_network_monitor (ClockLocation *loc);

#define PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CLOCK_LOCATION_TYPE, ClockLocationPrivate))

ClockLocation *
clock_location_new (const gchar *name, const gchar *timezone,
		    gfloat latitude, gfloat longitude,
		    const gchar *code, WeatherPrefs *prefs)
{
        ClockLocation *this;
        ClockLocationPrivate *priv;

        this = g_object_new (CLOCK_LOCATION_TYPE, NULL);
        priv = PRIVATE (this);

        priv->name = g_strdup (name);
        priv->timezone = g_strdup (timezone);

        /* initialize priv->tzname */
        clock_location_set_tz (this);
        clock_location_unset_tz (this);

        priv->latitude = latitude;
        priv->longitude = longitude;

        priv->weather_code = g_strdup (code);

	if (prefs) {
		priv->temperature_unit = prefs->temperature_unit;
		priv->speed_unit = prefs->speed_unit;
	}

        setup_weather_updates (this);

        return this;
}

static ClockLocation *current_location = NULL;

static void
clock_location_class_init (ClockLocationClass *this_class)
{
        GObjectClass *g_obj_class = G_OBJECT_CLASS (this_class);

        g_obj_class->finalize = clock_location_finalize;

        location_signals[WEATHER_UPDATED] =
		g_signal_new ("weather-updated",
			      G_OBJECT_CLASS_TYPE (g_obj_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (ClockLocationClass, weather_updated),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__POINTER,
			      G_TYPE_NONE, 1, G_TYPE_POINTER);

	location_signals[SET_CURRENT] = 
		g_signal_new ("set-current",
			      G_OBJECT_CLASS_TYPE (g_obj_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (ClockLocationClass, set_current),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

        g_type_class_add_private (this_class, sizeof (ClockLocationPrivate));
}

static void
clock_location_init (ClockLocation *this)
{
        ClockLocationPrivate *priv = PRIVATE (this);

        priv->name = NULL;

        priv->systz = system_timezone_new ();

        priv->timezone = NULL;

        priv->tzname = NULL;

        priv->latitude = 0;
        priv->longitude = 0;

	priv->temperature_unit = TEMP_UNIT_CENTIGRADE;
	priv->speed_unit = SPEED_UNIT_MS;
}

static void
clock_location_finalize (GObject *g_obj)
{
        ClockLocationPrivate *priv = PRIVATE (g_obj);

	remove_from_network_monitor (CLOCK_LOCATION (g_obj));

        if (priv->name) {
                g_free (priv->name);
                priv->name = NULL;
        }

        if (priv->systz) {
                g_object_unref (priv->systz);
                priv->systz = NULL;
        }

        if (priv->timezone) {
                g_free (priv->timezone);
                priv->timezone = NULL;
        }

        if (priv->tzname) {
                g_free (priv->tzname);
                priv->tzname = NULL;
        }

        if (priv->weather_code) {
                g_free (priv->weather_code);
                priv->weather_code = NULL;
        }

        if (priv->weather_info) {
                weather_info_free (priv->weather_info);
                priv->weather_info = NULL;
        }

        if (priv->weather_timeout) {
                g_source_remove (priv->weather_timeout);
                priv->weather_timeout = 0;
        }

        G_OBJECT_CLASS (clock_location_parent_class)->finalize (g_obj);
}

const gchar *
clock_location_get_name (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        return priv->name;
}

void
clock_location_set_name (ClockLocation *loc, const gchar *name)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        if (priv->name) {
                g_free (priv->name);
                priv->name = NULL;
        }

        priv->name = g_strdup (name);
}

gchar *
clock_location_get_timezone (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        return priv->timezone;
}

void
clock_location_set_timezone (ClockLocation *loc, const gchar *timezone)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        if (priv->timezone) {
                g_free (priv->timezone);
                priv->timezone = NULL;
        }

        priv->timezone = g_strdup (timezone);
}

gchar *
clock_location_get_tzname (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        return priv->tzname;
}

void
clock_location_get_coords (ClockLocation *loc, gfloat *latitude,
                               gfloat *longitude)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        *latitude = priv->latitude;
        *longitude = priv->longitude;
}

void
clock_location_set_coords (ClockLocation *loc, gfloat latitude,
                               gfloat longitude)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

        priv->latitude = latitude;
        priv->longitude = longitude;
}

static void
clock_location_set_tzname (ClockLocation *this, const char *tzname)
{
        ClockLocationPrivate *priv = PRIVATE (this);

        if (priv->tzname) {
                if (strcmp (priv->tzname, tzname) == 0) {
                        return;
                }

                g_free (priv->tzname);
                priv->tzname = NULL;
        }

        if (tzname) {
                priv->tzname = g_strdup (tzname);
        } else {
                priv->tzname = NULL;
        }
}

static void
clock_location_set_tz (ClockLocation *this)
{
        ClockLocationPrivate *priv = PRIVATE (this);

        time_t now_t;
        struct tm now;

        if (priv->timezone == NULL) {
                return;
        }

        setenv ("TZ", priv->timezone, 1);
        tzset();

        now_t = time (NULL);
        localtime_r (&now_t, &now);

        if (daylight && now.tm_isdst) {
                clock_location_set_tzname (this, tzname[1]);
        } else {
                clock_location_set_tzname (this, tzname[0]);
        }
}

static void
clock_location_unset_tz (ClockLocation *this)
{
        ClockLocationPrivate *priv = PRIVATE (this);
        const char *env_timezone;

        if (priv->timezone == NULL) {
                return;
        }

        env_timezone = system_timezone_get_env (priv->systz);

        if (env_timezone) {
                setenv ("TZ", env_timezone, 1);
        } else {
                unsetenv ("TZ");
        }
        tzset();
}

void
clock_location_localtime (ClockLocation *loc, struct tm *tm)
{
        time_t now;

        clock_location_set_tz (loc);

        time (&now);
        localtime_r (&now, tm);

        clock_location_unset_tz (loc);
}

gboolean
clock_location_is_current_timezone (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);
	const char *zone;

	zone = system_timezone_get (priv->systz);

	if (zone)
		return strcmp (zone, priv->timezone) == 0;
	else
		return clock_location_get_offset (loc) == 0;
}

gboolean
clock_location_is_current (ClockLocation *loc)
{
	if (current_location == loc)
		return TRUE;
	else if (current_location != NULL)
		return FALSE;

	if (clock_location_is_current_timezone (loc)) {
		/* Note that some code in clock.c depends on the fact that
		 * calling this function can set the current location if
		 * there's none */
		current_location = loc;
		g_object_add_weak_pointer (G_OBJECT (current_location), 
					   (gpointer *)&current_location);
		g_signal_emit (current_location, location_signals[SET_CURRENT],
			       0, NULL);

		return TRUE;
	}

	return FALSE;
}


glong
clock_location_get_offset (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);
        glong sys_timezone, local_timezone;
	glong offset;
	time_t t;
	struct tm *tm;

	t = time (NULL);
	
        unsetenv ("TZ");
        tm = localtime (&t);
        sys_timezone = timezone;

	if (tm->tm_isdst > 0) {
		sys_timezone -= 3600;
	}

        setenv ("TZ", priv->timezone, 1);
        tm = localtime (&t);
	local_timezone = timezone;

	if (tm->tm_isdst > 0) {
		local_timezone -= 3600;
	}

        offset = local_timezone - sys_timezone;

        clock_location_unset_tz (loc);

        return offset;
}

typedef struct {
	ClockLocation *location;
	GFunc callback;
	gpointer data;
	GDestroyNotify destroy;
} MakeCurrentData;

static void
make_current_cb (gpointer data, GError *error)
{
	MakeCurrentData *mcdata = data;

	if (error == NULL) {
		if (current_location)
			g_object_remove_weak_pointer (G_OBJECT (current_location), 
						      (gpointer *)&current_location);
		current_location = mcdata->location;
		g_object_add_weak_pointer (G_OBJECT (current_location), 
					   (gpointer *)&current_location);
		g_signal_emit (current_location, location_signals[SET_CURRENT],
			       0, NULL);
	}

	if (mcdata->callback)
		mcdata->callback (mcdata->data, error);
	else
		g_error_free (error);
}

static void
free_make_current_data (gpointer data)
{
	MakeCurrentData *mcdata = data;
	
	if (mcdata->destroy)
		mcdata->destroy (mcdata->data);
	
	g_object_unref (mcdata->location);
	g_free (mcdata);
}

void
clock_location_make_current (ClockLocation *loc, 
                             GFunc          callback,
                             gpointer       data,
                             GDestroyNotify destroy)
{
        ClockLocationPrivate *priv = PRIVATE (loc);
        gchar *filename;
	MakeCurrentData *mcdata;

	if (clock_location_is_current_timezone (loc)) {
		if (current_location)
			g_object_remove_weak_pointer (G_OBJECT (current_location), 
						      (gpointer *)&current_location);
		current_location = loc;
		g_object_add_weak_pointer (G_OBJECT (current_location), 
					   (gpointer *)&current_location);
		g_signal_emit (current_location, location_signals[SET_CURRENT],
			       0, NULL);
		if (callback)
               		callback (data, NULL);
		if (destroy)
			destroy (data);	
		return;
	}

	mcdata = g_new (MakeCurrentData, 1);

	mcdata->location = g_object_ref (loc);
	mcdata->callback = callback;
	mcdata->data = data;
	mcdata->destroy = destroy;

        filename = g_build_filename (SYSTEM_ZONEINFODIR, priv->timezone, NULL);
        set_system_timezone_async (filename, 
                                   (GFunc)make_current_cb, 
				   mcdata,
                                   free_make_current_data);
        g_free (filename);
}

const gchar *
clock_location_get_weather_code (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

	return priv->weather_code;
}

void
clock_location_set_weather_code (ClockLocation *loc, const gchar *code)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

	g_free (priv->weather_code);
	priv->weather_code = g_strdup (code);

	setup_weather_updates (loc);
}

WeatherInfo *
clock_location_get_weather_info (ClockLocation *loc)
{
        ClockLocationPrivate *priv = PRIVATE (loc);

	return priv->weather_info;
}

static void
weather_info_updated (WeatherInfo *info, gpointer data)
{
	ClockLocation *loc = data;
	ClockLocationPrivate *priv = PRIVATE (loc);

	g_signal_emit (loc, location_signals[WEATHER_UPDATED],
		       0, priv->weather_info);
}

static gboolean
update_weather_info (gpointer data)
{
	ClockLocation *loc = data;
	ClockLocationPrivate *priv = PRIVATE (loc);
	WeatherPrefs prefs = {
		FORECAST_STATE,
		FALSE,
		NULL,
		TEMP_UNIT_CENTIGRADE,
		SPEED_UNIT_MS,
		PRESSURE_UNIT_MB,
		DISTANCE_UNIT_KM
	};

	prefs.temperature_unit = priv->temperature_unit;
	prefs.speed_unit = priv->speed_unit;

	weather_info_abort (priv->weather_info);
        weather_info_update (priv->weather_info,
                             &prefs, weather_info_updated, loc);

	return TRUE;
}

static gchar *
rad2dms (gfloat lat, gfloat lon)
{
	gchar h, h2;
	gfloat d, deg, min, d2, deg2, min2;

	h = lat > 0 ? 'N' : 'S';
	d = fabs (lat);
	deg = floor (d);
	min = floor (60 * (d - deg));
	h2 = lon > 0 ? 'E' : 'W';
	d2 = fabs (lon);
	deg2 = floor (d2);
	min2 = floor (60 * (d2 - deg2));
	return g_strdup_printf ("%02d-%02d%c %02d-%02d%c",
				(int)deg, (int)min, h,
				(int)deg2, (int)min2, h2);
}

static GList *locations = NULL;

static void
update_weather_infos (void)
{
	GList *l;

	for (l = locations; l; l = l->next) {
		update_weather_info (l->data);
	}
}

#ifdef HAVE_NETWORK_MANAGER
static void
state_notify (DBusPendingCall *pending, gpointer data)
{
	DBusMessage *msg = dbus_pending_call_steal_reply (pending);

	if (!msg)
		return;

	if (dbus_message_get_type (msg) == DBUS_MESSAGE_TYPE_METHOD_RETURN) {
		dbus_uint32_t result;

		if (dbus_message_get_args (msg, NULL, 
					   DBUS_TYPE_UINT32, &result,
					   DBUS_TYPE_INVALID)) {
			if (result == NM_STATE_CONNECTED) {
				update_weather_infos ();
			}
		}
	}

	dbus_message_unref (msg);
}

static void 
check_network (DBusConnection *connection)
{
	DBusMessage *message;
	DBusPendingCall *reply;

	message = dbus_message_new_method_call (NM_DBUS_SERVICE,
						NM_DBUS_PATH,
						NM_DBUS_INTERFACE,
						"state");
	if (dbus_connection_send_with_reply (connection, message, &reply, -1)) {
		dbus_pending_call_set_notify (reply, state_notify, NULL, NULL);
		dbus_pending_call_unref (reply);
	}
	
	dbus_message_unref (message);
}

static DBusHandlerResult
filter_func (DBusConnection *connection,
             DBusMessage    *message,
             void           *user_data)
{
	if (dbus_message_is_signal (message,
				    NM_DBUS_INTERFACE, 
				    "StateChanged")) {
		check_network (connection);

		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static void
setup_network_monitor (void)
{
        GError *error;
	DBusError derror;
        static DBusGConnection *bus = NULL;
	DBusConnection *dbus;

        if (bus == NULL) {
                error = NULL;
                bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);
                if (bus == NULL) {
                        g_warning ("Couldn't connect to system bus: %s",
                                   error->message);
                        g_error_free (error);

			return;
                }

		dbus_error_init (&derror);
		dbus = dbus_g_connection_get_connection (bus);
                dbus_connection_add_filter (dbus, filter_func, NULL, NULL);
                dbus_bus_add_match (dbus,
                                    "type='signal',"
				    "interface='" NM_DBUS_INTERFACE "'",
                                    &derror);
		if (dbus_error_is_set (&derror)) {
			g_warning ("Couldn't register signal handler: %s: %s",
				   derror.name, derror.message);
			dbus_error_free (&derror);
		}
        }
}
#endif

static void
add_to_network_monitor (ClockLocation *loc)
{
#ifdef HAVE_NETWORK_MANAGER
	setup_network_monitor ();
#endif

	if (!g_list_find (locations, loc))
		locations = g_list_prepend (locations, loc);
}

static void
remove_from_network_monitor (ClockLocation *loc)
{
	locations = g_list_remove (locations, loc);
}

static void
setup_weather_updates (ClockLocation *loc)
{
	ClockLocationPrivate *priv = PRIVATE (loc);
	WeatherLocation *wl;
	WeatherPrefs prefs = {
		FORECAST_STATE,
		FALSE,
		NULL,
		TEMP_UNIT_CENTIGRADE,
		SPEED_UNIT_MS,
		PRESSURE_UNIT_MB,
		DISTANCE_UNIT_KM
	};

	gchar *dms;

	prefs.temperature_unit = priv->temperature_unit;
	prefs.speed_unit = priv->speed_unit;

        if (priv->weather_info) {
                weather_info_free (priv->weather_info);
                priv->weather_info = NULL;
        }

	if (priv->weather_timeout) {
		g_source_remove (priv->weather_timeout);
		priv->weather_timeout = 0;
	}

	if (!priv->weather_code || strcmp (priv->weather_code, "-") == 0)
		return;

	dms = rad2dms (priv->latitude, priv->longitude);
	wl = weather_location_new (priv->name, priv->weather_code,
				   NULL, NULL, dms, NULL, NULL);

	priv->weather_info =
		weather_info_new (wl, &prefs, weather_info_updated, loc);

	priv->weather_timeout =
		g_timeout_add_seconds (1800, update_weather_info, loc);

	weather_location_free (wl);
	g_free (dms);

	add_to_network_monitor (loc);
}

void
clock_location_set_weather_prefs (ClockLocation *loc,
				  WeatherPrefs *prefs)
{
	ClockLocationPrivate *priv = PRIVATE (loc);

	priv->temperature_unit = prefs->temperature_unit;
	priv->speed_unit = prefs->speed_unit;

	update_weather_info (loc);
}

