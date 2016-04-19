/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <watch_app.h>
#include <system_settings.h>
#include <badge.h>
#include "analogwatch.h"
#include "view.h"

#define APP_ID_CALL "com.samsung.call"
#define APP_ID_MESSAGES "com.samsung.message"

static void _badge_change_cb(unsigned int action, const char *app_id, unsigned int count, void *user_data);
static void _icon_pressed_cb(view_icon_id_t id);
static void _app_launch_request_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
static bool _get_time(watch_time_h watch_time, current_time_t *current_time);

/*
 * @brief The system language changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void lang_changed(app_event_info_h event_info, void* user_data)
{
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	if (locale == NULL)
		return;

	elm_language_set(locale);
	free(locale);

	return;
}

/*
 * @brief The region format changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void region_changed(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when region setting is changed
	 */
}

/*
 * @brief The low battery event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_battery(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low battery
	 */
	watch_app_exit();
}

/*
 * @brief The low memory event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_memory(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low memory
	 */
	watch_app_exit();
}

/*
 * @brief The device orientation changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void device_orientation(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when device orientation is changed
	 */
}

/*
 * @brief Called when the application starts.
 * @param[in] width The width of the window of idle screen that will show the watch UI
 * @param[in] height The height of the window of idle screen that will show the watch UI
 * @param[in] user_data The user data passed from the callback registration function
 */
static bool app_create(int width, int height, void* user_data)
{
	/*
	 * Hook to take necessary actions before main event loop starts
	 * Initialize UI resources and application's data
	 */

	app_event_handler_h handlers[5] = {NULL, };

	/*
	 * Register callbacks for each system event
	 */
	if (watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, lang_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, region_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, low_battery, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, low_memory, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, device_orientation, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	view_create_with_size(width, height);

	if (badge_register_changed_cb(_badge_change_cb, NULL) != BADGE_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "badge_register_changed_cb () is failed");

	view_set_icon_pressed_cb(_icon_pressed_cb);

	return true;
}

/*
 * @brief: This callback function is called when another application
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *user_data)
{
	/*
	 * Handle the launch request.
	 */
}

/*
 * @brief: This callback function is called each time
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *user_data)
{
	/*
	 * Take necessary actions when application becomes invisible.
	 */
}

/*
 * @brief: This callback function is called each time
 * the application becomes visible to the user
 */
static void app_resume(void *user_data)
{
	/*
	 * Take necessary actions when application becomes visible.
	 */
}

/*
 * @brief: This callback function is called once after the main loop of the application exits
 */
static void app_terminate(void *user_data)
{
	badge_unregister_changed_cb(_badge_change_cb);

	view_destroy();
}

/*
 * @brief Called at each second. This callback is not called while the app is paused or the device is in ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_time_tick(watch_time_h watch_time, void* user_data)
{
	current_time_t current_time = {0,};

	if (_get_time(watch_time, &current_time))
		view_set_display_time(current_time);
}

/*
 * @brief Called at each minute when the device in the ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
void app_ambient_tick(watch_time_h watch_time, void* user_data)
{
	app_time_tick(watch_time, user_data);
}

/*
 * @brief: This function will be called when the ambient mode is changed
 */
void app_ambient_changed(bool ambient_mode, void* user_data)
{
	/*
	 * Take necessary actions when application goes to/from ambient state
	 */

	view_toggle_ambient_mode(ambient_mode);
}

/*
 * @brief: Main function of the application
 */
int main(int argc, char *argv[])
{
	int ret;

	watch_app_lifecycle_callback_s event_callback = {0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	ret = watch_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);

	return ret;
}

/*
 * @brief: The callback function invoked on badge counter change notification.
 * @param[action]: type of the change affecting the badge.
 * @param[app_id]: the app_id of the affected application.
 * @param[count]: the badge counter.
 * @param[user_data]: the user data passed to the badge_register_changed_cb() function.
 */
static void _badge_change_cb(unsigned int action, const char *app_id, unsigned int count, void *user_data)
{
	if (!app_id)
		return;

	if (strncmp(app_id, APP_ID_CALL, strlen(app_id)) == 0)
		view_set_bagde_missed_calls(count);
	else if (strncmp(app_id, APP_ID_MESSAGES, strlen(app_id)) == 0)
		view_set_bagde_unread_messages(count);
}

/*
 * @brief The callback function invoked on application's icon tap.
 * @param[id]: the identifier of the tapped application.
 */
static void _icon_pressed_cb(view_icon_id_t id)
{
	app_control_h app_ctrl = NULL;
	const char *app_id = NULL;

	if (app_control_create(&app_ctrl) != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_control_create() is failed.");
		return;
	}

	switch (id) {
	case VIEW_ICON_ID_MISSED_CALLS:
		app_id = APP_ID_CALL;
		break;
	case VIEW_ICON_ID_UNREAD_MESSAGES:
		app_id = APP_ID_MESSAGES;
		break;
	default:
		dlog_print(DLOG_WARN, LOG_TAG, "Unknown id of the tapped application's icon.");
		app_control_destroy(app_ctrl);
		return;
	}

	if (app_control_set_app_id(app_ctrl, app_id) != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_control_set_app_id() is failed.");
		app_control_destroy(app_ctrl);
		return;
	}

	if (app_control_send_launch_request(app_ctrl, _app_launch_request_cb, (void *)app_id) != APP_CONTROL_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "app_control_send_launch_request() is failed.");

	app_control_destroy(app_ctrl);
}

/*
 * @brief The callback function invoked in response to app_control_send_launch_request().
 * @param[request]: the handle of app_control request.
 * @param[reply]: the handle of app_control response.
 * @param[result]: the result of app_control_send_launch_request() invocation.
 * @param[data]: the user data passed to the app_control_send_launch_request() function.
 */
static void _app_launch_request_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	const char *app_id = (const char *)data;

	switch (result) {
	case APP_CONTROL_RESULT_SUCCEEDED:
		dlog_print(DLOG_INFO, LOG_TAG, "The '%s' launch request succeeded.", app_id);
		break;
	case APP_CONTROL_RESULT_CANCELED:
		dlog_print(DLOG_WARN, LOG_TAG, "The '%s' launch request was canceled.", app_id);
		break;
	case APP_CONTROL_RESULT_FAILED:
		dlog_print(DLOG_WARN, LOG_TAG, "The '%s' launch request failed.", app_id);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "The '%s' launch request error.", app_id);
		break;
	}
}

/*
 * @brief: Obtains the current time from watch_time_h and converts to the current_time_t.
 * @param[watch_time]: The date and time structure acquired in time_tick callback function.
 * @param[current_time]: The structure of time components extracted from watch_time argument.
 * @return: The function returns 'true' if the time is successfully converted, otherwise 'false' is returned.
 */
static bool _get_time(watch_time_h watch_time, current_time_t *current_time)
{
	int ret;

	if (!current_time) {
		dlog_print(DLOG_ERROR, LOG_TAG, "current_time is NULL");
		return false;
	}

	memset(current_time, 0, sizeof(current_time_t));

	if (!watch_time) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_time is NULL");
		return false;
	}

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d", ret);
		return false;
	}

	watch_time_get_hour24(watch_time, &current_time->hour);
	watch_time_get_minute(watch_time, &current_time->minute);
	watch_time_get_second(watch_time, &current_time->second);

	return true;
}
