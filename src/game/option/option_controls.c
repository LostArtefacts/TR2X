#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/utils.h>

#include <dinput.h>

static void Option_Controls_InitText(void);
static void Option_Controls_ShutdownText(void);

static void Option_Controls_Control_Navigate(void);
static void Option_Controls_Control_WaitToListen(void);
static void Option_Controls_Control_Listen(void);
static void Option_Controls_Control_EndListen(void);

typedef enum {
    STATE_NAVIGATE,
    STATE_WAIT_TO_LISTEN,
    STATE_LISTEN,
    STATE_END_LISTEN,
} STATE;

static int32_t m_Cursor = 0;
static int32_t m_State = STATE_NAVIGATE;

static void Option_Controls_InitText(void)
{
    g_ControlsText[0] = Text_Create(
        0, -50, 0,
        g_GF_PCStrings
            [g_LayoutPage == 0 ? GF_S_PC_DEFAULT_KEYS : GF_S_PC_USER_KEYS]);
    Text_CentreH(g_ControlsText[0], 1);
    Text_CentreV(g_ControlsText[0], 1);
    Option_Controls_ShowControls();
    m_Cursor = -1;
    Text_AddBackground(
        g_ControlsText[0], 0, 0, 0, 0, 48, INV_COLOR_BLACK, 0, 0);
    Text_AddOutline(g_ControlsText[0], 1, INV_COLOR_BLUE, 0, 0);
}

static void Option_Controls_ShutdownText(void)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Remove(g_ControlsTextA[i]);
        g_ControlsTextA[i] = NULL;

        Text_Remove(g_ControlsTextB[i]);
        g_ControlsTextB[i] = NULL;
    }

    for (int32_t i = 0; i < 2; i++) {
        Text_Remove(g_ControlsText[i]);
        g_ControlsText[i] = NULL;
    }
}

static void Option_Controls_Control_Navigate(void)
{
    if (g_InputDB & (IN_LEFT | IN_RIGHT)) {
        if (m_Cursor == -1) {
            g_LayoutPage = 1 - g_LayoutPage;
            Option_Controls_UpdateText();
            Option_Controls_FlashConflicts();
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
            if (m_Cursor < 7) {
                m_Cursor += 7;
            } else if (m_Cursor < 14) {
                m_Cursor -= 7;
            } else {
                m_Cursor = 7;
            }
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, INV_COLOR_BLACK, 0,
                0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, INV_COLOR_BLUE, 0, 0);
        }
    } else if (
        (g_InputDB & IN_DESELECT)
        || ((g_InputDB & IN_SELECT) && m_Cursor == -1)) {
        Option_Controls_Shutdown();
        return;
    }

    if (g_LayoutPage == 0) {
        g_Input = 0;
        g_InputDB = 0;
        return;
    }

    if (g_InputDB & IN_SELECT) {
        m_State = STATE_WAIT_TO_LISTEN;
        g_ControlsTextB[m_Cursor]->pos.z = 16;
        Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
        Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        g_ControlsTextA[m_Cursor]->pos.z = 0;
        Text_AddBackground(g_ControlsTextA[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
        Text_AddOutline(g_ControlsTextA[m_Cursor], 1, 15, 0, 0);
    } else if (g_InputDB & IN_FORWARD) {
        if (m_Cursor == -1) {
            Text_RemoveBackground(g_ControlsText[0]);
            Text_RemoveOutline(g_ControlsText[0]);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        }

        m_Cursor--;
        if (m_Cursor < -1) {
            m_Cursor = 13;
        }

        if (m_Cursor == -1) {
            Text_AddBackground(g_ControlsText[0], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsText[0], 1, 15, 0, 0);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);
        }
        g_Input = 0;
        g_InputDB = 0;
    } else if (g_InputDB & IN_BACK) {
        if (m_Cursor == -1) {
            Text_RemoveBackground(g_ControlsText[0]);
            Text_RemoveOutline(g_ControlsText[0]);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        }

        m_Cursor++;
        if (m_Cursor > 13) {
            m_Cursor = -1;
        }

        if (m_Cursor == -1) {
            Text_AddBackground(g_ControlsText[0], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsText[0], 1, 15, 0, 0);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);
        }
    }

    g_Input = 0;
    g_InputDB = 0;
}

static void Option_Controls_Control_WaitToListen(void)
{
    if (g_Input) {
        return;
    }

    m_State = STATE_LISTEN;
}

