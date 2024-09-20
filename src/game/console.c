#include "game/console.h"

#include "game/clock.h"
#include "game/game_string.h"
#include "game/input.h"
#include "game/output.h"
#include "game/text.h"
#include "global/const.h"
#include "global/types.h"

#include <libtrx/game/console/common.h>
#include <libtrx/log.h>
#include <libtrx/memory.h>
#include <libtrx/utils.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOG_LINES 10
#define MAX_PROMPT_LENGTH 100
#define HOVER_DELAY_CPS 5
#define MARGIN 5
#define PADDING 3

static bool m_IsOpened = false;
static bool m_AreAnyLogsOnScreen = false;

static struct {
    char text[MAX_PROMPT_LENGTH];
    int32_t caret;
    TEXTSTRING *prompt_ts;
    TEXTSTRING *caret_ts;
} m_Prompt = { 0 };

static struct {
    double expire_at;
    TEXTSTRING *ts;
} m_Logs[MAX_LOG_LINES] = { 0 };

static const double m_PromptScale = 1.0;
static const double m_LogScale = 0.8;
static const char m_ValidPromptChars[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.- ";

static void M_ShutdownPrompt(void);
static void M_ShutdownLogs(void);
static void M_UpdatePromptTextstring(void);
static void M_UpdateCaretTextstring(void);
static COMMAND_RESULT M_Eval(const char *cmdline);

extern CONSOLE_COMMAND *g_ConsoleCommands[];
static void M_ShutdownPrompt(void)
{
    if (m_Prompt.prompt_ts != NULL) {
        Text_Remove(m_Prompt.prompt_ts);
        m_Prompt.prompt_ts = NULL;
    }
    if (m_Prompt.caret_ts != NULL) {
        Text_Remove(m_Prompt.caret_ts);
        m_Prompt.caret_ts = NULL;
    }
}

static void M_ShutdownLogs(void)
{
    for (int32_t i = 0; i < MAX_LOG_LINES; i++) {
        Text_Remove(m_Logs[i].ts);
        m_Logs[i].ts = NULL;
    }
}

static void M_UpdatePromptTextstring(void)
{
    Text_ChangeText(m_Prompt.prompt_ts, m_Prompt.text);
}

static void M_UpdateCaretTextstring(void)
{
    const char old = m_Prompt.prompt_ts->text[m_Prompt.caret];
    m_Prompt.prompt_ts->text[m_Prompt.caret] = '\0';
    const int32_t width =
        Text_GetWidth(m_Prompt.prompt_ts) * PHD_ONE / Text_GetScaleH(PHD_ONE);
    m_Prompt.prompt_ts->text[m_Prompt.caret] = old;
    Text_SetPos(m_Prompt.caret_ts, MARGIN + width, -MARGIN);
}

static COMMAND_RESULT M_Eval(const char *const cmdline)
{
    const char *args = NULL;
    const CONSOLE_COMMAND *matching_cmd = NULL;

    for (int32_t i = 0;; i++) {
        CONSOLE_COMMAND *cur_cmd = g_ConsoleCommands[i];
        if (cur_cmd == NULL) {
            break;
        }
        if (strstr(cmdline, cur_cmd->prefix) != cmdline) {
            continue;
        }

        if (cmdline[strlen(cur_cmd->prefix)] == ' ') {
            args = cmdline + strlen(cur_cmd->prefix) + 1;
        } else if (cmdline[strlen(cur_cmd->prefix)] == '\0') {
            args = "";
        } else {
            continue;
        }

        matching_cmd = cur_cmd;
        break;
    }

    if (matching_cmd == NULL) {
        Console_Log(GS(OSD_UNKNOWN_COMMAND), cmdline);
        return CR_BAD_INVOCATION;
    }

    const COMMAND_RESULT result = matching_cmd->proc(args);
    switch (result) {
    case CR_BAD_INVOCATION:
        Console_Log(GS(OSD_COMMAND_BAD_INVOCATION), cmdline);
        break;
    case CR_UNAVAILABLE:
        Console_Log(GS(OSD_COMMAND_UNAVAILABLE));
        break;
    case CR_SUCCESS:
    case CR_FAILURE:
        // logging these statuses are supposed to be
        // handled by the console commands themselves
        break;
    }
    return result;
}

void Console_Init(void)
{
    for (int32_t i = 0; i < MAX_LOG_LINES; i++) {
        m_Logs[i].expire_at = 0;
        m_Logs[i].ts = Text_Create(MARGIN, -MARGIN, 0, "");
        Text_SetScale(m_Logs[i].ts, PHD_ONE * m_LogScale, PHD_ONE * m_LogScale);
        Text_AlignBottom(m_Logs[i].ts, true);
        Text_SetMultiline(m_Logs[i].ts, true);
    }

    // in case this is called after text reinitializes its textstrings,
    // fix the broken pointers
    if (strcmp(m_Prompt.text, "") != 0) {
        Console_Open();
    }
}

void Console_Shutdown(void)
{
    m_IsOpened = false;
    M_ShutdownPrompt();
    M_ShutdownLogs();
}

void Console_Open(void)
{
    if (m_IsOpened) {
        M_ShutdownPrompt();
    } else {
        LOG_DEBUG("opening console!");
    }
    m_IsOpened = true;

    m_Prompt.caret = strlen(m_Prompt.text);

    m_Prompt.caret_ts = Text_Create(MARGIN, -MARGIN, 0, "\x11");
    Text_SetScale(
        m_Prompt.caret_ts, PHD_ONE * m_PromptScale, PHD_ONE * m_PromptScale);
    Text_AlignBottom(m_Prompt.caret_ts, true);
    Text_Flash(m_Prompt.caret_ts, 1, 20);

    m_Prompt.prompt_ts = Text_Create(MARGIN, -MARGIN, 0, m_Prompt.text);
    Text_SetScale(
        m_Prompt.prompt_ts, PHD_ONE * m_PromptScale, PHD_ONE * m_PromptScale);
    Text_AlignBottom(m_Prompt.prompt_ts, true);

    M_UpdateCaretTextstring();
}

void Console_Close(void)
{
    LOG_DEBUG("closing console!");
    m_IsOpened = false;
    strcpy(m_Prompt.text, "");
    M_ShutdownPrompt();
}

bool Console_IsOpened(void)
{
    return m_IsOpened;
}

void Console_Confirm(void)
{
    if (strcmp(m_Prompt.text, "") == 0) {
        Console_Close();
        return;
    }

    LOG_INFO("executing command: %s", m_Prompt.text);
    M_Eval(m_Prompt.text);
    Console_Close();
}

bool Console_HandleKeyDown(const uint32_t key)
{
    // TODO: make it possible to turn the console off
#if 0
    if (!g_Config.enable_console) {
        return false;
    }
#endif

    switch (key) {
    case VK_LEFT:
        if (!m_IsOpened) {
            return false;
        }
        if (m_Prompt.caret > 0) {
            m_Prompt.caret--;
            M_UpdateCaretTextstring();
        }
        return true;

    case VK_RIGHT:
        if (!m_IsOpened) {
            return false;
        }
        if (m_Prompt.caret < (int32_t)strlen(m_Prompt.text)) {
            m_Prompt.caret++;
            M_UpdateCaretTextstring();
        }
        return true;

    case VK_HOME:
        if (!m_IsOpened) {
            return false;
        }
        m_Prompt.caret = 0;
        M_UpdateCaretTextstring();
        return true;

    case VK_END:
        if (!m_IsOpened) {
            return false;
        }
        m_Prompt.caret = strlen(m_Prompt.text);
        M_UpdateCaretTextstring();
        return true;

    case VK_BACK:
        if (!m_IsOpened) {
            return false;
        }
        if (m_Prompt.caret > 0) {
            for (int32_t i = m_Prompt.caret; i < MAX_PROMPT_LENGTH; i++) {
                m_Prompt.text[i - 1] = m_Prompt.text[i];
            }
            m_Prompt.caret--;
            M_UpdatePromptTextstring();
            M_UpdateCaretTextstring();
        }
        return true;
    }

    return false;
}

void Console_HandleChar(const uint32_t char_)
{
    if (!m_IsOpened) {
        return;
    }

    char insert_string[2];
    insert_string[0] = char_;
    insert_string[1] = '\0';

    if (strlen(insert_string) != 1
        || !strstr(m_ValidPromptChars, insert_string)) {
        return;
    }

    const size_t insert_length = strlen(insert_string);
    const size_t available_space =
        MAX_PROMPT_LENGTH - strlen(m_Prompt.text) - 1;

    if (insert_length > available_space) {
        return;
    }

    for (int32_t i = strlen(m_Prompt.text); i >= m_Prompt.caret; i--) {
        m_Prompt.text[i + insert_length] = m_Prompt.text[i];
    }

    memcpy(m_Prompt.text + m_Prompt.caret, insert_string, insert_length);

    m_Prompt.caret += insert_length;
    m_Prompt.text[MAX_PROMPT_LENGTH - 1] = '\0';
    M_UpdatePromptTextstring();
    M_UpdateCaretTextstring();
}

void Console_Log(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    const size_t text_length = vsnprintf(NULL, 0, fmt, va);
    char *text = Memory_Alloc(text_length + 1);
    va_end(va);

    va_start(va, fmt);
    vsnprintf(text, text_length + 1, fmt, va);
    va_end(va);

    LOG_INFO("%s", text);
    int32_t dst_idx = -1;
    for (int32_t i = MAX_LOG_LINES - 1; i > 0; i--) {
        if (m_Logs[i].ts == NULL) {
            continue;
        }
        Text_ChangeText(m_Logs[i].ts, m_Logs[i - 1].ts->text);
        m_Logs[i].expire_at = m_Logs[i - 1].expire_at;
    }

    if (m_Logs[0].ts == NULL) {
        return;
    }

    m_Logs[0].expire_at =
        Clock_GetHighPrecisionCounter() + 1000 * strlen(text) / HOVER_DELAY_CPS;
    Text_ChangeText(m_Logs[0].ts, text);
    int32_t y = -MARGIN
        - Text_GetHeight(m_Prompt.prompt_ts) * m_PromptScale * PHD_ONE
            / Text_GetScaleV(PHD_ONE);

    for (int32_t i = 0; i < MAX_LOG_LINES; i++) {
        y -= PADDING;
        y -= Text_GetHeight(m_Logs[i].ts) * m_LogScale * PHD_ONE
            / Text_GetScaleV(PHD_ONE);
        Text_SetPos(m_Logs[i].ts, MARGIN, y);
    }

    m_AreAnyLogsOnScreen = true;
    Memory_FreePointer(&text);
}

void Console_ScrollLogs(void)
{
    int32_t i = MAX_LOG_LINES - 1;
    while (i >= 0 && !m_Logs[i].expire_at) {
        i--;
    }

    while (i >= 0 && m_Logs[i].expire_at
           && Clock_GetHighPrecisionCounter() >= m_Logs[i].expire_at) {
        m_Logs[i].expire_at = 0;
        Text_ChangeText(m_Logs[i].ts, "");
        i--;
    }

    m_AreAnyLogsOnScreen = i >= 0;
}

void Console_Draw(void)
{
    Console_ScrollLogs();

#if 0
// TODO: draw screen quad
    if (m_IsOpened || m_AreAnyLogsOnScreen) {
        int32_t sx = 0;
        int32_t sw = Viewport_GetWidth();
        int32_t sh = Screen_GetRenderScale(
            // not entirely accurate, but good enough
            TEXT_HEIGHT * m_PromptScale
                + MAX_LOG_LINES * TEXT_HEIGHT * m_LogScale,
            RSR_TEXT);
        int32_t sy = Viewport_GetHeight() - sh;

        RGBA_8888 top = { 0, 0, 0, 0 };
        RGBA_8888 bottom = { 0, 0, 0, 196 };

        Output_DrawScreenGradientQuad(sx, sy, sw, sh, top, top, bottom, bottom);
    }
#endif

    // achieved by Text_Draw()
#if 0
    if (m_Prompt.prompt_ts) {
        Text_DrawText(m_Prompt.prompt_ts);
    }
    if (m_Prompt.caret_ts) {
        Text_DrawText(m_Prompt.caret_ts);
    }
    for (int32_t i = 0; i < MAX_LOG_LINES; i++) {
        if (m_Logs[i].ts) {
            Text_DrawText(m_Logs[i].ts);
        }
    }
#endif
}
