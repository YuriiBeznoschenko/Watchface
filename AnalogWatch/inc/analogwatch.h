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

#if !defined(_ANALOGWATCH_H_)
#define _ANALOGWATCH_H_

#include <app.h>
#include <dlog.h>

struct _current_time {
	int hour;
	int minute;
	int second;
};

typedef struct _current_time current_time_t;

#if !defined(PACKAGE)
#define PACKAGE "org.example.analogwatch"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "analogwatch"

#endif