static void Option_Controls_Control_Listen(void)
{
    int32_t pressed = 0;

    if (g_JoyKeys) {
        for (int32_t i = 0; i < 32; i++) {
            if (g_JoyKeys & (1 << i)) {
                pressed = i;
                break;
            }
        }
        if (!pressed) {
            return;
        }
        pressed += 0x100;
    } else {
        for (int32_t i = 0; i < 256; i++) {
            if (g_DIKeys[i] & 0x80) {
                pressed = i;
                break;
            }
        }
        if (!pressed) {
            return;
        }
    }

    if (!pressed
        // clang-format off
        || g_KeyNames[pressed] == NULL
        || pressed == DIK_RETURN
        || pressed == DIK_LEFT
        || pressed == DIK_RIGHT
        || pressed == DIK_UP
        || pressed == DIK_DOWN
        // clang-format on
    ) {
        g_Input = 0;
        g_InputDB = 0;
        return;
    }

    if (pressed != DIK_ESCAPE) {
        g_Layout[g_LayoutPage].key[m_Cursor] = pressed;
        Text_ChangeText(g_ControlsTextA[m_Cursor], g_KeyNames[pressed]);
    }

    g_ControlsTextA[m_Cursor]->pos.z = 16;
    Text_RemoveBackground(g_ControlsTextA[m_Cursor]);
    Text_RemoveOutline(g_ControlsTextA[m_Cursor]);

    g_ControlsTextB[m_Cursor]->pos.z = 0;
    Text_AddBackground(g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
    Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);

    m_State = STATE_END_LISTEN;
    Option_Controls_FlashConflicts();

    g_Input = 0;
    g_InputDB = 0;
}

static void Option_Controls_Control_EndListen(void)
{
    if (g_Layout[g_LayoutPage].key[m_Cursor] & 0x100) {
        if (!(g_JoyKeys & (1 << g_Layout[g_LayoutPage].key[m_Cursor]))) {
            m_State = STATE_NAVIGATE;
        }
    } else if ((g_DIKeys[g_Layout[g_LayoutPage].key[m_Cursor]] & 0x80u) == 0) {
        m_State = STATE_NAVIGATE;
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LCONTROL) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RCONTROL;
        }
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LSHIFT) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RSHIFT;
        }
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LMENU) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RMENU;
        }
        Option_Controls_FlashConflicts();
    }
    g_Input = 0;
    g_InputDB = 0;
}

void __cdecl Option_Controls_FlashConflicts(void)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Flash(g_ControlsTextA[i], 0, 0);
        for (int32_t j = 0; j < 14; j++) {
            const uint16_t key1 = g_Layout[g_LayoutPage].key[i];
            const uint16_t key2 = g_Layout[g_LayoutPage].key[j];
            if (i != j && key1 == key2) {
                Text_Flash(g_ControlsTextA[i], 1, 20);
            }
        }
    }
}

void __cdecl Option_Controls_DefaultConflict(void)
{
    for (int32_t i = 0; i < 14; i++) {
        g_ConflictLayout[i] = false;
        for (int32_t j = 0; j < 14; j++) {
            uint16_t key1 = g_Layout[0].key[i];
            uint16_t key2 = g_Layout[1].key[j];
            if (key1 == key2) {
                g_ConflictLayout[i] = true;
            }
        }
    }
}

void Option_Controls_Shutdown(void)
{
    Option_Controls_ShutdownText();
    Option_Controls_DefaultConflict();
}

void __cdecl Option_Controls(INVENTORY_ITEM *const item)
{
    if (g_ControlsText[0] == NULL) {
        Option_Controls_InitText();
    }

    switch (m_State) {
    case STATE_NAVIGATE:
        Option_Controls_Control_Navigate();
        break;

    case STATE_WAIT_TO_LISTEN:
        Option_Controls_Control_WaitToListen();
        break;

    case STATE_LISTEN:
        Option_Controls_Control_Listen();
        break;

    case STATE_END_LISTEN:
        Option_Controls_Control_EndListen();
        break;

    default:
        g_Input = 0;
        g_InputDB = 0;
    }
}

