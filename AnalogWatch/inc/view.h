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

#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>
#include "analogwatch.h"

typedef enum {VIEW_ICON_ID_MISSED_CALLS, VIEW_ICON_ID_UNREAD_MESSAGES} view_icon_id_t;
typedef void (*icon_pressed_cb)(view_icon_id_t id);

void view_create_with_size(int width, int height);
void view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name);
void view_set_display_time(current_time_t current_time);
void view_toggle_ambient_mode(bool ambient_mode);
void view_set_bagde_missed_calls(int count);
void view_set_bagde_unread_messages(int count);
void view_set_icon_pressed_cb(icon_pressed_cb cb);
void view_destroy(void);

#endif
