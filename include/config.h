/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <stddef.h>

// Maximum number of concurrent animations allowed in the system
constexpr int MAX_ANIMATION_COUNT = 35; 

// Maximum length of text strings for widgets that contains text
constexpr int MAX_TEXT_LENGTH = 30;

// Maximum number of apps that can be registered
constexpr int MAX_APP_NUM = 10;

// Maximum number of applications held by the view stack
constexpr int MAX_VIEW_DEPTH = 8;

// Fixed application storage. Every concrete IApplication must fit one slot.
constexpr size_t APPLICATION_POOL_SLOT_SIZE = 1536;
constexpr size_t APPLICATION_POOL_SLOT_ALIGNMENT = 16;
constexpr size_t APPLICATION_POOL_CAPACITY = MAX_VIEW_DEPTH;

// Maximum number of items displayed by an IconView
constexpr int MAX_ICONVIEW_ITEMS = MAX_APP_NUM;

// Inline storage reserved for callbacks and captured lambdas
constexpr int CALLBACK_STORAGE_SIZE = 32;

// Maximum slots in a single AppView
constexpr int MAX_APPVIEW_SLOT_NUM = 10;

// Maximum slots in a ListView
constexpr int MAX_LISTVIEW_SLOT_NUM = 30;

// Maximum number of characters in a ListItem name
constexpr int MAX_LISTITEM_NAME_NUM = 30;

// Number of items shown per page in a ListView
constexpr int LISTVIEW_ITEMS_PER_PAGE = 4;

// Maximum depth of nested ListViews
constexpr int MAX_LISTVIEW_DEPTH = 6;

// Stack size for callback animations (for internal coroutine/animation safety)
constexpr int CALLBACK_ANIMATION_STACK_SIZE = 2;

// Maximum number of popups that can appear simultaneously
constexpr int MAX_POPUP_NUM = 3;

// Maximum number of widgets displayed on screen at the same time
constexpr int MAX_ONSCREEN_WIDGET_NUM = 6;

// Maximum width (number of digits) for fixed-width integer display
constexpr int MAX_INT_FIXED_WIDTH = 6;

constexpr int MAX_COROUTINE_NUM = 10; // Maximum number of concurrent coroutines allowed in the system


/* Input Event Router */

/* FocusManager */
#define FOCUS_MANAGER_NAVI_PREV     InputEvent::LEFT
#define FOCUS_MANAGER_NAVI_NEXT     InputEvent::RIGHT
#define FOCUS_MANAGER_NAVI_SELECT   InputEvent::SELECT

/* ListView */
#define LISTVIEW_NAVI_UP         InputEvent::LEFT
#define LISTVIEW_NAVI_DOWN       InputEvent::RIGHT
#define LISTVIEW_NAVI_SELECT     InputEvent::SELECT
#define LISTVIEW_NAVI_BACK       InputEvent::BACK
// #define LISTVIEW_NAVI_LEFT       InputEvent::LEFT
// #define LISTVIEW_NAVI_RIGHT      InputEvent::RIGHT

/* IconView */
#define ICONVIEW_NAVI_LEFT     InputEvent::LEFT
#define ICONVIEW_NAVI_RIGHT    InputEvent::RIGHT
#define ICONVIEW_NAVI_SELECT   InputEvent::SELECT
#define ICONVIEW_NAVI_BACK     InputEvent::BACK

/* NumScroll */
#define NUMSCROLL_NAVI_UP       InputEvent::RIGHT
#define NUMSCROLL_NAVI_DOWN     InputEvent::LEFT
#define NUMSCROLL_NAVI_SELECT   InputEvent::SELECT