void __cdecl Option_Controls_ShowControls(void)
{
    int32_t right_col = GetRenderWidth() / 2;
    CLAMPG(right_col, 320);

    if (g_ControlsTextA[0] == NULL) {
        int32_t left_col = right_col - 140;
        if (right_col >= 320) {
            left_col = right_col - 200;
        }

        const CONTROL_LAYOUT *const layout = &g_Layout[g_LayoutPage];

        g_ControlsTextA[0] =
            Text_Create(left_col, -25, 16, g_KeyNames[layout->key[0]]);
        g_ControlsTextA[1] =
            Text_Create(left_col, -10, 16, g_KeyNames[layout->key[1]]);
        g_ControlsTextA[2] =
            Text_Create(left_col, 5, 16, g_KeyNames[layout->key[2]]);
        g_ControlsTextA[3] =
            Text_Create(left_col, 20, 16, g_KeyNames[layout->key[3]]);
        g_ControlsTextA[4] =
            Text_Create(left_col, 35, 16, g_KeyNames[layout->key[4]]);
        g_ControlsTextA[5] =
            Text_Create(left_col, 50, 16, g_KeyNames[layout->key[5]]);
        g_ControlsTextA[6] =
            Text_Create(left_col, 65, 16, g_KeyNames[layout->key[6]]);
        g_ControlsTextA[7] =
            Text_Create(right_col + 10, -25, 16, g_KeyNames[layout->key[7]]);
        g_ControlsTextA[8] =
            Text_Create(right_col + 10, -10, 16, g_KeyNames[layout->key[8]]);
        g_ControlsTextA[9] =
            Text_Create(right_col + 10, 5, 16, g_KeyNames[layout->key[9]]);
        g_ControlsTextA[10] =
            Text_Create(right_col + 10, 20, 16, g_KeyNames[layout->key[10]]);
        g_ControlsTextA[11] =
            Text_Create(right_col + 10, 35, 16, g_KeyNames[layout->key[11]]);
        g_ControlsTextA[12] =
            Text_Create(right_col + 10, 50, 16, g_KeyNames[layout->key[12]]);
        g_ControlsTextA[13] =
            Text_Create(right_col + 10, 65, 16, g_KeyNames[layout->key[13]]);

        for (int32_t i = 0; i < 14; i++) {
            Text_CentreV(g_ControlsTextA[i], true);
        }

        m_Cursor = 0;
    }

    if (g_ControlsTextB[0] == NULL) {
        int32_t left_col = right_col - 70;
        if (right_col >= 320) {
            left_col = right_col - 130;
        }

        g_ControlsTextB[0] = Text_Create(
            left_col, -25, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_RUN]);
        g_ControlsTextB[1] = Text_Create(
            left_col, -10, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_BACK]);
        g_ControlsTextB[2] = Text_Create(
            left_col, 5, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_LEFT]);
        g_ControlsTextB[3] = Text_Create(
            left_col, 20, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_RIGHT]);
        g_ControlsTextB[4] = Text_Create(
            left_col, 35, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_STEP_LEFT]);
        g_ControlsTextB[5] = Text_Create(
            left_col, 50, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_STEP_RIGHT]);
        g_ControlsTextB[6] = Text_Create(
            left_col, 65, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_WALK]);
        g_ControlsTextB[7] = Text_Create(
            right_col + 90, -25, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_JUMP]);
        g_ControlsTextB[8] = Text_Create(
            right_col + 90, -10, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_ACTION]);
        g_ControlsTextB[9] = Text_Create(
            right_col + 90, 5, 16,
            g_GF_GameStrings[GF_S_GAME_KEYMAP_DRAW_WEAPON]);
        g_ControlsTextB[10] = Text_Create(
            right_col + 90, 20, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_FLARE]);
        g_ControlsTextB[11] = Text_Create(
            right_col + 90, 35, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_LOOK]);
        g_ControlsTextB[12] = Text_Create(
            right_col + 90, 50, 16, g_GF_GameStrings[GF_S_GAME_KEYMAP_ROLL]);
        g_ControlsTextB[13] = Text_Create(
            right_col + 90, 65, 16,
            g_GF_GameStrings[GF_S_GAME_KEYMAP_INVENTORY]);

        for (int32_t i = 0; i < 14; i++) {
            Text_CentreV(g_ControlsTextB[i], true);
        }
    }

    g_ControlsText[1] = Text_Create(0, -55, 0, " ");
    Text_CentreV(g_ControlsText[1], 1);
    Text_CentreH(g_ControlsText[1], 1);
    Text_AddOutline(g_ControlsText[1], 1, 15, 0, 0);

    if (right_col < 320) {
        for (int32_t i = 0; i < 14; ++i) {
            Text_SetScale(g_ControlsTextA[i], PHD_ONE * 0.5, PHD_ONE);
            Text_SetScale(g_ControlsTextB[i], PHD_ONE * 0.5, PHD_ONE);
        }
        Text_AddBackground(g_ControlsText[1], 300, 140, 0, 0, 48, 0, 0, 0);
    } else {
        Text_AddBackground(g_ControlsText[1], 420, 150, 0, 0, 48, 0, 0, 0);
    }
}

void __cdecl Option_Controls_UpdateText(void)
{
    Text_ChangeText(
        g_ControlsText[0],
        g_GF_PCStrings
            [g_LayoutPage == 0 ? GF_S_PC_DEFAULT_KEYS : GF_S_PC_USER_KEYS]);

    for (int32_t i = 0; i < 14; i++) {
        const uint16_t key = g_Layout[g_LayoutPage].key[i];
        if (g_KeyNames[key] != NULL) {
            Text_ChangeText(g_ControlsTextA[i], g_KeyNames[key]);
        } else {
            Text_ChangeText(g_ControlsTextA[i], "BAD");
        }
    }
}
