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

#include <watch_app_efl.h>
#include "analogwatch.h"
#include "view.h"
#include "view_defines.h"

#define MAIN_EDJ "edje/main.edj"

static struct view_info {
	Evas_Object *win;
	Evas_Object *layout;
	int w;
	int h;
	icon_pressed_cb icon_pressed_cb;
} s_info = {
	.win = NULL,
	.layout = NULL,
	.w = 0,
	.h = 0,
};

static char *_create_resource_path(const char *file_name);
static Evas_Object *_create_layout(void);
static void _emit_signal(Evas_Object *layout, const char *target_part, const char *signal_name);
static void _set_badge(int message_id, int badge_count);
static void _missed_calls_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _missed_calls_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _unread_messages_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _unread_messages_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);

/*
 * @brief Creates the application's UI with window's width and height preset.
 */
void view_create_with_size(int width, int height)
{
	s_info.w = width;
	s_info.h = height;

	view_create();
}

/*
 * @brief Create Essential Object window and layout
 */
void view_create(void)
{
	s_info.win = view_create_win(PACKAGE);
	if (!s_info.win) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return;
	}

	s_info.layout = _create_layout();
	if (!s_info.layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create main layout.");
		return;
	}

	evas_object_show(s_info.win);
}

/*
 * @brief Makes a basic window named with package name.
 * @param[pkg_name]: Name of the window.
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;
	int ret;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	ret = watch_app_get_elm_win(&win);
	if (ret != APP_ERROR_NONE)
		return NULL;

	elm_win_title_set(win, pkg_name);
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_FALSE);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_BG_TRANSPARENT);
	elm_win_prop_focus_skip_set(win, EINA_TRUE);
	elm_win_role_set(win, "no-effect");

	evas_object_resize(win, s_info.w, s_info.h);

	return win;
}

/*
 * @brief Makes and sets a layout to the part
 * @param[parent]: Object to which you want to set this layout
 * @param[file_path]: File path of EDJ will be used
 * @param[group_name]: Group name in EDJ that you want to set to layout
 * @param[part_name]: Part name to which you want to set this layout
 */
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	elm_object_part_content_set(parent, part_name, layout);

	return layout;
}

/*
 * @brief Draws the clock's hands.
 * @param[current_time]: the structure of time components.
 */
void view_set_display_time(current_time_t current_time)
{
	Edje_Message_Int_Set *msg = malloc(sizeof(Edje_Message_Int_Set) + 2 * sizeof(int));

	msg->count = 3;
	msg->val[0] = current_time.hour;
	msg->val[1] = current_time.minute;
	msg->val[2] = current_time.second;

	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT_SET, MSG_ID_SET_TIME, msg);

	free(msg);
}

/*
 * @brief Toggles the ambient mode on (draws a second hand) and off (hides a second hand).
 * @param[current_time]: the structure of time components.
 */
void view_toggle_ambient_mode(bool ambient_mode)
{
	Edje_Message_Int msg = {0,};

	msg.val = (int)ambient_mode;

	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT, MSG_ID_AMBIENT_MODE, &msg);
}

/*
 * @brief Sets the badge counter for 'missed calls' icon.
 * @param[count]: the number of missed calls to be displayed.
 */
void view_set_bagde_missed_calls(int count)
{
	_set_badge(MSG_ID_SET_BADGE_MISSED_CALLS, count);
}

/*
 * @brief Sets the badge counter for 'unread messages' icon.
 * @param[count]: the number of unread messages to be displayed.
 */
void view_set_bagde_unread_messages(int count)
{
	_set_badge(MSG_ID_SET_BADGE_UNREAD_MESSAGES, count);
}

/*
 * @brief Sets the callback function which will be invoked on application's icon tap.
 * @param[cb]: The callback function to be attached.
 */
void view_set_icon_pressed_cb(icon_pressed_cb cb)
{
	s_info.icon_pressed_cb = cb;
}

/*
 * @brief Destroys the main window.
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/*
 * @brief Creates path to the given resource file by concatenation of the basic resource path and the given file_name.
 * @param[file_name]: File name or path relative to the resource directory.
 * @return: The absolute path to the resource with given file_name is returned.
 */
static char *_create_resource_path(const char *file_name)
{
	static char res_path_buff[PATH_MAX] = {0,};
	char *res_path = NULL;

	res_path = app_get_resource_path();
	if (res_path == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get resource path.");
		return NULL;
	}

	snprintf(res_path_buff, PATH_MAX, "%s%s", res_path, file_name);
	free(res_path);

	return &res_path_buff[0];
}

/*
 * @brief Creates the application's layout.
 * @return: The Evas_Object of the layout created.
 */
static Evas_Object *_create_layout(void)
{
	char *edj_path = NULL;

	edj_path = _create_resource_path(MAIN_EDJ);

	Evas_Object *layout = view_create_layout_for_part(s_info.win, edj_path, "main", "default");
	if (!layout)
		return NULL;

	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(layout, s_info.w, s_info.h);
	evas_object_resize(layout, s_info.w, s_info.h);
	evas_object_show(layout);

	elm_object_signal_callback_add(layout, "mouse,down,1", PART_MISSED_CALLS, _missed_calls_mouse_down_cb, NULL);
	elm_object_signal_callback_add(layout, "mouse,up,1", PART_MISSED_CALLS, _missed_calls_mouse_up_cb, NULL);
	elm_object_signal_callback_add(layout, "mouse,down,1", PART_UNREAD_MESSAGES, _unread_messages_mouse_down_cb, NULL);
	elm_object_signal_callback_add(layout, "mouse,up,1", PART_UNREAD_MESSAGES, _unread_messages_mouse_up_cb, NULL);

	return layout;
}

/*
 * @brief Sends a signal to the EDJE script.
 * @param[layout]: the target layout for the signal.
 * @param[target_part]: the target part's name within the layout for the signal.
 * @param[signal_name]: the name of the signal to be sent.
 */
static void _emit_signal(Evas_Object *layout, const char *target_part, const char *signal_name)
{
	if (!layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid input argument.");
		return;
	}

	elm_object_signal_emit(layout, signal_name, target_part);
}

/*
 * @brief Sends a message to the EDJE script in order to set the badge's value. This function handles the
 * 'missed calls' and the 'unread messages' badges depending on the message_id.
 * @param[message_id]: the message id. Available messages: MSG_ID_SET_BADGE_MISSED_CALLS, MSG_ID_SET_BADGE_UNREAD_MESSAGES.
 */
static void _set_badge(int message_id, int badge_count)
{
	Edje_Message_Int msg = {0,};

	if (!s_info.layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid layout object.");
		return;
	}

	msg.val = badge_count;

	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT, message_id, &msg);
}

/*
 * @brief The callback function invoked on mouse down event over the 'missed calls' icon.
 * @param[data]: the user data passed to the elm_object_signal_callback_add function.
 * @param[obj]: the calling object (main layout in this case).
 * @param[emission]: the signal emitted from EDJE resulting with the function invocation.
 * @param[source]: the part name where the signal was emitted from.
 */
static void _missed_calls_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_emit_signal(obj, PART_MISSED_CALLS, SIGNAL_MISSED_CALLS_PRESS);
}

/*
 * @brief The callback function invoked on mouse up event over the 'missed calls' icon.
 * @param[data]: the user data passed to the elm_object_signal_callback_add function.
 * @param[obj]: the calling object (main layout in this case).
 * @param[emission]: the signal emitted from EDJE resulting with the function invocation.
 * @param[source]: the part name where the signal was emitted from.
 */
static void _missed_calls_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_emit_signal(obj, PART_MISSED_CALLS, SIGNAL_MISSED_CALLS_UNPRESS);

	if (s_info.icon_pressed_cb)
		s_info.icon_pressed_cb(VIEW_ICON_ID_MISSED_CALLS);
}

/*
 * @brief The callback function invoked on mouse down event over the 'unread messages' icon.
 * @param[data]: the user data passed to the elm_object_signal_callback_add function.
 * @param[obj]: the calling object (main layout in this case).
 * @param[emission]: the signal emitted from EDJE resulting with the function invocation.
 * @param[source]: the part name where the signal was emitted from.
 */
static void _unread_messages_mouse_down_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_emit_signal(obj, PART_UNREAD_MESSAGES, SIGNAL_UNREAD_MESSAGES_PRESS);
}

/*
 * @brief The callback function invoked on mouse up event over the 'unread messages' icon.
 * @param[data]: the user data passed to the elm_object_signal_callback_add function.
 * @param[obj]: the calling object (main layout in this case).
 * @param[emission]: the signal emitted from EDJE resulting with the function invocation.
 * @param[source]: the part name where the signal was emitted from.
 */
static void _unread_messages_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_emit_signal(obj, PART_UNREAD_MESSAGES, SIGNAL_UNREAD_MESSAGES_UNPRESS);

	if (s_info.icon_pressed_cb)
		s_info.icon_pressed_cb(VIEW_ICON_ID_UNREAD_MESSAGES);
}
